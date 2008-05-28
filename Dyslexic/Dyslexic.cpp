/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Dyslexic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#include "Dyslexic.hpp"
#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Dyslexic
{

  static FT_Library * g_library = 0;
  static FT_Error g_error = 0;

  bool initialize()
  {
    if(g_library) 
      return true;

    g_library = new FT_Library;

    g_error = FT_Init_FreeType(g_library);
    if(g_error) {
      delete g_library;
      g_library = 0;
      return false;
    }

    return true;    
  }

  FT_Error error() 
  {
    return g_error;
  }

  FT_Library * freetype()
  {
    if(!g_library) {
      bool result = initialize();
      assert(result);
    }

    return g_library;
  }

}
