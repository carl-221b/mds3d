#include "integrator.h"
#include "scene.h"

class NormalsIntegrator : public Integrator {
public:
    NormalsIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {
        /** TODO : Find the surface that is visible in the requested direction
                   Return its ambiant color */
        Hit hit;

        scene->intersect(ray,hit);
        if(hit.foundIntersection()){
          float x=abs(hit.normal().x());
          float y=abs(hit.normal().y());
          float z=abs(hit.normal().z());
          return Color3f(x,y,z);
        }
        return scene->backgroundColor();
    }

    std::string toString() const {
        return "NormalsIntegrator[]";
    }
};

REGISTER_CLASS(NormalsIntegrator, "normals")
