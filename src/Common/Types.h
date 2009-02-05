/*
 * Common/Types.h - Definice datovych typu.
 *
 * Copyright (c) 2006 Jiri Zajpt, <jzajpt@blueberry.cz>
*/

#ifndef _COMMON_TYPES__H
#define _COMMON_TYPES__H


typedef unsigned int u_int;
typedef unsigned int uint;


#ifdef WIN32

typedef unsigned char  u_int8_t;
typedef unsigned char  uint8_t;
typedef signed char    int8_t;

typedef unsigned short u_int16_t;
typedef unsigned short uint16_t;
typedef signed short   int16_t;

typedef unsigned int   u_int32_t;
typedef unsigned int   uint32_t;
typedef signed int     int32_t;

#endif // WIN32


#endif // _COMMON_TYPES__H
