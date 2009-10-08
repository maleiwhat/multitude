/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "BinaryData.hpp"

#include <Radiant/BinaryStream.hpp>

#include <Nimble/Math.hpp>

#include <Radiant/Trace.hpp>

#include <string.h>
#include <stdlib.h>

#ifdef WIN32
# include <WinPort.h>
#endif
 
namespace Radiant {

  static bool __verbose = true;

  static void badmarker(const char * func, int32_t marker)
  {
    if(!__verbose)
      return;
    char a = marker & 0xFF;
    char b = (marker >> 8) & 0xFF;
    char c = (marker >> 16) & 0xFF;
    char d = (marker >> 24) & 0xFF;
    Radiant::error("%s # bad marker %c%c%c%c", func, a, b, c, d);
  }

  inline int32_t makeMarker(int32_t a, int32_t b, int32_t c, int32_t d)
  {
    return a | (b << 8) | (c << 16) | (d << 24);
  }

  const int FLOAT_MARKER  = makeMarker(',', 'f', '\0', '\0');
  const int VECTOR2F_MARKER  = makeMarker(',', 'f', '2', '\0');
  const int VECTOR2I_MARKER  = makeMarker(',', 'i', '2', '\0');
  const int VECTOR4F_MARKER  = makeMarker(',', 'f', '4', '\0');
  const int VECTOR4I_MARKER  = makeMarker(',', 'i', '4', '\0');
  const int INT32_MARKER  = makeMarker(',', 'i', '\0', '\0');
  const int INT64_MARKER  = makeMarker(',', 'l', '\0', '\0');
  const int TS_MARKER     = makeMarker(',', 't', '\0', '\0');
  const int STRING_MARKER = makeMarker(',', 's', '\0', '\0');
  const int WSTRING_MARKER = makeMarker(',', 'S', '\0', '\0');
  const int BLOB_MARKER   = makeMarker(',', 'b', '\0', '\0');

  BinaryData::BinaryData()
    : m_current(0),
      m_total(0),
      m_size(0),
      m_shared(false),
      m_buf(0)
    {}

  BinaryData::BinaryData(const BinaryData & that)
    : m_current(0),
      m_total(0),
      m_size(0),
      m_shared(false),
      m_buf(0)
  {
    *this = that;
  }

  BinaryData::~BinaryData()
  {
    if(!m_shared)
      free(m_buf);
  }

  void BinaryData::writeFloat32(float v)
  {
    ensure(8);
    getRef<int32_t>() = FLOAT_MARKER;
    getRef<float>()   = v;
  }

  void BinaryData::writeInt32(int32_t v)
  {
    ensure(8);
    getRef<int32_t>() = INT32_MARKER;
    getRef<int32_t>() = v;
  }

  void BinaryData::writeInt64(int64_t v)
  {
    ensure(12);
    getRef<int64_t>() = INT64_MARKER;
    getRef<int64_t>() = v;
  }
  
  void BinaryData::writeTimeStamp(int64_t v)
  {
    ensure(12);
    getRef<int32_t>() = TS_MARKER;
    getRef<int64_t>() = v;
  }
    
  void BinaryData::writeString(const char * s)
  {
    int len = strlen(s);
    int space = stringSpace(s);
    ensure(4 + space);

    getRef<int32_t>() = STRING_MARKER;
    char * ptr =  getPtr<char>(space);
    if(len)
      memcpy(ptr, s, len + 1);
    else
      getRef<int32_t>() = 0;
  }

  void BinaryData::writeWString(const std::wstring & str)
  {
    ensure(8 + str.size() * 4);

    getRef<int32_t>() = WSTRING_MARKER;
    getRef<int32_t>() = str.size();

    for(unsigned i = 0; i < str.size(); i++) {
      getRef<int32_t>() = str[i];
    }
  }

  void BinaryData::writeBlob(const void * ptr, int n)
  {
    ensure(8 + n);

    getRef<int32_t>() = BLOB_MARKER;
    getRef<int32_t>() = n;
    char * dest =  getPtr<char>(n);
    if(n)
      memcpy(dest, ptr, n);
  }

  void BinaryData::writeVector2Float32(Nimble::Vector2f v)
  {
    ensure(12);
    getRef<int32_t>() = VECTOR2F_MARKER;
    getRef<float>() = v[0];
    getRef<float>() = v[1];
  }

  void BinaryData::writeVector2Int32(Nimble::Vector2i v)
  {
    ensure(12);
    getRef<int32_t>() = VECTOR2I_MARKER;
    getRef<int>() = v[0];
    getRef<int>() = v[1];
  }

  void BinaryData::writeVector4Int32(const Nimble::Vector4i & v)
  {
    ensure(20);
    getRef<int32_t>() = VECTOR4I_MARKER;
    getRef<int>() = v[0];
    getRef<int>() = v[1];
    getRef<int>() = v[2];
    getRef<int>() = v[3];
  }

  void BinaryData::writeVector4Float32(const Nimble::Vector4f & v)
  {
    ensure(20);
    getRef<int32_t>() = VECTOR4F_MARKER;
    getRef<float>() = v[0];
    getRef<float>() = v[1];
    getRef<float>() = v[2];
    getRef<float>() = v[3];
  }

  void BinaryData::append(const BinaryData & that)
  {
    ensure(that.pos());
    memcpy(getPtr<char>(that.pos()), that.data(), that.pos());
  }

  float BinaryData::readFloat32(bool * ok)
  {
    if(!available(8)) {
      if(ok) * ok = false;
      return 0.0f;
    }

    int32_t marker = getRef<int32_t>();

    if(marker == FLOAT_MARKER)
      return getRef<float>();
    else if(marker == INT32_MARKER)
      return float(getRef<int32_t>());
    else if(ok)
      *ok = false;

    skipParameter(marker);
    
    return 0.0f;
  }

  int32_t BinaryData::readInt32(bool * ok)
  {
    if(!available(8)) {
      if(ok) * ok = false;
      unavailable("BinaryData::readInt32");
      return 0;
    }

    int32_t marker = getRef<int32_t>();

    if(marker == INT32_MARKER)
      return getRef<int32_t>();
    else if(marker == INT64_MARKER)
      return int32_t(getRef<int64_t>());
    else if(marker == FLOAT_MARKER)
      return Nimble::Math::Round(getRef<float>());
    else if(ok) {
      badmarker("BinaryData::readInt32", marker);
      *ok = false;
    }

    skipParameter(marker);
    return 0;
  }

  int64_t BinaryData::readInt64(bool * ok)
  {
    if(!available(8)) {
      if(ok) * ok = false;
      unavailable("BinaryData::readInt64");
      return 0;
    }

    int64_t marker = getRef<int64_t>();

    if(marker == INT64_MARKER)
      return getRef<int64_t>();
    else if(marker == INT32_MARKER)
      return getRef<int32_t>();
    else if(marker == FLOAT_MARKER)
      return Nimble::Math::Round(getRef<float>());
    else if(ok) {
      badmarker("BinaryData::readInt64", int32_t(marker));
      *ok = false;
    }

    skipParameter(int(marker));
    return 0;
  }

  int64_t BinaryData::readTimeStamp(bool * ok)
  {
    if(!available(12)) {
      if(ok) * ok = false;
      return 0;
    }

    int32_t marker = getRef<int32_t>();

    if(marker == TS_MARKER)
      return getRef<int64_t>();
    else if(ok)
      *ok = false;

    skipParameter(marker);
    return 0;
  }
  
  bool BinaryData::readString(char * str, int maxbytes)
  {
    int32_t marker = getRef<int32_t>();

    if(marker != STRING_MARKER) {
      skipParameter(marker);
      return false;
    }
    const char * source = & m_buf[m_current];
    int len = strlen(source);

    skipParameter(marker);

    if(len >= maxbytes) {
      str[0] = 0;
      return false;
    }
    
    memcpy(str, source, len);
    str[len] = '\0';

    return true;
  }

  bool BinaryData::readString(std::string & str)
  {
    int32_t marker = getRef<int32_t>();

    if(marker != STRING_MARKER) {
      skipParameter(marker);
      return false;
    }
    const char * source = & m_buf[m_current];
    int len = strlen(source);

    skipParameter(marker);

    str.resize(len);

    memcpy( & str[0], source, len);

    return true;
  }

  bool BinaryData::readWString(std::wstring & str)
  {
    int32_t marker = getRef<int32_t>();

    if(marker != WSTRING_MARKER) {
      skipParameter(marker);
      return false;
    }
    int len = getRef<int32_t>();

    str.resize(len);

    for(int i = 0; i < len; i++) {
      str[i] = getRef<int32_t>();
    }

    return true;
  }

  bool BinaryData::readBlob(void * ptr, int n)
  {
    int32_t marker = getRef<int32_t>();

    if(marker != BLOB_MARKER) {
      skipParameter(marker);
      return false;
    }

    int32_t recv = getRef<int32_t>();

    const char * source = & m_buf[m_current];

    skipParameter(marker);

    memcpy( ptr, source, Nimble::Math::Min(n, recv));
    
    return n == recv;
  }

  Nimble::Vector2f BinaryData::readVector2Float32(bool * ok)
  {
    if(!available(12)) {
      if(ok) * ok = false;
      return Nimble::Vector2f(0, 0);
    }

    int32_t marker = getRef<int32_t>();

    if(marker == VECTOR2F_MARKER) {
      return getRef<Nimble::Vector2f>();
    }
    else if(marker == VECTOR2I_MARKER) {
      return getRef<Nimble::Vector2i>();
    }
    else {
      skipParameter(marker);
      if(ok)
	*ok = false;

      return Nimble::Vector2f(0, 0);
    }
    
  }

  Nimble::Vector2i BinaryData::readVector2Int32(bool * ok)
  {
    if(!available(12)) {
      if(ok) * ok = false;
      return Nimble::Vector2f(0, 0);
    }

    int32_t marker = getRef<int32_t>();

    if(marker == VECTOR2I_MARKER) {
      return getRef<Nimble::Vector2i>();
    }
    else if(marker == VECTOR2F_MARKER) {
      return getRef<Nimble::Vector2f>();
    }
    else {
      skipParameter(marker);
      if(ok)
	*ok = false;

      return Nimble::Vector2f(0, 0);
    }
    
  }

  Nimble::Vector4i BinaryData::readVector4Int32(bool * ok)
  {
    if(!available(12)) {
      if(ok) * ok = false;
      return Nimble::Vector4i(0, 0, 0, 1);
    }

    int32_t marker = getRef<int32_t>();

    if(marker == VECTOR4I_MARKER) {
      return getRef<Nimble::Vector4i>();
    }
    else if(marker == VECTOR4F_MARKER) {
      return getRef<Nimble::Vector4f>();
    }
    else {
      skipParameter(marker);
      if(ok)
	*ok = false;

      return Nimble::Vector4i(0, 0, 0, 1);
    }
    
  }


  Nimble::Vector4f BinaryData::readVector4Float32(bool * ok)
  {
    if(!available(12)) {
      if(ok) * ok = false;
      return Nimble::Vector4f(0, 0, 0, 1);
    }

    int32_t marker = getRef<int32_t>();

    if(marker == VECTOR4I_MARKER) {
      return getRef<Nimble::Vector4i>();
    }
    else if(marker == VECTOR4F_MARKER) {
      return getRef<Nimble::Vector4f>();
    }
    else {
      skipParameter(marker);
      if(ok)
	*ok = false;

      return Nimble::Vector4f(0, 0, 0, 1);
    }
    
  }


  bool BinaryData::write(Radiant::BinaryStream * stream)
  {
    int32_t s = pos();
    if(stream->write(&s, 4) != 4)
      return false;

    return stream->write( & m_buf[0], s) == s;
  }

  bool BinaryData::read(Radiant::BinaryStream * stream)
  {
    uint32_t s = 0;
    m_current = 0;
    m_total = 0;

    if(stream->read(&s, 4) != 4)
      return false;

    if(m_size < s) {
      if(s > 500000000) { // Not more than 500 MB at once, please
        Radiant::error("BinaryData::read # Attempting extraordinary read (%d bytes)", s);
	return false;
      }
      ensure(s + 8);
      // m_size = s;
    }
    
    int n = stream->read( & m_buf[0], s);
    if(n != (int) s) {
      error("BinaryData::read # buffer read failed (got %d != %d)",
	    n, s);
      return false;
    }

    m_current = 0;
    m_total = s;

    return true;
  }

  void BinaryData::linkTo(void * data, int capacity)
  {
    if(!m_shared && m_buf)
      free(m_buf);
    
    m_buf = (char *) data;
    m_size = capacity;
    m_shared = true;
  }

  void BinaryData::ensure(unsigned bytes)
  {
    unsigned need = m_current + bytes;
    if(need > m_size) {
      if(m_shared)
	fatal("BinaryData::ensure # Sharing data, cannot ensure required space");
      
      m_size = need + 128 + need / 16;
      m_buf = (char *) realloc(m_buf, m_size);
    }
    m_total = m_current + bytes;
  }

  void BinaryData::clear()
  {
    rewind();
    
    bzero(data(), m_size);
  }

  void BinaryData::skipParameter(int marker)
  {
    if(marker == INT32_MARKER ||
       marker == FLOAT_MARKER)
      m_current += 4;
    else if(marker == TS_MARKER ||
	    marker == VECTOR2F_MARKER ||
	    marker == VECTOR2I_MARKER)
      m_current += 8;
    else if(marker == STRING_MARKER) {
      const char * str = & m_buf[m_current];
      m_current += stringSpace(str);
    }
    else if(marker == WSTRING_MARKER) {
      int len = getRef<int32_t>();
      m_current += len * 4;
    }
    else if(marker == BLOB_MARKER) {
      int n = getRef<int32_t>();
      m_current += n;
    }
  }
  
  int BinaryData::stringSpace(const char * str)
  {
    int len = strlen(str) + 1;

    int rem = len & 0x3;

    int pad = rem ? 4 - rem : 0;
    return len + pad;
  }

  void BinaryData::unavailable(const char * func)
  {
    if(!__verbose)
      return;
    Radiant::error("%s # Not enough data available (at %u/%u)",
		   func, m_current, m_total);
  }

}
