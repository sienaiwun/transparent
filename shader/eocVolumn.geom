#version 430

layout (triangles) in ;
layout (triangle_strip, max_vertices = 6) out;

in vec3 worldPos[];
in vec3 worldNormal[];
in vec2 tc[];


out vec3 worldPos_g;
out vec3 worldNormal_g;
out vec2 tc_g;
void main()
{	
	
	for(int i = 0;i<gl_in.length();i++)
	{
		gl_Position = gl_in[i].gl_Position;
		worldPos_g = worldPos[i];
		worldNormal_g = worldNormal[i];
		tc_g = tc[i];
		EmitVertex();
	}
	EndPrimitive();
	

}