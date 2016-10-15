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
    bvec2 test= greaterThan(texture2D(edgeTex,tc).xy , vec2(0.5f,0.5f));
	bool isEdge = all(test);
		
	
	if(isEdge)
	{
		
		//if(length(texture2D(posTex,tc).xyz-worldPos)<0.25)
			return true ;
		//else
		//	return false;
		
	}
	else 
		return false;
		
}


bool VerticalEdgeTest(vec3 worldPos)
{
	vec4 temp = MVP * vec4(worldPos,1.0);
	vec2 tc = temp.xy/temp.w *0.5+0.5;
	vec2 step = 1.0/resolution;
   	bool c = isVerticalEdge(tc,worldPos);
	bool l = isVerticalEdge(tc-vec2(step.x,0),worldPos);
	bool r = isVerticalEdge(tc+vec2(step.x,0),worldPos);
	bool t = isVerticalEdge(tc+vec2(0,step.y),worldPos);
	bool b = isVerticalEdge(tc-vec2(0,step.y),worldPos);
	//return c||l||r||t||b;

	bool tr = isVerticalEdge(tc+vec2(step.x,step.y),worldPos);
	bool bl = isVerticalEdge(tc-vec2(step.x,step.y),worldPos);
	bool tl = isVerticalEdge(tc+vec2(step.x,-step.y),worldPos);
	bool br = isVerticalEdge(tc+vec2(-step.x,step.y),worldPos);

	return c||l||r||t||b||tr||bl||tl||br;
}

void main()
{	
	
    int isEdgeNum = 0;
	vec3 point1,point2,tex1,tex2;
	for(int i = 0;i<gl_in.length();i++)
	{
		int vIsEdgeI = int(VerticalEdgeTest( worldPos[i]));
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
		if(VerticalEdgeTest(midPoint))
		{
			
			const float farDis = 150;
		  
			tex1 = point1 + farDis* normalize(point1 - eocCameraPos);
			tex2 = point2 + farDis* normalize(point2 - eocCameraPos);
			
			emitVertex(tex1);
			emitVertex(tex2);
			emitVertex(point1);
			emitVertex(point2);
		}
			/*
			emitVertex(worldPos[0]);
			emitVertex(worldPos[2]);
			emitVertex(worldPos[1]);
			emitVertex(eocCameraPos);
			*/
		
	}
	EndPrimitive();
	

}