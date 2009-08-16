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

#include "TCPSocket.hpp"

#include "Sleep.hpp"
#include "Trace.hpp"

#include <QTcpSocket>

#include <errno.h>

namespace Radiant
{

  class TCPSocket::D : public QTcpSocket
  {};

  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  TCPSocket::TCPSocket()
  {
    m_d = new D();
  }

  TCPSocket::TCPSocket(int fd)
  {
    m_d = new D();
    if(!m_d->setSocketDescriptor(fd))
      Radiant::error("TCPSocket::TCPSocket # setSocketDescriptor failed");
  }

  TCPSocket::~TCPSocket()
  {
    delete m_d;
  }

  int TCPSocket::open(const char * host, int port)
  {
    close();

    m_d->connectToHost(host, port);

    bool ok = m_d->waitForConnected(5000);

    if(!ok) {
      QString errstr = m_d->errorString() ;
      error("TCPSocket::open # %s", errstr.toStdString().c_str());
      return EINVAL;
    }

    return 0;
  }

  bool TCPSocket::close()
  {
    int count = 0;

    while(m_d->bytesToWrite() && count < 500) {
      Radiant::Sleep::sleepMs(5);
      count++;
    }

    m_d->close();

    return true;
  }
 
  bool TCPSocket::isOpen() const
  {
    return m_d->isValid();
  }

  const char * TCPSocket::host() const
  {
    return m_d->peerName().toAscii();
  }

  int TCPSocket::port() const
  {
    return m_d->peerPort();
  }

  int TCPSocket::read(void * buffer, int bytes)
  {
    int got = 0;
    char * ptr = (char *) buffer;

    while(got < bytes && m_d->state() == QAbstractSocket::ConnectedState) {

      bool something = m_d->waitForReadyRead(10000);
      int n = m_d->read(ptr, bytes - got);
      got += n;
    }
    
    info("TCPSocket::read # state = %d", (int)  m_d->state());

    return got;
  }

  int TCPSocket::write(const void * buffer, int bytes)
  {
    int n = m_d->write((const char *)buffer, bytes);

    if(n == bytes)
      m_d->flush();

    return n;
  }

  bool TCPSocket::isHungUp() const
  {
    return (m_d->state() != QAbstractSocket::ConnectedState);
//    return m_d->q->isValid();
  }

  bool TCPSocket::isPendingInput(unsigned int waitMicroSeconds)
  {
    return m_d->waitForReadyRead(waitMicroSeconds);
  }

  void TCPSocket::debug()
  {
    Radiant::info("TCPSocket::debug #");
    Radiant::info("\tSTATE %d", m_d->state());
    Radiant::info("\tVALID %d", m_d->isValid());
    Radiant::info("\tERROR %d", m_d->error());
  }

}
