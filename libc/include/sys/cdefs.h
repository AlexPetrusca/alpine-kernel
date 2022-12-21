#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H 1

#define __myos_libc 1

#define __unused    __attribute__((__unused__))

#if defined(__cplusplus)
#define __BEGIN_DECLS   extern "C" {
#define __END_DECLS     }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#endif
