#version 330 core

in vec3 vtx_position;
in vec3 vtx_color;
in vec3 vtx_normal;
out vec3 var_color;

uniform mat4 obj_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat3 normal_mat;
uniform vec3 light_pos;

out vec3 light_dir;
out vec3 view_pos;
out vec3 normal;
out vec3 color;

void main()
{

  gl_Position = proj_mat * view_mat * obj_mat * vec4(vtx_position.xyz,1.);



  vec4 viewPos=view_mat * obj_mat * vec4(vtx_position,1);
  vec3 norm= normalize(normal_mat * vtx_normal);
  vec3 lightDir = light_pos - viewPos.xyz;
  vec4 light4=view_mat * vec4(lightDir,1);
  lightDir= normalize(light4.xyz);


  light_dir=lightDir;
  view_pos=viewPos.xyz;
  normal=norm;
  color=vtx_color;



}

