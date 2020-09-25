#include "integrator.h"
#include "scene.h"

class TexcoordsIntegrator : public Integrator {
public:
    TexcoordsIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {
        /** TODO : Find the surface that is visible in the requested direction
                   Return its ambiant color */
       Hit hit;

       scene->intersect(ray,hit);
       if(hit.foundIntersection()){
         float x=hit.uv().x();
         float y=hit.uv().y();
         float z=0;
         return Color3f(x,y,z);
       }
       //return scene->backgroundColor();
       return Color3f(0.0f);
    }

    std::string toString() const {
        return "TexcoordsIntegrator[]";
    }
};

REGISTER_CLASS(TexcoordsIntegrator, "texcoords")
