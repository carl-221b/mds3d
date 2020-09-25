#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 v_tex;
in vec3 v_tangent;
in vec3 v_bitangent;

uniform vec3 lightDir;
uniform sampler2D tex_1;
uniform sampler2D tex_2;
uniform sampler2D tex_3;
uniform sampler2D tex_4;


uniform sampler2D tex_5;
uniform sampler2D tex_6;
uniform mat3 normal_mat;

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


  // ------------------------------------------------------------ Normal mapping sur un plan ------------------------------------------------------------
//  vec4 textget = texture(tex_1,v_tex);
//  vec3 normal_map=normal_mat * (2*textget.xyz-1);
//  vec3 color_normal= texture(tex_2,v_tex).xyz;
//  out_color = vec4(ambient * color_normal + blinn(normalize(normal_map),normalize(v_view), lightDir, color_normal, spec_color, shininess),1.0);

  //couleur diffuse
  //out_color=vec4(color_normal,1);
  //couleur carte de normales
  //out_color= textget;

    // ------------------------------------------------------------ Normal mapping sur une sphere ------------------------------------------------------------

//    vec4 textget = texture(tex_4,v_tex);
//    vec3 normal_map=normal_mat * (2*textget.xyz-1);
//    vec3 color_normal= texture(tex_3,v_tex).xyz;

//    vec3 v_n=normalize(v_normal);
//    vec3 v_t=normalize(v_tangent);
//    vec3 v_b=normalize(v_bitangent);

//    mat3 tbnvMatrix = transpose(mat3(v_t, v_b, v_n));


//    out_color = vec4(ambient * color_normal + blinn(normalize(normal_map),tbnvMatrix * normalize(v_view), tbnvMatrix*lightDir, color_normal, spec_color, shininess),1.0);
//    out_color=vec4(v_t,1);


  // ------------------------------------------------------------ Normal mapping sur Jerry the ogre ------------------------------------------------------------


      vec4 textget = texture(tex_4,v_tex);
      vec3 normal_map=normal_mat * (2*textget.xyz-1);
      vec3 color_normal= texture(tex_3,v_tex).xyz;

      vec3 v_n=normalize(v_normal);
      vec3 v_t=normalize(v_tangent);
      vec3 v_b=normalize(v_bitangent);

      mat3 tbnvMatrix = transpose(mat3(v_t, v_b, v_n));


      out_color = vec4(ambient * color_normal + blinn(normalize(normal_map),tbnvMatrix * normalize(v_view), tbnvMatrix*lightDir, color_normal, spec_color, shininess),1.0);
  //    out_color=vec4(v_t,1);
}
