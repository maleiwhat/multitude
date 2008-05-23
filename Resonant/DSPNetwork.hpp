/* COPYRIGHT
 *
 * This file is part of Resonant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Resonant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef RESONANT_DSPNETWORK_HPP
#define RESONANT_DSPNETWORK_HPP

#include <Resonant/AudioLoop.hpp>
#include <Resonant/ControlData.hpp>
#include <Resonant/Module.hpp>

#include <Radiant/Mutex.hpp>

#include <Radiant/RefPtr.hpp>

#include <list>
#include <vector>
#include <cassert>

namespace Resonant {

  using Radiant::RefPtr;

  class ModuleOutCollect;

  /** Stuff that needs implementing: 

      - When a module is put into the graph (that's running) it can
      end up with getting input from a source that is invalidated by
      later modules. This should be fixed once we get to having
      trouble...
  */

  class MTEXPORT DSPNetwork : public AudioLoop
  {
  public:

    /** Note the lack of destructor. You need to call "clear" manually. */
    class Buf
    {
    public:

      Buf() : m_data(0), m_size(0)
      {}
      
      void allocate(int n) 
      {
	if(n != m_size) {
	  delete [] m_data; 
	  m_data = new float [n];
	  m_size = n;
	}
      }

      void init() { allocate(Module::MAX_CYCLE); }

      void clear() { delete [] m_data; m_data = 0; m_size = 0; }

      float * m_data;
      int     m_size;
    };
    
    class MTEXPORT Connection
    {
    public:
      Connection() : m_channel(0),m_buf(0) { m_moduleId[0] = '\0'; }
      Connection(const char * moduleId, int channel)
        : m_channel(channel),m_buf(0)
      {
	setModuleId(moduleId);
      }
      
      void setModuleId(const char * id)
      {
	if(id) strcpy(m_moduleId, id);
	else m_moduleId[0] = '\0';
      }

      inline bool operator == (const Connection & that) const
      {
        return (strcmp(m_moduleId, that.m_moduleId) == 0) &&
          (m_channel == that.m_channel);
      }

      char        m_moduleId[Module::MAX_ID_LENGTH];
      int         m_channel;
      Buf        *m_buf;
    };

    class MTEXPORT NewConnection
    {
    public:
      NewConnection() : m_sourceChannel(0), m_targetChannel(0) {}

      char        m_sourceId[Module::MAX_ID_LENGTH];
      char        m_targetId[Module::MAX_ID_LENGTH];
      int         m_sourceChannel;
      int         m_targetChannel;
    };

    class MTEXPORT Item
    {
    public:
      Item();
      ~Item();

      inline void process(int n)
      {
	assert(m_compiled != false);
	m_module->process(& m_ins[0], & m_outs[0], n);
      }

      void eraseInput(const Connection & c);
      int findInInput(float * ptr) const;
      int findInOutput(float * ptr) const;

      Module * m_module;

      std::vector<Connection> m_inputs;

      std::list<NewConnection> m_connections;
      std::vector<float *> m_ins;
      std::vector<float *> m_outs;
      
      bool m_compiled;
      bool m_done;
    };

    typedef std::list<Item> container;
    typedef container::iterator iterator;

    DSPNetwork();
    virtual ~DSPNetwork();

    bool start(const char * device);

    int outChannels() { return outParameters().channelCount; }

    void addModule(Item &);
    void markDone(Item &);

  protected:

    virtual int callback(const void *in, void *out,
			 unsigned long framesPerBuffer,
			 const PaStreamCallbackTimeInfo* time,
			 PaStreamCallbackFlags status);
    
    void doCycle(int);

    void checkNewItems();
    void checkDoneItems();

    bool uncompile(Item &);
    bool compile(Item &);
    bool compile(Item &, int);
    Buf & findFreeBuf(int);
    bool bufIsFree(int, int);
    void checkValidId(Item &);
    Item * findItem(const char * id);
    Module * findModule(const char * id);
    float * findOutput(const char * id, int channel);
    
    container m_items;

    container m_newItems;

    std::vector<Buf> m_buffers;
    
    ModuleOutCollect *m_collect;

    ControlData m_controlData;
    
    char        m_devName[128];
    bool        m_continue;
    long        m_frames;
    int         m_doneCount;

    Radiant::MutexAuto m_newMutex;
  };

}

#endif
