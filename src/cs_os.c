/**
 * cs_str.c
 * Copyright (c) 2007-2018 ls
 **/
#include "../inc/cs.h"
#include "../inc/cs_os.h"

CS_API cs_int_t cs_os_pagesize() {
	static cs_int_t __os_pagesize = -1;
	
	if (__os_pagesize == -1) {
#ifdef _WIN32
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		__os_pagesize = (cs_int_t)si.dwPageSize;
#else
		__os_pagesize = getpagesize();
#endif
	}

	return (__os_pagesize);
}

CS_API cs_uint_t cs_os_cpu_count() {
	static cs_uint_t __os_cpu_num = 0;

	if (__os_cpu_num == 0) {
#ifdef _WIN32
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		__os_cpu_num = (cs_uint_t)si.dwNumberOfProcessors;
#endif
	}

	return(__os_cpu_num);
}

#ifdef _WIN32
CS_API cs_int_t cs_os_is_winnt() {
	static cs_int_t __os_type = -1;
	/*
	* The value of __os_type is computed only once, and cached to
	* avoid the overhead of repeated calls to GetVersion().
	*/
	if (__os_type == -1) {
		if ((GetVersion() & 0x80000000) == 0) {
			__os_type = 1;
		} else {
			__os_type = 0;
		}
	}

	return (__os_type);
}
#endif