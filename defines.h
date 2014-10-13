/*
 * defines.h
 *
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define INCLUDE_SERIALIZATION	1
#define INDEX_LONG

#ifdef INDEX_LONG
typedef long IndexType;
#else
typedef int IndexType;
#endif

#endif /* DEFINES_H_ */
