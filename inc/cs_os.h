/**
 * cs_os.h
 * Copyright (c) 2007-2018 ls
 **/
#ifndef _CS_OS_H_INCLUDED_
#define _CS_OS_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

/*
* cs_os_cpu_count
*	Return the number of CPUs.
*/
CS_API cs_uint_t cs_os_cpu_count();
CS_API cs_int_t  cs_os_pagesize();

#ifdef _WIN32
/*
* cs_os_is_winnt
* Return 1 if Windows/NT, otherwise 0.
*/
CS_API cs_int_t  cs_os_is_winnt();
#endif

#ifdef __cplusplus
}
#endif /* C++ support */
#endif