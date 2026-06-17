#version 330 core

out vec4 out_frag_color;

uniform vec4 color;

void main()
{
	out_frag_color = color;
}