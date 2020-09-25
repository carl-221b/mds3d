#version 330 core

in vec3 vtx_position;
in vec3 vtx_color;
out vec3 var_color;
uniform float zoom;
uniform vec2 offset;
uniform int fil;

void main()
{
  vec3 v=vec3(-vtx_position.z,vtx_position.y,vtx_position.x);
  gl_Position = vec4((v.xy+offset)*zoom,v.z, 1.);
  if(fil==1)
    var_color=vec3(1,1,1);
  else
    var_color=vtx_color;

}
