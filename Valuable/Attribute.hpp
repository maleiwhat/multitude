/* COPYRIGHT
 *
 * This file is part of Valuable.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Valuable.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUABLE_ATTRIBUTE_HPP
#define VALUABLE_ATTRIBUTE_HPP

#include <Valuable/Export.hpp>
#include <string>

namespace Valuable
{  
  using namespace std;

  class VALUABLE_API Attribute
  {
  public:
    Attribute(void);
    Attribute(string k,string v);
    virtual ~Attribute(void);
    int depth;
    string key,value;
  };
}


#endif
