#ifndef DYSLEXIC_GPU_MANAGED_FONT_HPP
#define DYSLEXIC_GPU_MANAGED_FONT_HPP

#include <Luminous/GLResource.hpp>

#include <Nimble/Matrix3.hpp>

#include <vector>

namespace Dyslexic
{
  class GPUFontBase;
  class CPUManagedFont;
  class GPUFont;

  /// A managed font that contains multiple sizes of a single font residing on
  /// the GPU
  class GPUManagedFont : public Luminous::GLResource
  {
    public:
      GPUManagedFont(CPUManagedFont * font, Luminous::GLResources * glrc);

      void render(const std::wstring & text, int pointSize, const Nimble::Matrix3 & m);
      void render(const std::string & text, int pointSize, const Nimble::Matrix3 & m);

    private:
      typedef std::vector<GPUFontBase *> container;

      float extractScale(const Nimble::Matrix3 & m);

      void computeRenderParams(const Nimble::Matrix3 & m, int pts, GPUFont ** gf, float * scale);

      GPUFontBase * getFont(int fontNo);
      
      CPUManagedFont * m_cmf;  
      Luminous::GLResources * m_resources;
      container m_fonts;
  };


}

#endif