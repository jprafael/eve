#ifndef EVE_RT_PLATFORM_H
#define EVE_RT_PLATFORM_H

#if MSVC
	#ifdef _M_X86
		#define ARCH_X86
	#elif _M_X64
		#define ARCH_X86_64
	#else
		#error unknown ARCH
	#endif
#endif

#if __GNUC__
	#ifdef __i386__
		#define ARCH_X86
	#elif __x86_64__
		#define ARCH_X86_64
	#else
		#error unknown ARCH
	#endif
#endif

/* from libfiber */
#ifndef CACHE_SIZE
	#if defined(ARCH_X86) || defined(ARCH_X86_64)
	    #define CACHE_SIZE (64)
	#else
	    #error please define CACHE_SIZE for your architecture
	#endif
#endif

#define unlikely(x) (__builtin_expect((x),0))
#define likely(x) (__builtin_expect((x),1))


#ifdef __GNUC__
	#define THREAD_LOCAL __thread
#elif MSVC
	#define THREAD_LOCAL __declspec( thread )
#else
	/* assume c++11 standart */
	#define THREAD_LOCAL thread_local
#endif

/* OS dependent */
unsigned int get_cpu_count();

#if defined(__linux__) || (defined(TARGET_OS_MAC) && defined(_SC_NPROCESSORS_ONLN))
#include <unistd.h>
unsigned int get_cpu_count()
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
unsigned int get_cpu_count()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}
#else
	#error please define get_cpu_count() for your OS
#endif


#endif