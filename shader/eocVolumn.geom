#version 430

layout (triangles) in ;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 MVP; // Projection * ModelView
uniform vec3 eocCameraPos;

in vec3 worldPos[];
noperspective in vec2 vtc[];
flat in int vIsEdge[];

out vec3 pWorldPos;
void emitVertex(vec3 pos)
{
	gl_Position =  MVP * vec4(pos,1.0);
	pWorldPos  = pos;
	EmitVertex();
}
void main()
{	
	
    int isEdgeNum = 0;
	vec3 point1,point2,tex1,tex2;
	for(int i = 0;i<gl_in.length();i++)
	{
		isEdgeNum +=vIsEdge[i];
		if((isEdgeNum==1) && (vIsEdge[i] == 1))
		{
			point1 =  worldPos[i];
		}
		else if((isEdgeNum==2 )&& (vIsEdge[i] == 1))
		{
			point2 =  worldPos[i];
		}
	}
	if(isEdgeNum<2.5&&isEdgeNum>1.5)
	{
		
			
			const float farDis = 150;
		  
			tex1 = point1 + farDis* normalize(point1 - eocCameraPos);
			tex2 = point2 + farDis* normalize(point2 - eocCameraPos);
			
			emitVertex(tex1);
			emitVertex(tex2);
			emitVertex(point1);
			emitVertex(point2);
			/*
			emitVertex(worldPos[0]);
			emitVertex(worldPos[2]);
			emitVertex(worldPos[1]);
			emitVertex(eocCameraPos);
			*/
		
	}
	EndPrimitive();
	

}