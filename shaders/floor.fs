#version 150

#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D U_Texture;
in vec4 V_Color;
in vec2 V_Texcoord;
out vec4 color;
void main()
{
	color=V_Color*texture(U_Texture,V_Texcoord);
}