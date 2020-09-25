#include "integrator.h"
#include "scene.h"

class WhittedIntegrator : public Integrator {
public:
  WhittedIntegrator(const PropertyList &props) {
    m_maxRecursion = props.getInteger("maxRecursion");
  }



  Color3f Li(const Scene *scene, const Ray &ray) const {
    /** TODO : Find the surface that is visible in the requested direction
    Return its ambiant color */
    Hit hit;
    scene->intersect(ray,hit);

    Color3f res(0.0f);

    LightList l=scene->lightList();
    if(hit.foundIntersection()){
      Point3f intersection=ray.at(hit.t());
      Vector3f vdir=ray.direction;
      for (size_t i=0; i<l.size(); ++i) {
        float dist;
        Vector3f ldir= l[i]->direction(intersection,&dist);
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


    if(ray.recursionLevel<m_maxRecursion){

      /* Reflexion */
      if(!hit.shape()->material()->reflectivity().isZero()){
        Vector3f r=2*hit.normal().dot(-ray.direction) * hit.normal() + ray.direction;
        r.normalize();
        Ray rayRebound(intersection + hit.normal() *0.0001 ,r);
        rayRebound.recursionLevel=ray.recursionLevel+1;
        res+= Li(scene,rayRebound) * hit.shape()->material()->reflectivity() * r.dot(hit.normal());
      }

      /* Refraction */
      if(!hit.shape()->material()->transmissivness().isZero()){
        float eta=hit.shape()->material()->etaA()/hit.shape()->material()->etaB();
        Vector3f rdir= ray.direction;
        Normal3f norm= hit.normal();
        float cost1 = norm.dot(-rdir);
        float cost2= sqrt(1 - eta * eta * (1 - cost1 * cost1));
        // https://fr.wikipedia.org/wiki/Lois_de_Snell-Descartes#Forme_vectorielle_des_lois_de_Snell-Descartes
        // Last opened : 23/01/2020 18:40
        Vector3f rRefra;
        if(cost1>0){
           rRefra = eta * rdir+ (eta * cost1 - cost2)* norm;
        }
        else{
          eta=hit.shape()->material()->etaB()/hit.shape()->material()->etaA();
          //hit.setNormal(-hit.normal());
          norm=-norm;
          cost1 = norm.dot(-rdir);
          cost2= sqrt(1 - eta * eta * (1 - cost1 * cost1));
          rRefra = eta * rdir + (eta * cost1 - cost2)* norm;
        }
        rRefra=rRefra.normalized();
        Ray rayFraction(ray.at(hit.t()) + rdir * 0.0001 , rRefra);
        rayFraction.recursionLevel=ray.recursionLevel+1;
        res+= Li(scene,rayFraction) * hit.shape()->material()->transmissivness() ;
      }
    }
  }

  else if(ray.recursionLevel==0){
    return scene->backgroundColor();
  }
  else
    return Color3f(0.0f);

  Color3f res2(fabs(res.r()),fabs(res.g()),fabs(res.b()));

  return res2;
}

std::string toString() const {
  return "WhittedIntegrator[]";
}

protected:
  int m_maxRecursion;
};

REGISTER_CLASS(WhittedIntegrator, "whitted")
