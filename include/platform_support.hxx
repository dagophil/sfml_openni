#ifndef PLATFORM_SUPPORT_HXX
#define PLATFORM_SUPPORT_HXX

/**
 * This header file defines old compiler macros for compatibility reasons.
 * Include this file before including any of the OpenNI header files.
 */ 
#ifdef __linux__
  #ifndef linux
    #define linux __linux__
  #endif
#endif
#ifdef __i386__
  #ifndef i386
    #define i386 __i386__
  #endif
#endif

#endif
