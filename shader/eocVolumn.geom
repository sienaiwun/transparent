#version 430

layout (triangles) in ;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 MVP; // Projection * ModelView
uniform vec3 eocCameraPos;

in vec3 worldPos[];

out vec3 pWorldPos;


uniform sampler2D edgeTex;
uniform sampler2D posTex;
uniform vec2 resolution;


void emitVertex(vec3 pos)
{
	gl_Position =  MVP * vec4(pos,1.0);
	pWorldPos  = pos;
	EmitVertex();
}


bool isVerticalEdge(vec2 tc, vec3 worldPos)
{
  	
	
	if( all( greaterThan(texture2D(edgeTex,tc).xy , vec2(0.5f,0.5f))))
	{
		
		//if(length(texture2D(posTex,tc).xyz-worldPos)<0.25)
			return true ;
		//else
		//	return false;
		
	}
	else 
		return false;
		
}


bool VerticalEdgeTest(vec3 worldPos,float length)
{
	vec4 temp = MVP * vec4(worldPos,1.0);
	vec2 tc = temp.xy/temp.w *0.5+0.5;
	vec2 step = 1.0/resolution;
	for (float x = -length;x<=length;x++)
	{
		for (float y = -length;y<=length;y++)
		{
			 if(isVerticalEdge(tc+vec2(step.x*x,step.y*y),worldPos))
				return true;
		}
	}
	return false;
}

void main()
{	
	
    int isEdgeNum = 0;
	vec3 point1,point2,tex1,tex2;
	for(int i = 0;i<gl_in.length();i++)
	{
		int vIsEdgeI = int(VerticalEdgeTest( worldPos[i],1.5));
		isEdgeNum +=vIsEdgeI;
		
		if((isEdgeNum==1) && (vIsEdgeI == 1))
		{
			point1 =  worldPos[i];
		}
		else if((isEdgeNum==2 )&& (vIsEdgeI == 1))
		{
			point2 =  worldPos[i];
		}
	}
	if(isEdgeNum==2)
	{
		vec3 midPoint = (point1+point2)/2;
		if(VerticalEdgeTest(midPoint,2))
		{
			
			const float farDis = 150;
		  
			tex1 = point1 + farDis* normalize(point1 - eocCameraPos);
			tex2 = point2 + farDis* normalize(point2 - eocCameraPos);
			
			emitVertex(tex1);
			emitVertex(tex2);
			emitVertex(point1);
			emitVertex(point2);
		}
		
	}
	EndPrimitive();
	

}