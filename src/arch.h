//
// arch.h
//
// This is only necessary for the introspection as of now
//

#ifndef ARCH_H

#ifndef __amd64__
	#if defined(_M_AMD64)
		#define __amd64__
	#endif
#endif

#ifndef __i386__
	#if defined(__i386) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__INTEL__) || defined(__THW_INTEL__) || defined(__I86__)
		#define __i386__
	#endif
#endif

#if defined(__amd64__) && defined(__i386__)
	#error Both detected both a x86 and amd64 style architecture.
#endif

#endif
