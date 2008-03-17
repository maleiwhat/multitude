/* COPYRIGHT
 *
 * This file is part of Diva.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Diva.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DIVA_TYPES_HPP
#define DIVA_TYPES_HPP

#ifndef WIN32

#include <sys/types.h>

typedef long long llong;
typedef unsigned long long ullong;

typedef unsigned int uint;
typedef unsigned long ulong;

#else

//#include <windows.h>

#include <unixstuff.h>
typedef signed __int64 llong;
typedef unsigned __int64 ullong;
typedef unsigned int uint;
typedef unsigned long ulong;

#endif	//WIN32

typedef unsigned char uchar;

#endif


