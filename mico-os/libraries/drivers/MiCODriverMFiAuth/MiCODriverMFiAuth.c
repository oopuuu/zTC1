/**
 ******************************************************************************
 * @file    PlatformMFiAuth.c
 * @author  William Xu
 * @version V1.0.0
 * @date    08-June-2014
 * @brief   This header contains function called by Apple WAC code that must be
 *          implemented by the platform. These functions are called when Apple
 *          code needs to interact with the Apple Authentication Coprocessor.
 *          Please refer to the relevant version of the "Auth IC" document to
 *          obtain more details on how to interact with the Authentication
 *          Coprocessor. This document can be found on the MFi Portal.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "MiCODrivers/MiCODriverI2c.h"

#define CP_ADDRESS_1 0x20
#define CP_ADDRESS_2 0x22

static mico_i2c_device_t MFi_CP =
{
    .port = MICO_I2C_CP,
    .address = CP_ADDRESS_1>>1,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode = I2C_HIGH_SPEED_MODE,
};

#define AH_DEVICE_VERSION                         0x00  // Length 1   R
#define AH_FIRMWARE_VERSON                        0x01  // Length 1   R
#define AH_AUTHENTICATION_PROTOCOL_MAJOR_VERSION  0x02  // Length 1   R
#define AH_AUTHENTICATION_PROTOCOL_MINOR_VERSION  0x03  // Length 1   R 
#define AH_DEVICE_ID                              0x04  // Length 4   R
#define AH_ERROR_CODE                             0x05  // Length 1   R

#define AH_AUTHENTICATION_CONTROL_STATUS          0x10  // Length 1
#define AH_CHALLENGE_RESPONSE_DATA_LENGTH         0x11  // Length 2   R/W
#define AH_CHALLENGE_RESPONSE_DATA                0x12  // Lenght 128 R/W
#define AH_CHALLENGE_DATA_LENGTH                  0x20  // Length 2   R/W
#define AH_CHALLENGE_DATA                         0x21  // Lenght 128 R/W

#define AH_ACCESSORY_CERTIFICATE_DATA_LENGTH      0x30  // Length 2   R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE1       0x31  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE2       0x32  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE3       0x33  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE4       0x34  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE5       0x35  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE6       0x36  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE7       0x37  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE8       0x38  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE9       0x39  // Length 128 R
#define AH_ACCESSORY_CERTIFICATE_DATA_PAGE10      0x3a  // Length 128 R

#define AH_SELF_TEST_CONTROL_STATUS               0x40  // Length 1   R/W
#define AH_SYSTEM_EVENT_COUNTER                   0x4D  // Length 1   R

#define  AH_IPOD_CERTIFICATE_DATA_LENGTH          0x50  // Length 2   R
#define  AH_IPOD_CERTIFICATE_DATA_PAGE1           0x51  // Length 128 R/W
#define  AH_IPOD_CERTIFICATE_DATA_PAGE2           0x52  // Length 128 R/W
#define  AH_IPOD_CERTIFICATE_DATA_PAGE3           0x53  // Length 128 R/W
#define  AH_IPOD_CERTIFICATE_DATA_PAGE4           0x54  // Length 128 R/W
#define  AH_IPOD_CERTIFICATE_DATA_PAGE5           0x55  // Length 128 R/W
#define  AH_IPOD_CERTIFICATE_DATA_PAGE6           0x56  // Length 128 R/W
#define  AH_IPOD_CERTIFICATE_DATA_PAGE7           0x57  // Length 128 R/W
#define  AH_IPOD_CERTIFICATE_DATA_PAGE8           0x58  // Length 128 R/W

static OSStatus CP_ReadBuffer(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
static OSStatus CP_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);

OSStatus MicoMFiAuthInitialize( int i2c )
{
    OSStatus err = kNoErr;
    // PLATFORM_TO_DO
    bool isCPFound = false;
    MFi_CP.port = i2c;
    
    err = MicoI2cInitialize( (mico_i2c_device_t *)&MFi_CP );
    require_noerr_action(err, exit, err = kRequestErr);

    isCPFound = MicoI2cProbeDevice( (mico_i2c_device_t *)&MFi_CP, 3 );
    if(!isCPFound){
      MFi_CP.address = CP_ADDRESS_2>>1;
      isCPFound = MicoI2cProbeDevice( (mico_i2c_device_t *)&MFi_CP, 3 );
    }
    require_action( isCPFound == true, exit, err = kNotFoundErr );

exit:
    return err;
}

void MicoMFiAuthFinalize( void )
{
    // PLATFORM_TO_DO
    return;
}

OSStatus MicoMFiAuthCreateSignature( const void *inDigestPtr,
                                         size_t     inDigestLen,
                                         uint8_t    **outSignaturePtr,
                                         size_t     *outSignatureLen )
{
    OSStatus err = kNoErr;
    uint8_t deviceVersion;

    // PLATFORM_TO_DO
    uint16_t Len_big = ((inDigestLen&0xFF00)>>8) + ((inDigestLen&0xFF)<<8);
    uint8_t control_status;

    err =  CP_BufferWrite((uint8_t *)(&Len_big), AH_CHALLENGE_DATA_LENGTH, 2);
    require( err == kNoErr, exit ) ;

    err = CP_ReadBuffer(&deviceVersion, AH_ERROR_CODE, 1);
    require( err == kNoErr, exit ) ;
    //wac_log("Write Digest data lengthstatus: %d", deviceVersion);

///////////////////////////////////////////////////////////////////////////////////////

    err =  CP_BufferWrite((uint8_t *)inDigestPtr, AH_CHALLENGE_DATA, inDigestLen);
    require( err == kNoErr, exit ) ;

    err = CP_ReadBuffer(&deviceVersion, AH_ERROR_CODE, 1);
    require( err == kNoErr, exit ) ;
    //wac_log("Write Digest data status: %d", deviceVersion);

///////////////////////////////////////////////////////////////////////////////////////

    control_status = 1;
    err =  CP_BufferWrite(&control_status, AH_AUTHENTICATION_CONTROL_STATUS, 1);
    require( err == kNoErr, exit ) ;

    deviceVersion = 0;
    err = CP_ReadBuffer(&deviceVersion, AH_ERROR_CODE, 1);
    require( err == kNoErr, exit ) ;
    //wac_log("Write control data status: %d", deviceVersion);
///////////////////////////////////////////////////////////////////////////////////////

    do{
      control_status = 0;
      err = CP_ReadBuffer(&control_status, AH_AUTHENTICATION_CONTROL_STATUS, 1);
      require( err == kNoErr, exit ) ;
      //wac_log("Read control: %d", control_status);
    }while((control_status>>4)!=0x1);

///////////////////////////////////////////////////////////////////////////////////////

    err = CP_ReadBuffer((uint8_t *)&Len_big, AH_CHALLENGE_RESPONSE_DATA_LENGTH, 2);
    require( err == kNoErr, exit ) ;

    *outSignatureLen = (size_t)(((Len_big&0xFF00)>>8) + ((Len_big&0xFF)<<8));

    //wac_log("Response length: %d", *outSignatureLen);

    *outSignaturePtr = malloc(*outSignatureLen);

    err = CP_ReadBuffer(*outSignaturePtr, AH_CHALLENGE_RESPONSE_DATA, *outSignatureLen);
    require( err == kNoErr, exit ) ;

    return err;

exit:
    return kNotPreparedErr;
}

OSStatus MicoMFiAuthCopyCertificate( uint8_t **outCertificatePtr, size_t *outCertificateLen )
{
    // PLATFORM_TO_DO
    OSStatus err = kNoErr;
    uint8_t page, pageNumber;
    uint16_t Len_big;
    uint8_t *obj;

    err = CP_ReadBuffer((uint8_t *)&Len_big, AH_ACCESSORY_CERTIFICATE_DATA_LENGTH, 2);  
    require( err == kNoErr, exit ) ;

    *outCertificateLen = (size_t)(((Len_big&0xFF00)>>8) + ((Len_big&0xFF)<<8));

     *outCertificatePtr = malloc(*outCertificateLen);
     obj = *outCertificatePtr;

    pageNumber = *outCertificateLen/128;

    for(page = AH_ACCESSORY_CERTIFICATE_DATA_PAGE1; page < AH_ACCESSORY_CERTIFICATE_DATA_PAGE1 + pageNumber; page++){
      err = CP_ReadBuffer(obj, page, 128);
      require( err == kNoErr, exit ) ;
      obj+=128;
      //wac_log("Read page number: %x", page);
    }

    pageNumber = (*outCertificateLen)%128;
    err = CP_ReadBuffer(obj, page, pageNumber);
    //wac_log("Read page number: %x,bytes: %d", page, pageNumber);
    
    return err;

exit: 
    return kNotPreparedErr;
}

static OSStatus CP_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  OSStatus err = kNoErr;
  mico_i2c_message_t message;
  uint8_t *txData = NULL;
  txData = malloc(NumByteToWrite+1);
  memcpy(txData, &WriteAddr, 1);
  memcpy(txData+1, pBuffer, NumByteToWrite);
  
  err = MicoI2cBuildTxMessage(&message, txData, NumByteToWrite+1, 100);
  require_noerr( err , exit ) ;
  err = MicoI2cTransfer( (mico_i2c_device_t *)&MFi_CP, &message, 1 );
  require_noerr( err , exit ) ;
 
exit:
  if(txData) free(txData);
  return err;
}

OSStatus CP_ReadBuffer(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  OSStatus err = kNoErr;
  mico_i2c_message_t message;

  err = MicoI2cBuildCombinedMessage(&message, &ReadAddr, pBuffer, 1, NumByteToRead, 100);
  require_noerr( err , exit ) ;
  err = MicoI2cTransfer( (mico_i2c_device_t *)&MFi_CP, &message, 1 );
  require_noerr( err , exit ) ;
 
exit:
  return err;
}



