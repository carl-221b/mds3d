#include "sphere.h"
#include <iostream>

Sphere::Sphere(float radius)
    : m_radius(radius)
{
}

Sphere::Sphere(const PropertyList &propList)
{
    m_radius = propList.getFloat("radius",1.f);
    m_center = propList.getPoint("center",Point3f(0,0,0));
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Ray& ray, Hit& hit) const
{
    /// TODO: compute ray-sphere intersection

    float a= (ray.direction.dot(ray.direction));
    float b= 2*((ray.direction.dot(ray.origin-m_center)));
    float c= ((ray.origin-m_center).dot(ray.origin-m_center)) - (m_radius*m_radius);

    float delta=(b*b)-4*a*c;

    if (delta>=0){
      if (delta==0){
        hit.setT((-b)/2*a);
      }
      if(delta>0){
        float r1=((-b)-sqrt(delta))/(2*a);
        float r2=((-b)+sqrt(delta)/(2*a));
        if(r1>0){
          hit.setT(r1);
        }
        else{
          hit.setT(r2);
        }
      }
      hit.setShape(this);

      Point3f p= ray.at(hit.t());
      Vector3f norm= p-m_center;


      float u= 0.5+atan2(norm.x(),norm.y())/(2*M_PI);
      float v= 0.5 + asin(norm.z()/m_radius)/M_PI;
      Vector2f uv(u,v);
      hit.setUV(uv);

      norm.normalize();
      hit.setNormal(norm);


      return true;
    }

    return false;

}

REGISTER_CLASS(Sphere, "sphere")
