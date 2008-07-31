#include "ValueListener.hpp"

#include <algorithm>

namespace Valuable
{  


  ValueListener::~ValueListener()
  {
    for(vliterator it = m_listening.begin(); it != m_listening.end(); it++)
      (*it)->remove(this);
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  ValueListeners::ValueListeners(const ValueListeners & that)
  {
    if(that.size())
      m_list = new container(*that.m_list);
    else
      m_list = 0;
  }

  ValueListeners::~ValueListeners()
  {
    delete m_list;
  }

  void ValueListeners::push_back(ValueListener * listener)
  { 
    makeList();
    m_list->push_back(listener);
    listener->m_listening.push_back(this);
  }

  void ValueListeners::remove(ValueListener * listener)
  {
    if(!m_list)
      return;

    iterator it = std::find(begin(), end(), listener);

    if(it != end())
      m_list->erase(it);
  }

  void ValueListeners::emitChange(ValueObject * obj)
  {
    if(!m_list)
      return;

    for(iterator it = begin(); it != end(); it++) {
      (*it)->valueChanged(obj);
    }
  }

  void ValueListeners::emitDelete(ValueObject * obj)
  {
    if(!m_list)
      return;

    for(iterator it = begin(); it != end(); it++) {
      (*it)->valueDeleted(obj);
    }
  }
  

  ValueListeners & ValueListeners::operator = (const ValueListeners & that)
  {
    if(that.size()) {
      makeList();
      *m_list = *that.m_list;
    }
    else {
      delete m_list;
      m_list = 0;
    }

    return * this;
  }
}
