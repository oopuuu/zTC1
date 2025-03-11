/*****************************************************
  Usage:
   1. function: (a or b)
      a. ATTR_TEXT_IN_TCM int func(int par);
      b. ATTR_TEXT_IN_TCM int func(int par)
        {
        }
   2. RO data:
      a. ATTR_RODATA_IN_TCM const int b = 8;
   3. RW data:
      a. ATTR_RWDATA_IN_TCM int b = 8;
   4. ZI data:
      a. ATTR_ZIDATA_IN_TCM int b;
      
  Note: must put these attributes at HEAD of declaration.
*****************************************************/

#ifndef MEMORY_ATTRIBUTE_H_
#define MEMORY_ATTRIBUTE_H_

#ifndef __ICCARM__

#define ATTR_TEXT_IN_TCM              __attribute__ ((__section__(".ramTEXT")))
#define ATTR_RODATA_IN_TCM
#define ATTR_RWDATA_IN_TCM
#define ATTR_ZIDATA_IN_TCM            __attribute__ ((__section__(".tcmBSS")))


#define ATTR_TEXT_IN_RAM
#define ATTR_RWDATA_IN_NONCACHED_RAM
#define ATTR_ZIDATA_IN_NONCACHED_RAM

#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__aligned__(4)))
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__aligned__(4)))

#define ATTR_PACKED __attribute__ ((__packed__))

#define ATTR_4BYTE_ALIGN __attribute__ ((__aligned__(4)))
#else
#define ATTR_TEXT_IN_TCM              _Pragma("location=\".ramTEXT\"")
#define ATTR_RODATA_IN_TCM
#define ATTR_RWDATA_IN_TCM
#define ATTR_ZIDATA_IN_TCM            _Pragma("location=\".tcmBSS\"")
#define ATTR_TEXT_IN_RAM
#define ATTR_RWDATA_IN_NONCACHED_RAM
#define ATTR_ZIDATA_IN_NONCACHED_RAM
#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  _Pragma("location=\".noncached_rwdata\"") \
                                                  _Pragma("data_alignment=4")
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  _Pragma("location=\".noncached_zidata\"") \
                                                  _Pragma("data_alignment=4")
#define ATTR_PACKED __packed
#define ATTR_4BYTE_ALIGN _Pragma("data_alignment=4")
#endif
#endif

