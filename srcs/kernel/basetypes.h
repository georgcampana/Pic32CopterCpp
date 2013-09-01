/* 
 * File:   basetypes.h
 * Author: Georg Campana
 *
 * Created on 1 settembre 2013, 22.40
 */

#ifndef BASETYPES_H
#define	BASETYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef char         Char;

typedef signed char         Int8;
typedef signed short int    Int16;
typedef signed long int     Int32;
typedef signed long long    Int64;

typedef unsigned char       UInt8;
typedef unsigned short int  UInt16;
typedef unsigned long int   UInt32;
typedef unsigned long long  UInt64;

#ifndef NULL
#  define NULL (0)
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* BASETYPES_H */

