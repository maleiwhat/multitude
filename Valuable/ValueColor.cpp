#include "ValueColor.hpp"

#include <Radiant/StringUtils.hpp>

namespace Valuable
{

  ValueColor::ValueColor(HasValues * parent, const std::string & name, const Radiant::Color & c, bool transit)
    : ValueObject(parent, name, transit),
    m_color(c)
  {}

  bool ValueColor::deserializeXML(xercesc::DOMElement * element) {
    using namespace xercesc;

    const XMLCh * content = element->getTextContent();
    char * myContent = XMLString::transcode(content);

    std::stringstream in(myContent);

    for(int i = 0; i < 4; i++) {
      in >> m_color[i];
    }

    XMLString::release(&myContent);

    return true;
  }

  std::string ValueColor::asString(bool * const ok) const {
    if(ok) *ok = true;

    std::string r = Radiant::StringUtils::stringify(m_color[0]);

    for(int i = 1; i < 4; i++)
      r += std::string(" ") + Radiant::StringUtils::stringify(m_color[i]);

    return r;
  }

  bool ValueColor::set(const Nimble::Vector4f & v)
  {
    m_color = v;
    return true;
  }

}
