#include "plane.h"

Plane::Plane()
{
}

Plane::Plane(const PropertyList &propList)
{
    m_position = propList.getPoint("position",Point3f(0,0,0));
    m_normal = propList.getVector("normal",Point3f(0,0,1));
}

Plane::~Plane()
{
}

bool Plane::intersect(const Ray& ray, Hit& hit) const
{
    float noa=m_normal.dot(-ray.origin+m_position);
    float dn=ray.direction.dot(m_normal);

    if(dn==0){
      return false;
      //tend vers 0 = le rayon est parallèle et distinct du plan
    }

    float t= noa/dn;

    if(t>0){
      hit.setT(t);
      hit.setShape(this);
      hit.setNormal(m_normal);
      Point3f intersect=ray.at(hit.t());
      Vector2f uv(intersect.x(),intersect.y());
      hit.setUV(uv);
      return true;
    }
    else
      return false;
}



REGISTER_CLASS(Plane, "plane")
