/* basetype.h - Stub header for Nox base types
 * NoxCompress.cpp uses common game types that are defined
 * elsewhere in the codebase. This provides the minimal set needed.
 */

#ifndef BASETYPE_H
#define BASETYPE_H

/* If building within the Generals engine, these types come from GameCommon.h.
 * This stub provides fallback definitions. */

#ifndef Bool
typedef bool Bool;
#endif

#ifndef Int
typedef int Int;
#endif

#ifndef UnsignedInt
typedef unsigned int UnsignedInt;
#endif

#ifndef UnsignedByte
typedef unsigned char UnsignedByte;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* BASETYPE_H */
