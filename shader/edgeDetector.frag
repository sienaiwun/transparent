#version 430

layout(location = 0) in vec2 tc;

uniform sampler2D colorTex;

uniform sampler2D normalTex;
uniform sampler2D posTex;


uniform mat4 MVP; // Projection * ModelView
uniform mat4 mvpInv;
uniform mat4 modeView;
uniform mat4 modeViewInv;

uniform vec2 resolution;
uniform vec3 cameraPos;
layout (location =  0) out vec4 color0;  //pos



float getDepthRep(vec3 worldPos)
{
	vec4 tmp = modeView*vec4(worldPos,1);
	return 1.0/tmp.z;
}
float getDepthTx(vec2 tc)
{
   return getDepthRep(texture2D(posTex,tc).xyz);
}

#define FISTDIS 0.0005
float fistDepth(float c,float  l,float r)
{
	if (abs(l+r-2*c)<FISTDIS)
		return 0;
	else
		return 1;
}
vec2 firstDisconect(vec2 tc)
{
vec2 step = 1.0/resolution;
    vec2 result;
	float c = getDepthTx(tc);
	float l = getDepthTx(tc-vec2(step.x,0));
	float r = getDepthTx(tc+vec2(step.x,0));
	float t = getDepthTx(tc+vec2(0,step.y));
	float b = getDepthTx(tc-vec2(0,step.y));
	result.x = fistDepth(c,l,r);
	result.y = fistDepth(c,t,b);
	return result;
}


void main()
{
		vec2 result = firstDisconect(tc);
		color0.xyzw = vec4( result,0,1);
}