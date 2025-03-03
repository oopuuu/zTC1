/**
 ******************************************************************************
 * @file    mico_mdns.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This header contains function called by mdns protocol operation
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mico_mdns.h"
#include "StringUtils.h"
#include "SocketUtils.h"

/** "224.0.0.251" */
#define INADDR_MULTICAST_MDNS  ((uint32_t)0xE00000FBUL)

static int mDNS_fd = -1;
static bool bonjour_instance = false;

typedef struct
{
  char*               hostname;
  char*               instance_name;
  char*               service_name;
  char*               txt_att;
  char                instance_name_suffix[4];
  WiFi_Interface      interface;
  uint32_t            ttl;
  uint16_t            port;
  uint8_t             count_down;
  mdns_record_state_t state;
} dns_sd_service_record_t;

#define APP_Available_Offset               0
#define Support_TLV_Config_Offset          2

#define Problem_Detected_Offset            0
#define Not_Configured_Offset              1


#define SERVICE_QUERY_NAME             "_services._dns-sd._udp.local."

//#define mdns_utils_log(M, ...) custom_log("mDNS Utils", M, ##__VA_ARGS__)
//#define mdns_utils_log_trace() custom_log_trace("mDNS Utils")

#define mdns_utils_log(M, ...)
#define mdns_utils_log_trace()


static dns_sd_service_record_t   available_services[ MAX_RECORD_COUNT ];
static uint8_t	available_service_count = MAX_RECORD_COUNT;

static int dns_get_next_question( dns_message_iterator_t* iter, dns_question_t* q, dns_name_t* name );
static int dns_compare_name_to_string( dns_name_t* name, const char* string );
static int dns_create_message( dns_message_iterator_t* message, uint16_t size );
static void dns_write_header( dns_message_iterator_t* iter, uint16_t id, uint16_t flags, uint16_t question_count, uint16_t answer_count, uint16_t authorative_count );
static void dns_write_record( dns_message_iterator_t* iter, const char* name, uint16_t record_class, uint16_t record_type, uint32_t ttl, uint8_t* rdata );
static void mdns_send_message(int fd, dns_message_iterator_t* message );
static void dns_free_message( dns_message_iterator_t* message );
static void mdns_process_query(int fd, dns_name_t* name, dns_question_t* question, dns_message_iterator_t* source );
static void dns_write_record( dns_message_iterator_t* iter, const char* name, uint16_t record_class, uint16_t record_type, uint32_t ttl, uint8_t* rdata );
static void dns_write_uint16( dns_message_iterator_t* iter, uint16_t data );
static void dns_write_uint32( dns_message_iterator_t* iter, uint32_t data );
static void dns_write_bytes( dns_message_iterator_t* iter, uint8_t* data, uint16_t length );
static uint16_t dns_read_uint16( dns_message_iterator_t* iter );
static void dns_skip_name( dns_message_iterator_t* iter );
static void dns_write_name( dns_message_iterator_t* iter, const char* src );

static OSStatus start_bonjour_service(void);

static mico_mutex_t bonjour_mutex = NULL;
static mico_semaphore_t update_state_sem = NULL;
static int update_state_fd = 0;
static mico_thread_t mfi_bonjour_thread_handler;
static void _bonjour_thread(uint32_t arg);

void process_dns_questions(int fd, dns_message_iterator_t* iter )
{
  dns_name_t name;
  dns_question_t question;
  dns_message_iterator_t response;
  IPStatusTypedef para;
  int a = 0;
  int question_processed;
  uint32_t myip;
  
  memset( &response, 0, sizeof(dns_message_iterator_t) );
  
  for ( a = 0; a < htons(iter->header->question_count); ++a )
  {
    if (iter->iter > iter->end)
      break;
    if(dns_get_next_question( iter, &question, &name )==0)
      break;
    question_processed = 0;
    switch ( question.question_type ){
    case RR_TYPE_PTR:
      if ( available_services != NULL ){
        // Check if its a query for all available services  
        if ( dns_compare_name_to_string( &name, SERVICE_QUERY_NAME ) ){
          int b = 0;
          if(dns_create_message( &response, 512 )) {
            dns_write_header(&response, iter->header->id, 0x8400, 0, available_service_count, 0 );          
            for ( b = 0; b < available_service_count; ++b ){
              dns_write_record( &response, SERVICE_QUERY_NAME, RR_CLASS_IN, RR_TYPE_PTR, 1500, (uint8_t*) available_services[b].service_name );
            }
            mdns_send_message(fd, &response );
            dns_free_message( &response );
            question_processed = 1;
          }
        }
        // else check if its one of our records
        else {
          int b = 0;
          for ( b = 0; b < available_service_count; ++b ){
            //printf("UDP multicast test: Recv a SERVICE Detail request: %s.\r\n", name);
            if ( dns_compare_name_to_string( &name, available_services[b].service_name )){
              
              if( available_services[b].state != RECORD_NORMAL )
                continue;

              micoWlanGetIPStatus(&para, available_services[b].interface);
              myip = inet_addr(para.ip);
              if( myip == 0 || myip == 0xFFFFFFFF) continue;
              
              // Send the PTR, TXT, SRV and A records
              if(dns_create_message( &response, 512 )){
                dns_write_header( &response, iter->header->id, 0x8400, 0, 4, 0 );
                //dns_write_record( &response, MFi_SERVICE_QUERY_NAME, RR_CLASS_IN, RR_TYPE_PTR, 1500, (u8*) available_services[b].service_name );
                dns_write_record( &response, available_services[b].service_name, RR_CLASS_IN, RR_TYPE_PTR, available_services[b].ttl, (uint8_t*) available_services[b].instance_name );
                dns_write_record( &response, available_services[b].instance_name, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_TXT, available_services[b].ttl, (uint8_t*) available_services[b].txt_att );
                dns_write_record( &response, available_services[b].instance_name, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_SRV, available_services[b].ttl, (uint8_t*) &available_services[b]);
                dns_write_record( &response, available_services[b].hostname, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_A, available_services[b].ttl, (uint8_t*) &myip);
                mdns_send_message(fd, &response );
                dns_free_message( &response );
                question_processed = 1;
              }
            }
          }
        }
      }
      break;
    }
    if (!question_processed ){
      mdns_process_query(fd, &name, &question, iter);
    }
  }
}

static void mdns_process_query(int fd, dns_name_t* name, 
                               dns_question_t* question, dns_message_iterator_t* source )
{
  dns_message_iterator_t response;
  IPStatusTypedef para;
  uint32_t myip;
  int b = 0;
  
  memset( &response, 0, sizeof(dns_message_iterator_t) );
  
  switch ( question->question_type )
  {
  case RR_QTYPE_ANY:
  case RR_TYPE_A:
    for ( b = 0; b < available_service_count; ++b ){
      if ( dns_compare_name_to_string( name, available_services[b].hostname ) ){				

        micoWlanGetIPStatus(&para, available_services[b].interface);
        myip = inet_addr(para.ip);
        if( myip == 0 || myip == 0xFFFFFFFF) continue;

        if(dns_create_message( &response, 256 )){
          dns_write_header( &response, source->header->id, 0x8400, 0, 1, 0 );
          dns_write_record( &response, available_services[b].hostname, RR_CLASS_IN | RR_CACHE_FLUSH, RR_TYPE_A, available_services[b].ttl, (uint8_t *)&myip);
          mdns_send_message(fd, &response );
          dns_free_message( &response );
          return;
        }
      }
    }    
  default:
   break;;
  }
}


static int dns_get_next_question( dns_message_iterator_t* iter, dns_question_t* q, dns_name_t* name )
{
  // Set the name pointers and then skip it
  name->start_of_name   = (uint8_t*) iter->iter;
  name->start_of_packet = (uint8_t*) iter->header;
  dns_skip_name( iter );
  if (iter->iter > iter->end)
    return 0;
  
  // Read the type and class
  q->question_type  = dns_read_uint16( iter );
  q->question_class = dns_read_uint16( iter );
  return 1;
}

static int dns_compare_name_to_string( dns_name_t* name, const char* string )
{
  uint8_t section_length;
  int finished = 0;
  int result   = 1;
  uint8_t* buffer 	  = name->start_of_name;
  char *temp;
  
  while ( !finished )
  {
    // Check if the name is compressed. If so, find the uncompressed version
    while ( (uint8_t)(*buffer) & 0xC0 )
    {
      uint16_t offset = ( *buffer++ ) << 8;
      offset += *buffer;
      offset &= 0x3FFF;
      buffer = name->start_of_packet + offset;
    }
    
    // Compare section
    section_length = *( buffer++ );
    temp = malloc(section_length+1);
    temp[section_length] = 0;
    memcpy(temp, buffer, section_length );
    free(temp);
    if ( strncmp( (char*) buffer, string, section_length ) )
    {
      result	 = 0;
      finished = 1;
    }
    string += section_length + 1;
    buffer += section_length;
    
    // Check if we've finished comparing
    if ( *buffer == 0 || *string == 0 )
    {
      finished = 1;
      // Check if one of the strings has more data
      if ( *buffer != 0 || *string != 0 )
      {
        result = 0;
      }
    }
  }
  
  return result;
}

static int dns_create_message( dns_message_iterator_t* message, uint16_t size )
{
  message->header = (dns_message_header_t*) malloc( size );
  if ( message->header == NULL )
  {
    return 0;
  }
  
  message->iter = (uint8_t *) message->header + sizeof(dns_message_header_t);
  return 1;
}

static void dns_free_message( dns_message_iterator_t* message )
{
  free(message->header);
  message->header = NULL;
}

static void dns_write_string( dns_message_iterator_t* iter, const char* src )
{
  uint8_t* segment_length_pointer;
  uint8_t  segment_length;
  
  while ( *src != 0 && (uint8_t)(*src) != 0xC0)
  {
    /* Remember where we need to store the segment length and reset the counter*/
    segment_length_pointer = iter->iter++;
    segment_length = 0;
    
    /* Copy bytes until '.' or end of string*/
    while ( *src != '.' && *src != 0 &&  (uint8_t)(*src) != 0xC0)
    {
      if (*src == '/')
        src++; // skip '/'
      
      *iter->iter++ = *src++;
      ++segment_length;
    }
    
    /* Store the length of the segment*/
    *segment_length_pointer = segment_length;
    
    /* Check if we stopped because of a '.', if so, skip it*/
    if ( *src == '.' )
    {
      ++src;
    }
    
  }
  
  if ( (uint8_t)(*src) == 0xC0) { // compress name
    *iter->iter++ = *src++;
    *iter->iter++ = *src++;
  } else {
    /* Add the ending null */
    *iter->iter++ = 0;
  }
}


static void dns_write_header( dns_message_iterator_t* iter, uint16_t id, uint16_t flags, uint16_t question_count, uint16_t answer_count, uint16_t authorative_count )
{
  memset( iter->header, 0, sizeof(dns_message_header_t) );
  iter->header->id				= htons(id);
  iter->header->flags 			= htons(flags);
  iter->header->question_count	= htons(question_count);
  iter->header->name_server_count = htons(authorative_count);
  iter->header->answer_count		= htons(answer_count);
}


static void dns_write_record( dns_message_iterator_t* iter, const char* name, uint16_t record_class, uint16_t record_type, uint32_t ttl, uint8_t* rdata )
{
  uint8_t* rd_length;
  uint8_t* temp_ptr;
  
  /* Write the name, type, class, TTL*/
  dns_write_name	( iter, name );
  dns_write_uint16( iter, record_type );
  dns_write_uint16( iter, record_class );
  dns_write_uint32( iter, ttl );
  
  /* Keep track of where we store the rdata length*/
  rd_length	= iter->iter;
  iter->iter += 2;
  temp_ptr	= iter->iter;
  
  switch ( record_type )
  {
  case RR_TYPE_A:
    dns_write_bytes( iter, rdata, 4 );
    break;
    
  case RR_TYPE_PTR:
  case RR_TYPE_TXT:
    dns_write_name( iter, (const char*) rdata );
    break;
    
  case RR_TYPE_SRV:
    /* Set priority and weight to 0*/
    dns_write_uint16( iter, 0 );
    dns_write_uint16( iter, 0 );
    
    /* Write the port*/
    dns_write_uint16( iter, ( (dns_sd_service_record_t*) rdata )->port );
    
    /* Write the hostname*/
    dns_write_string( iter, ( (dns_sd_service_record_t*) rdata )->hostname );
    break;
  default:
    break;
  }
  // Write the rdata length
  rd_length[0] = ( iter->iter - temp_ptr ) >> 8;
  rd_length[1] = ( iter->iter - temp_ptr ) & 0xFF;
}

static void mdns_send_message(int fd, dns_message_iterator_t* message )
{
  struct sockaddr_in addr;

//  printf("enter send message\r\n");
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_MULTICAST_MDNS);
  addr.sin_port = htons(5353);
  sendto(fd, message->header, message->iter - (uint8_t*)message->header, 0, (struct sockaddr *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  addr.sin_port = htons(INADDR_BROADCAST);
  sendto(fd, message->header, message->iter - (uint8_t*)message->header, 0, (struct sockaddr *)&addr, sizeof(addr));
}

static void dns_write_uint16( dns_message_iterator_t* iter, uint16_t data )
{
  // We cannot assume the u8 alignment of iter->iter so we can't just typecast and assign
  iter->iter[0] = data >> 8;
  iter->iter[1] = data & 0xFF;
  iter->iter += 2;
}

static void dns_write_uint32( dns_message_iterator_t* iter, uint32_t data )
{
  iter->iter[0] = data >> 24;
  iter->iter[1] = data >> 16;
  iter->iter[2] = data >> 8;
  iter->iter[3] = data & 0xFF;
  iter->iter += 4;
}

static void dns_write_bytes( dns_message_iterator_t* iter, uint8_t* data, uint16_t length )
{
  int a = 0;
  
  for ( a = 0; a < length; ++a )
  {
    iter->iter[a] = data[a];
  }
  iter->iter += length;
}

static uint16_t dns_read_uint16( dns_message_iterator_t* iter )
{
  uint16_t temp = (uint16_t) ( *iter->iter++ ) << 8;
  temp += (uint16_t) ( *iter->iter++ );
  return temp;
}

static void dns_skip_name( dns_message_iterator_t* iter )
{
  while ( *iter->iter != 0 )
  {
    // Check if the name is compressed
    if ( *iter->iter & 0xC0 )
    {
      iter->iter += 1; // Normally this should be 2, but we have a ++ outside the while loop
      break;
    }
    else
    {
      iter->iter += (uint32_t) *iter->iter + 1;
    }
    if (iter->iter > iter->end)
      break;
  }
  // Skip the null u8
  ++iter->iter;
}

static void dns_write_name( dns_message_iterator_t* iter, const char* src )
{
  dns_write_string( iter, src );
}

static bool is_service_match ( dns_sd_service_record_t *record, char *service_name, WiFi_Interface interface )
{
  if( record->state == RECORD_REMOVED || record->state == RECORD_REMOVE )
    return false;

  if( ( service_name == NULL || strcmp( record->service_name, service_name ) == 0 ) && record->interface == interface )
    return true;

  return false;
}

static int find_record_by_service ( char *service_name, WiFi_Interface interface )
{
  int i;
  uint32_t insert_index = 0xFF;

  for ( i = 0; i < available_service_count; i++ ){
    if( is_service_match ( &available_services[i], service_name, interface ) ){
      insert_index = i;
      break;
    }
  }  
  return insert_index;
}

static int find_empty_record ( void )
{
  int i;
  uint32_t insert_index = 0xFF;

  for ( i = 0; i < available_service_count; i++ ){
    if( available_services[i].state == RECORD_REMOVED ){
      insert_index = i;
      break;
    }
  }
  return insert_index;
}

static mico_thread_t _bonjour_announce_handler = NULL;

void _bonjour_send_anounce_thread(uint32_t arg)
{
  uint32_t insert_index = 0xFF;
  UNUSED_PARAMETER( arg );

  while(1){
    insert_index = 0xFF;
    for ( int i = 0; i < available_service_count; i++ ){
      if( available_services[i].state != RECORD_REMOVED && available_services[i].count_down != 0 ){
        insert_index = i;
        break;
      }
    }  

    if( insert_index == 0xFF )
      goto exit;
    
    mdns_utils_log( "sem trigger" );
    mico_rtos_set_semaphore( &update_state_sem );
    mico_thread_msleep(200);
  }
  
exit:
  _bonjour_announce_handler = NULL;
  mico_rtos_delete_thread( NULL );
  return;
}

static void _clean_record_resource( dns_sd_service_record_t *record )
{
  if(record->service_name)  {
    free(record->service_name);
    record->service_name = NULL;
  }
  if(record->hostname){
    free(record->hostname);
    record->hostname = NULL;
    }
  if(record->instance_name){
      free(record->instance_name);
      record->instance_name = NULL;
    }
  if(record->txt_att){
    free(record->txt_att);
    record->txt_att = NULL;
  }

}


OSStatus mdns_add_record( mdns_init_t init, WiFi_Interface interface, uint32_t time_to_live )
{
  int len;
  OSStatus err = kNoErr;
  uint32_t insert_index = 0xFF;

  if( bonjour_instance == false ){
    err = start_bonjour_service( );
    require_noerr(err, exit);
  }
  
  insert_index = find_record_by_service ( init.service_name, interface );
  if( insert_index == 0xFF)
    insert_index = find_empty_record ( );

  require_action( insert_index < available_service_count, exit, err = kNoResourcesErr);
  
  mico_rtos_lock_mutex( &bonjour_mutex );

  _clean_record_resource( &available_services[insert_index] );

  available_services[insert_index].interface = interface;
  available_services[insert_index].service_name = (char*)__strdup(init.service_name);
  available_services[insert_index].hostname = (char*)__strdup(init.host_name);

  len = strlen(init.instance_name);
  available_services[insert_index].instance_name = (char*)malloc(len+3);//// 0xc00c+\0
  memcpy(available_services[insert_index].instance_name, init.instance_name, len);
  available_services[insert_index].instance_name[len]= 0xc0;
  available_services[insert_index].instance_name[len+1]= 0x0c;
  available_services[insert_index].instance_name[len+2]= 0;
  
  available_services[insert_index].txt_att = (char*)__strdup(init.txt_record);

  available_services[insert_index].port = init.service_port;
  available_services[insert_index].state = RECORD_UPDATE;
  available_services[insert_index].count_down = 5;
  available_services[insert_index].ttl = time_to_live;
  
  if( _bonjour_announce_handler == NULL)
    mico_rtos_create_thread( &_bonjour_announce_handler, MICO_APPLICATION_PRIORITY, "Bonjour Announce", _bonjour_send_anounce_thread, 0x400, 0 );

  mico_rtos_unlock_mutex( &bonjour_mutex );

exit:
  return err;
}


void mdns_update_txt_record( char *service_name, WiFi_Interface interface, char *txt_record )
{
  uint32_t insert_index = 0xFF;

  if( bonjour_instance == false ) return;

  insert_index = find_record_by_service ( service_name, interface );
  if( insert_index == 0xFF ) return;

  mico_rtos_lock_mutex( &bonjour_mutex );

  if(available_services[insert_index].txt_att)  free(available_services[insert_index].txt_att);
  available_services[insert_index].txt_att = (char*)__strdup(txt_record);
  available_services[insert_index].state = RECORD_UPDATE;
  available_services[insert_index].count_down = 5;

  if( _bonjour_announce_handler == NULL)
    mico_rtos_create_thread( &_bonjour_announce_handler, MICO_APPLICATION_PRIORITY, "Bonjour Announce", _bonjour_send_anounce_thread, 0x400, 0 );

  mico_rtos_unlock_mutex( &bonjour_mutex );
}
  

void mdns_suspend_record( char *service_name, WiFi_Interface interface, bool will_remove )
{
  int i;
  uint32_t insert_index = 0xFF;
  
  mdns_utils_log( "Suspend %s@%d",  service_name == NULL ? "null" : service_name, interface);

  if( bonjour_instance == false ) return;

  mico_rtos_lock_mutex( &bonjour_mutex );

  for ( i = 0; i < available_service_count; i++ ){
    if( is_service_match ( &available_services[i], service_name, interface ) == false) 
      continue;
    
    mdns_utils_log( "Find index %d to suspend", i);
    if( will_remove == true )
      available_services[i].state = RECORD_REMOVE;
    else{
      if( available_services[i].state != RECORD_REMOVE )
        available_services[i].state = RECORD_SUSPEND;
    }
  
    available_services[i].count_down = 5; 
    insert_index = i;
  }

  if( insert_index == 0xFF )
    goto exit;

  if( _bonjour_announce_handler == NULL)
    mico_rtos_create_thread( &_bonjour_announce_handler, MICO_APPLICATION_PRIORITY, "Bonjour Announce", _bonjour_send_anounce_thread, 0x400, 0 );

exit:
  mico_rtos_unlock_mutex( &bonjour_mutex );
  return;
}

void mdns_resume_record( char *service_name, WiFi_Interface interface )
{
  int i;
  uint32_t insert_index = 0xFF;

  if( bonjour_instance == false ) return;

  mico_rtos_lock_mutex( &bonjour_mutex );

  for ( i = 0; i < available_service_count && is_service_match( &available_services[i], service_name, interface ); i++ ){
    available_services[i].state = RECORD_UPDATE;
    available_services[i].count_down = 5; 
    insert_index = i;
  }

  if( insert_index == 0xFF )
    goto exit;

  if( _bonjour_announce_handler == NULL)
    mico_rtos_create_thread( &_bonjour_announce_handler, MICO_APPLICATION_PRIORITY, "Bonjour Announce", _bonjour_send_anounce_thread, 0x400, 0 );

exit:
  mico_rtos_unlock_mutex( &bonjour_mutex );
  return;
}

mdns_record_state_t mdns_get_record_status( char *service_name, WiFi_Interface interface)
{
    uint32_t insert_index = find_record_by_service( service_name, interface );

    if( insert_index == 0xFF ) return RECORD_REMOVED;

    return available_services[insert_index].state;
}

void mdns_handler(int fd, uint8_t* pkt, int pkt_len)
{
  dns_message_iterator_t iter;
  
  iter.header = (dns_message_header_t*) pkt;
  iter.iter   = (uint8_t*) iter.header + sizeof(dns_message_header_t);
  iter.end = pkt+pkt_len;
  
  // Check if the message is a response (otherwise its a query)
  if ( ntohs(iter.header->flags) & DNS_MESSAGE_IS_A_RESPONSE )
  {
  }
  else
  {
    process_dns_questions(fd, &iter );
  }
}

void bonjour_send_record(int record_index)
{
  dns_message_iterator_t response;
  uint32_t myip;
  IPStatusTypedef para;
  int ttl  = 0;

  /* Send service and a ttl > 0 for a working record*/
  if( available_services[record_index].state == RECORD_NORMAL || available_services[record_index].state == RECORD_UPDATE ){
    ttl = available_services[record_index].ttl;
    
    if(dns_create_message( &response, 512 )) {
      dns_write_header(&response, 0x0, 0x8400, 0, 1, 0 );
      dns_write_record( &response, SERVICE_QUERY_NAME, RR_CLASS_IN, RR_TYPE_PTR, 1500, (uint8_t*) available_services[record_index].service_name );
      mdns_send_message(mDNS_fd, &response );
      dns_free_message( &response );
    }
  }
  micoWlanGetIPStatus(&para, available_services[record_index].interface);
  myip = inet_addr(para.ip);
  if( myip == 0 || myip == 0xFFFFFFFF) return;


  if(dns_create_message( &response, 512 )){
    mdns_utils_log( "TTL = %d",  ttl);
    micoWlanGetIPStatus(&para, available_services[record_index].interface);
    dns_write_header( &response, 0x0, 0x8400, 0, 4, 0 );
    dns_write_record( &response, available_services[record_index].service_name, RR_CLASS_IN, RR_TYPE_PTR, ttl, (uint8_t*) available_services[record_index].instance_name );
    dns_write_record( &response, available_services[record_index].instance_name, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_TXT, ttl, (uint8_t*) available_services[record_index].txt_att );
    dns_write_record( &response, available_services[record_index].instance_name, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_SRV, ttl, (uint8_t*) &available_services[record_index]);
    dns_write_record( &response, available_services[record_index].hostname, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_A, ttl, (uint8_t*) &myip);
    mdns_send_message(mDNS_fd, &response );
    dns_free_message( &response );
  }
}

void BonjourNotify_WifiStatusHandler( WiFiEvent event, void *arg )
{
  UNUSED_PARAMETER(arg);  
  switch (event) {
  case NOTIFY_STATION_UP:
    mdns_resume_record( NULL, Station );
    break;
  case NOTIFY_STATION_DOWN:
    mdns_suspend_record( NULL, Station, false );
    break;
  case NOTIFY_AP_UP:
    mdns_resume_record( NULL, Soft_AP );
    break;
  case NOTIFY_AP_DOWN:
    mdns_suspend_record( NULL, Soft_AP, false );
    break;
  default:
    break;
  }
  return;
}

void BonjourNotify_SYSWillPoerOffHandler( void *arg )
{
    UNUSED_PARAMETER(arg);  
    mdns_suspend_record( NULL, Station, true );
    mdns_suspend_record( NULL, Soft_AP, true );
}

uint8_t *buf = NULL;

static OSStatus start_bonjour_service(void)
{
  OSStatus err = kNoErr;
  struct sockaddr_in addr;
  ip_mreq mreq_opt;

  if(bonjour_mutex == NULL)
    mico_rtos_init_mutex( &bonjour_mutex );

  if(update_state_sem == NULL)
    mico_rtos_init_semaphore( &update_state_sem, 1 );

  update_state_fd = mico_create_event_fd( update_state_sem );

  memset( available_services, 0x0, sizeof( available_services ) );

  buf = malloc(1500);
  require_action(buf, exit, err =kNoMemoryErr);
  
  mDNS_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  require_action(IsValidSocket( mDNS_fd ), exit, err = kNoResourcesErr );
  
  mreq_opt.imr_multiaddr.s_addr = htonl(INADDR_MULTICAST_MDNS);
  mreq_opt.imr_interface.s_addr = htonl(INADDR_ANY);
  setsockopt(mDNS_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq_opt, sizeof(ip_mreq));
  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5353);
  addr.sin_addr.s_addr = INADDR_ANY;
  err = bind(mDNS_fd, (struct sockaddr *)&addr, sizeof(addr));
  require_noerr(err, exit);

  err = mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)BonjourNotify_WifiStatusHandler, NULL );
  require_noerr( err, exit );
  err = mico_system_notify_register( mico_notify_SYS_WILL_POWER_OFF, (void *)BonjourNotify_SYSWillPoerOffHandler, NULL );
  require_noerr( err, exit );

  err = mico_rtos_create_thread(&mfi_bonjour_thread_handler, MICO_APPLICATION_PRIORITY, "Bonjour", _bonjour_thread, 0x500, 0 );
  require_noerr(err, exit);

  bonjour_instance = true;

exit:
  return err;
}

void _bonjour_thread(uint32_t arg)
{
  int i, con = -1;
  struct timeval t;
  fd_set readfds;
  struct sockaddr_in addr;
  socklen_t addrLen;
  //OSStatus err = kNoErr;
  UNUSED_PARAMETER( arg );

  t.tv_sec = 1;
  t.tv_usec = 0;
  
  while(1) {
    /*Check status on erery sockets on bonjour query */
    FD_ZERO(&readfds);
    FD_SET(mDNS_fd, &readfds);
    FD_SET(update_state_fd, &readfds);
    select( Max( mDNS_fd, update_state_fd )+ 1, &readfds, NULL, NULL, &t );

    if ( FD_ISSET( update_state_fd, &readfds ) ){ 
      mdns_utils_log( "sem recved" );
      mico_rtos_get_semaphore( &update_state_sem, 0 );
      mico_rtos_lock_mutex( &bonjour_mutex );
      for ( i = 0; i < available_service_count; i++ ){
        switch ( available_services[i].state ){
          case RECORD_REMOVE: 
            mdns_utils_log( "Remove record %d", i );
            bonjour_send_record( i );
            available_services[i].count_down--;
            if( available_services[i].count_down == 0){
              _clean_record_resource( &available_services[i] );
              available_services[i].state = RECORD_REMOVED;
            }
            break;
          case RECORD_SUSPEND:
            if( available_services[i].count_down ){
              mdns_utils_log( "Suspend record %d", i );
              bonjour_send_record( i );
              if( available_services[i].count_down )
                available_services[i].count_down--;       
            }
            break;
          case RECORD_UPDATE:
            mdns_utils_log( "Update record %d, cd: %d", i, available_services[i].count_down );
            bonjour_send_record( i );
            available_services[i].count_down--;
            if( available_services[i].count_down == 0)
              available_services[i].state = RECORD_NORMAL;
            break;
          default:
            break;
        }
      }
      mico_rtos_unlock_mutex( &bonjour_mutex );
    }
    
    /*Read data from udp and send data back */ 
    if (FD_ISSET(mDNS_fd, &readfds)) {
      con = recvfrom(mDNS_fd, buf, 1500, 0, (struct sockaddr *)&addr, &addrLen);
      mico_rtos_lock_mutex( &bonjour_mutex );
      mdns_handler(mDNS_fd, (uint8_t *)buf, con);
      mico_rtos_unlock_mutex( &bonjour_mutex );
    }
  }
  
  //mdns_utils_log("Exit: mDNS thread exit with err = %d", err);
  //SocketClose( &mDNS_fd );
  //if(buf) free(buf);
  //mico_rtos_delete_thread(NULL);
}



