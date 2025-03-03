#include "../../../ver0_11/src/ff.h"

#if _USE_LFN != 0

#if   _CODE_PAGE == 932	/* Japanese Shift_JIS */
#include "../../../ver0_11/src/option/cc932.c"
#elif _CODE_PAGE == 936	/* Simplified Chinese GBK */
#include "../../../ver0_11/src/option/cc936.c"
#elif _CODE_PAGE == 949	/* Korean */
#include "../../../ver0_11/src/option/cc949.c"
#elif _CODE_PAGE == 950	/* Traditional Chinese Big5 */
#include "../../../ver0_11/src/option/cc950.c"
#else					/* Single Byte Character-Set */
#include "../../../ver0_11/src/option/ccsbcs.c"
#endif

#endif
