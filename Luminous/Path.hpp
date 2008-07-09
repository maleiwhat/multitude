#ifndef FP_PATH_HPP
#define FP_PATH_HPP

#include <Valuable/HasValues.hpp>

#include <Nimble/Matrix3.hpp>
#include <Nimble/Vector2.hpp>

#include <Luminous/GLSLProgramObject.hpp>

#include <vector>

#include <Luminous/TCBSpline.hpp>

class Path : public Valuable::HasValues
{
  public:
    Path(long id);

    void addPoint(Nimble::Vector2f p) { m_points.push_back(p); }

    void render();

    void renderSpline();
    void renderLineStrip() const;

    void renderDebug() const;

    void prepare();

    long id() const { return m_id; }

    void simplify(float clusterTolerance, float dpTolerance);

    void simplifyAngular(float degrees);

    void transform(const Nimble::Matrix3f & m);

    size_t size() const { return m_points.size(); }
    Nimble::Vector2f point(size_t i) const { return m_points[i]; }

    const char * const type() const { return "Path"; }
    xercesc::DOMElement * serializeXML(xercesc::DOMDocument * doc);
    bool deserializeXML(xercesc::DOMElement * element);

    Nimble::Vector2f center() const;

    static bool intersect(const Path & p1, const Nimble::Matrix3f & m1, const Path & p2, const Nimble::Matrix3f & m2);

    bool isDegenerate() const;

    TCBSpline2 * spline();

    void debugDump() const;
  
  protected:
    typedef std::vector<Nimble::Vector2f> container;

    long m_id;
    container m_points;
};

#endif
