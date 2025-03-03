#ifndef __WMLOG_H__
#define __WMLOG_H__

#ifdef CONFIG_ENABLE_ERROR_LOGS
#define wmlog_e(_mod_name_, _fmt_, ...)                                        \
	wmprintf("[%s]%s" _fmt_ "\n\r", _mod_name_, " Error: ", ##__VA_ARGS__)
#else
#define wmlog_e(...)
#endif /* CONFIG_ENABLE_ERROR_LOGS */

#ifdef CONFIG_ENABLE_WARNING_LOGS
#define wmlog_w(_mod_name_, _fmt_, ...)                                        \
	wmprintf("[%s]%s" _fmt_ "\n\r", _mod_name_, " Warn: ", ##__VA_ARGS__)
#else
#define wmlog_w(...)
#endif /* CONFIG_ENABLE_WARNING_LOGS */

/* General debug function. User can map his own debug functions to this
ne */
#ifdef CONFIG_DEBUG_BUILD
#define wmlog(_mod_name_, _fmt_, ...)                          \
	wmprintf("[%s] " _fmt_ "\n\r", _mod_name_, ##__VA_ARGS__)
#else
#define wmlog(...)
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DEBUG_BUILD
/* Function entry */
#define wmlog_entry(_fmt_, ...)                                        \
	wmprintf("> %s (" _fmt_ ")\n\r", __func__, ##__VA_ARGS__)

/* function exit */
#define wmlog_exit(_fmt_, ...)                                        \
	wmprintf("< %s" _fmt_ "\n\r", __func__, ##__VA_ARGS__)
#else
#define wmlog_entry(...)
#define wmlog_exit(...)
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_LL_DEBUG
#define ll_log(_fmt_, ...)						\
	ll_printf(_fmt_, ##__VA_ARGS__)
#else
#define ll_log(...)
#endif /* CONFIG_LL_DEBUG */
#endif /* __WMLOG_H__ */
