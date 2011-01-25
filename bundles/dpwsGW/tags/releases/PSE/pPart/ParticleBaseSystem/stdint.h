#ifndef __STDINT_H__
#define __STDINT_H__

typedef unsigned int8 uint8_t;
typedef signed int8 int8_t;

typedef unsigned int16 uint16_t;
typedef signed int16 int16_t;

typedef unsigned int32 uint32_t;
typedef signed int32 int32_t;
 
typedef int1 int1_t;

#ifndef ssize_t 
#define ssize_t int8_t
#endif

#endif
