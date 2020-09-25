#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 v_tex;

uniform sampler2D tex_1;
uniform sampler2D tex_2;
uniform sampler2D tex_3;
uniform sampler2D tex_4;
uniform sampler2D tex_5;

uniform vec3 lightDir;

out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  vec3 res = vec3(0,0,0);
  float dc = max(0,dot(n,l));
  if(dc>0) {
    res = dCol * dc;
    float sc = max(0,dot(n,normalize(v+l)));
    if(sc>0)
      res += sCol * pow(sc,s) * dc;
  }
  return res;
}

void main(void) {
  float ambient = 0.0;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);
//  out_color = vec4(ambient * v_color + blinn(normalize(v_normal),normalize(v_view), lightDir, v_color, spec_color, shininess),1.0);
  float light_factor=max(0,dot(v_normal,lightDir)) ;
  vec4 earth =light_factor * texture(tex_1,v_tex);
  vec4 cloud = light_factor * texture(tex_2,v_tex);
  vec4 night = texture(tex_3,v_tex);

  vec4 earth_cloud=mix(earth,cloud,cloud);

  vec4 earth_cloud_night= mix(night,earth_cloud,light_factor);
//  out_color= earth_cloud_night ;
// ------------------------------------------------------------------------
// plane
//  out_color=texture(tex_4,v_tex);
// ------------------------------------------------------------------------
// cow
//  out_color=texture(tex_5,v_tex);

// ------------------------------------------------------------------------

   out_color = vec4(ambient * v_color + blinn(normalize(v_normal),normalize(v_view), lightDir, v_color, spec_color, shininess),1.0);
}
