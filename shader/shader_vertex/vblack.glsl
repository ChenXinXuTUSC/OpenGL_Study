#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool x_p;
uniform bool y_p;
uniform bool z_p;

void main()
{
    vec4 pos1 = model * vec4(aPos, 1.0);
    if (x_p)
	    gl_Position = projection * view * vec4(0.01, pos1.y, pos1.z, 1.0);
    if (y_p)
	    gl_Position = projection * view * vec4(pos1.x, 0.01, pos1.z, 1.0);
    if (z_p)
	    gl_Position = projection * view * vec4(pos1.x, pos1.y, 0.01, 1.0);
}