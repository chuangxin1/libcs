/**
 * csacred.com crt
 * Copyright (c) 2007-2018 ls
 **/
#ifndef _CS_CRT_H_INCLUDED_
#define _CS_CRT_H_INCLUDED_

#define cs_version 062
#define CS_VERSION "0.6.2"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>	
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/timeb.h>
#include <errno.h>

#define CS_OK          0
#define CS_ERR        -1
#define CS_AGAIN      -2
#define CS_BUSY       -3
#define CS_DONE       -4
#define CS_DECLINED   -5
#define CS_ABORT      -6

/* Platform */
#ifdef _WIN32 /* Windows */
#include <winsock2.h>
//#include <mswsock.h>
#include <wininet.h>
#include <winreg.h>
	
#include <process.h>
#include <tchar.h>
#include <io.h>
#include <ws2tcpip.h>

#ifndef LIBCS_STATIC
#ifdef  LIBCS_EXPORTS
#define CS_API _declspec(dllexport)
#else
#define CS_API _declspec(dllimport)
#endif
#else
#define CS_API
#endif
#define cs_inline     __inline

#define cs_int64_t    __int64
#define cs_uint64_t   unsigned __int64
#define cs_sock_t     SOCKET

#define cs_s64        "%I64d"
#define cs_us64       "%I64u"

#if _MSC_VER > 1200 /* VC6+ */
typedef intptr_t      cs_int_t;
typedef uintptr_t     cs_uint_t;
#else /* VC6 */
#define _WIN32_WINNT  0x0500
typedef int           cs_int_t;
typedef unsigned int  cs_uint_t;
#endif

#define cs_errno      GetLastError()
#define cs_sock_errno GetLastError() // WSAGetLastError()
#else /* Linux/BSD */
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>  /* TCP_NODELAY, TCP_CORK */
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <poll.h>
#include <sys/malloc.h>
#include <sys/time.h>

#define CS_API
#define cs_inline     inline

typedef intptr_t      cs_int_t;
typedef uintptr_t     cs_uint_t;
#define cs_int64_t    long long
#define cs_uint64_t   unsigned long long
#define cs_sock_t     int

#define cs_errno      errno
#define cs_sock_errno errno

#define cs_s64        "%lld"
#define cs_us64       "%llu"
#endif

#ifndef u_char
#define u_char unsigned char
#endif

#define CS_ALIGNMENT              sizeof(unsigned long) /* platform word */

#define cs_align(d, a)            (((d) + (a - 1)) & ~(a - 1))
#define cs_align_ptr(p, a)        (u_char *) (((uintptr_t) (p) +  \
  ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#define cs_offsetof(type, member) ((size_t)&((type *)0)->member)
#ifdef _WIN32
#define cs_container_of(ptr, type, member) (type *)((char *)ptr - \
  cs_offsetof(type, member))
#else
#define cs_container_of(ptr, type, member) ({                     \
  const typeof(((type *)0)->member) *__mptr = (ptr);              \
  (type *)((char *)__mptr - cs_offsetof(type, member));})
#endif

CS_API const u_char *cs_libversion();

#ifdef __cplusplus
}
#endif /* C++ support */
#endif