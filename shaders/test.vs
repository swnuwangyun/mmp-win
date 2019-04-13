#version 150

in vec4 position;
in vec4 color;
in vec2 texcoord;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
out vec4 V_Color;
out vec2 V_Texcoord;
void main()
{
	V_Color=color;
	V_Texcoord=texcoord;
	gl_Position=ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}