#include <wmstdio.h>
#include <wm_os.h>
#include <wmlog.h>

void wmpanic()
{
	wmlog("**** Panic from function: %p ****", "\n\r",
		 __builtin_return_address(0));

	/* Ensure that nothing runs after this */
	os_enter_critical_section();
	while (1)
		;
}
