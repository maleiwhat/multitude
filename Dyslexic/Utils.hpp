/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Dyslexic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DYSLEXIC_UTILS_HPP
#define DYSLEXIC_UTILS_HPP

#include <Dyslexic/CPUBitmapFont.hpp>
#include <Dyslexic/Export.hpp>

#include <Radiant/StringUtils.hpp>

namespace Dyslexic
{

  /** Utility functions for the Dyslexic font rendering engine. */

  namespace Utils
  {

    /// Zero-width space character is used as new line character.
    enum { W_NEWLINE = 0x200B };

    /**
      * @brief Break wstring into lines.
      * The lines will be less than or equal to the specified width when displayed
      * in the given font.
      * @param ws The string to be broken.
      * @param width Maximum width of line.
      * @param bitmapFont Font used for rendering.
      * @param lines Reference to list to receive the lines.
      * @note Newline characters are retained in the output.
      * @param afterSpace true to break lines after inter-word spaces.
      */
    void DYSLEXIC_API breakToLines(const std::wstring & ws, const float width,
      CPUFont & bitmapFont, Radiant::StringUtils::WStringList & lines,
      const bool afterSpace = true);

    /**
      * @brief Tokenize wstring.
      * @param ws The string to be tokenized.
      * @param delim One or more delimiter characters.
      * @param out Reference to list to receive the tokens.
      * @param afterDelim true to split string after delimiter.
      */
    void DYSLEXIC_API split(const std::wstring & ws, const std::wstring & delim,
      Radiant::StringUtils::WStringList & out, const bool afterDelim = true);

  }

}

#endif
