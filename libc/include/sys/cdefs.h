#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H 1

#define __myos_libc 1

#define __unused      __attribute__((__unused__))
#define isr           __attribute__((interrupt))
#define static_assert _Static_assert

#if defined(__cplusplus)
#define __BEGIN_DECLS   extern "C" {
#define __END_DECLS     }
#else
#define __BEGIN_DECLS
#define __END_DECLS

#endif

// Allows getting the size of a type at compile time.
//
// Example of code:
//    char (*__kaboom)[sizeof( long long )] = 1;
// GCC error:
//    error: invalid conversion from 'int' to 'char (*)[8]' [-fpermissive]
// So sizeof(long long) == 8

#define  DEBUG_SIZEOF(type)  uint8_t (*__kaboom)[sizeof( type )] = 1;

#endif
