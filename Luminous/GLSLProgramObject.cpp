/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Luminous/GLSLProgramObject.hpp>
#include <Radiant/FileUtils.hpp>
#include <iostream>

using namespace std;

namespace Luminous
{

  GLSLProgramObject::GLSLProgramObject(GLResources * resources)
    : GLResource(resources),
      m_isLinked(false)
  {
    m_handle = glCreateProgram();
  }

  GLSLProgramObject::~GLSLProgramObject()
  {
    GLSLProgramObject::clear();
    
    glDeleteProgram(m_handle);
  }

  void GLSLProgramObject::addObject(GLSLShaderObject* obj)
  {
    if(obj == 0) {
      cerr << "GLSLProgramObject::addObject # attempt to add "
	"null shader object" << endl;
      return;
    }

    if(!obj->m_isCompiled) {
      cerr << "GLSLProgramObject::addObject # attempt to add "
	"non-compiled object: trying to compile it...";
      if(!obj->compile()) {
        cerr << "failed." << endl;
        return;
      } else {
        cerr << "ok." << endl;
      }
    }

    m_shaderObjects.push_back(obj);
  }

  bool GLSLProgramObject::link()
  {
    list<GLSLShaderObject*>::iterator i;

    if(m_isLinked) {
      cerr << "GLSLProgramObject::link # program already "
	"linked, trying to re-link" << endl;
      for(i = m_shaderObjects.begin(); i != m_shaderObjects.end(); i++) {
        glDetachShader(m_handle, (*i)->m_handle);
      }
    }

    for(i = m_shaderObjects.begin(); i != m_shaderObjects.end(); i++) {
      glAttachShader(m_handle, (*i)->m_handle);
    }

    glLinkProgram(m_handle);

    GLint linked;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &linked);

    if(linked) {
      m_isLinked = true;
      cout << linkerLog() << endl;
    } else  {
      cerr << "GLSLProgramObject::link # linking failed." << endl;
      m_isLinked = false;
    }

    return m_isLinked;
  }

  void GLSLProgramObject::clear()
  {    
    std::list<GLSLShaderObject*>::iterator i;
    for(i = m_shaderObjects.begin(); i != m_shaderObjects.end(); i++) {
      glDetachShader(m_handle, (*i)->m_handle);
      delete (*i);
    }

    m_shaderObjects.clear();
  }

  const char* GLSLProgramObject::linkerLog()
  {
    if(m_handle == 0) {
      cerr << "GLSLProgramObject::linkerLog # program object is null" << endl;
      return 0;
    }

#if 0
    GLint logLen = 0;
    glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &logLen);

    if(logLen == 0) {
      m_linkerLog.clear();
      return 0;
    }

    m_linkerLog.resize(logLen);

    GLsizei readLen;
    glGetProgramInfoLog(m_handle, logLen, &readLen, & m_linkerLog[0]);
#else
    m_linkerLog.resize(512);
    
    glGetProgramInfoLog(m_handle, 512, 0, & m_linkerLog[0]);
#endif

    return & m_linkerLog[0];
  }

  void GLSLProgramObject::bind()
  {
    if(m_handle == 0) {
      cerr << "GLSLProgramObject::bind # attempt to bind null program" << endl;
      return;
    }

    if(!m_isLinked) {
      cerr << "GLSLProgramObject::bind # attempt to "
	"bind program that is not linked" << endl;
      return;
    }

    glUseProgram(m_handle);
  }

  void GLSLProgramObject::unbind()
  {
    glUseProgram(0);
  }

  int GLSLProgramObject::getUniformLoc(const std::string& name)
  {
    return glGetUniformLocation(m_handle, name.c_str());
  }

  int GLSLProgramObject::getUniformLoc(const char * name)
  {
    return glGetUniformLocation(m_handle, name);
  }

  bool GLSLProgramObject::validate()
  {
    glValidateProgram(m_handle);

    GLint status;
    glGetProgramiv(m_handle, GL_VALIDATE_STATUS, &status);

    return (status == GL_TRUE);
  }

  GLSLProgramObject* GLSLProgramObject::fromFiles
  (const char* vsFile, const char* fsFile)
  {
    // Load & compile vertex shader
    GLSLShaderObject* vs = 0;
    if(vsFile) {
      vs = new GLSLShaderObject(GL_VERTEX_SHADER);

      char* code = Radiant::FileUtils::loadTextFile(vsFile);
      vs->setSource(code);

      delete [] code;

      if(!vs->compile()) {
        cerr << "GLSLProgramObject::fromFiles # vertex shader compile error:"
	     << endl << vs->compilerLog() << endl;
        delete vs;
        return 0;
      }
    }

    // Load & compile fragment shader
    GLSLShaderObject* fs = 0;
    if(fsFile) {
      fs = new GLSLShaderObject(GL_FRAGMENT_SHADER);
      
      char* code = Radiant::FileUtils::loadTextFile(fsFile); 
      fs->setSource(code);
  
      delete [] code;

      if(!fs->compile()) {
        cerr << "GLSLProgramObject::fromFiles # fragment shader "
	  "compile error:" << endl << fs->compilerLog() << endl;
        delete fs;
        return 0;
      }
    }
  
    // Create a program object and link it
    GLSLProgramObject* program = new GLSLProgramObject();

    if(vs) program->addObject(vs);
    if(fs) program->addObject(fs);

    if(!program->link()) {
      cerr << "GLSLProgramObject::fromFiles # linking shader failed:"
	   << endl << program->linkerLog() << endl;
      delete program;
      return 0;
    }

    return program;
  }
}

