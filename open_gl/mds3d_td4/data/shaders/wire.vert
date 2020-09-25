#version 330 core


in vec3 vtx_position;
in vec3 vtx_color;
in vec3 vtx_normal;

uniform mat4 obj_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;


out vec3 var_color;


void main()
{

  gl_Position = proj_mat * view_mat * obj_mat * vec4(vtx_position.xyz,1.);
  var_color=vec3(1,1,1);

}
