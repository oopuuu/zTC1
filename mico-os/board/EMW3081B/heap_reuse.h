#ifndef __HEAP_REUSE_H__
#define __HEAP_REUSE_H__
/* yhb defined, ref micoLinkerForIAR.icf 
  * some memory region can be used as HEAP when there will not used any more.
  */


/* call it After mico MFG test and force OTA */
void bootup_region_reuse(void);

/* call it in configure mode; or after ota check in work mode */
void tota_region_reuse(void);

/* call it in work mode; or configure mode with alink mode */
void elink_region_reuse(void);

/* call it in work mode; or configure mode with easylink mode */
void alink_region_reuse(void);

#endif
