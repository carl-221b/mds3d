#include "integrator.h"
#include "scene.h"

class DirectIntegrator : public Integrator {
public:
    DirectIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {
        /** TODO : Find the surface that is visible in the requested direction
                   Return its ambiant color */
        Hit hit;
        scene->intersect(ray,hit);

        Color3f res;
        res.setZero();

        LightList l=scene->lightList();
        if(hit.foundIntersection()){

          for (size_t i=0; i<l.size(); ++i) {
            Point3f intersection=ray.at(hit.t());
            float dist;
            Vector3f ldir= l[i]->direction(intersection,&dist);
            Vector3f vdir=ray.direction;
            ldir.normalize();

            Ray rShade(intersection + hit.normal() *0.0001,ldir);
            Hit shadeHit;
            scene->intersect(rShade,shadeHit);
            if (!shadeHit.foundIntersection() || shadeHit.t()>dist){   //si il y a pas d'objet entre lumière et l'objet à afficher OU s'il y a un objet mais derrière la lumière
              float lin= ldir.dot(hit.normal());
              if(lin<0){
                lin=0;
              }
              res+= hit.shape()->material()->brdf(vdir,-ldir,hit.normal(),hit.uv()) * lin * l[i]->intensity(intersection);
          }
          }
      }
      else{
        res=scene->backgroundColor();
      }
      return res;
  }
    std::string toString() const {
        return "DirectIntegrator[]";
    }
};

REGISTER_CLASS(DirectIntegrator, "direct")
