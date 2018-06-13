/*
 * Generated by dtrace(1M).
 */

#ifndef	_PROBES_H
#define	_PROBES_H

#include <unistd.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define NEWTON_STABILITY "___dtrace_stability$newton$v1$1_1_0_1_1_0_1_1_0_1_1_0_1_1_0"

#define NEWTON_TYPEDEFS "___dtrace_typedefs$newton$v2"

#if !defined(DTRACE_PROBES_DISABLED) || !DTRACE_PROBES_DISABLED

#define	NEWTON_NEWTON_DONE() \
do { \
	__asm__ volatile(".reference " NEWTON_TYPEDEFS); \
	__dtrace_probe$newton$newton__done$v1(); \
	__asm__ volatile(".reference " NEWTON_STABILITY); \
} while (0)
#define	NEWTON_NEWTON_DONE_ENABLED() \
	({ int _r = __dtrace_isenabled$newton$newton__done$v1(); \
		__asm__ volatile(""); \
		_r; })
#define	NEWTON_NEWTON_START() \
do { \
	__asm__ volatile(".reference " NEWTON_TYPEDEFS); \
	__dtrace_probe$newton$newton__start$v1(); \
	__asm__ volatile(".reference " NEWTON_STABILITY); \
} while (0)
#define	NEWTON_NEWTON_START_ENABLED() \
	({ int _r = __dtrace_isenabled$newton$newton__start$v1(); \
		__asm__ volatile(""); \
		_r; })


extern void __dtrace_probe$newton$newton__done$v1(void);
extern int __dtrace_isenabled$newton$newton__done$v1(void);
extern void __dtrace_probe$newton$newton__start$v1(void);
extern int __dtrace_isenabled$newton$newton__start$v1(void);

#else

#define	NEWTON_NEWTON_DONE() \
do { \
	} while (0)
#define	NEWTON_NEWTON_DONE_ENABLED() (0)
#define	NEWTON_NEWTON_START() \
do { \
	} while (0)
#define	NEWTON_NEWTON_START_ENABLED() (0)

#endif /* !defined(DTRACE_PROBES_DISABLED) || !DTRACE_PROBES_DISABLED */


#ifdef	__cplusplus
}
#endif

#endif	/* _PROBES_H */
