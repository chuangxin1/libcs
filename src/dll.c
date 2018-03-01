#include "../inc/cs.h"

#ifdef _WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {	
	return(TRUE);
}
#endif

CS_API const u_char *cs_libversion() {
	return ((const u_char *)CS_VERSION);
}