#version 430

in vec3 worldPos;
in vec3 worldNormal;
in vec2 tc;

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
vec2 fistDepth(float c,float  l,float r)
{
	vec2 result;
	if (abs(l+r-2*c)<FISTDIS)
		result.x = 0;
	else
		result.x = 1;
	if(c+FISTDIS<r)
		result.y = 1;
	else
		result.y = 0;
	return result;
}
vec4 firstDisconect(vec2 tc,vec3 worldPos)
{
	vec2 step = 1.0/resolution;
    vec4 result;
	float c = getDepthRep(worldPos);
	float l = getDepthTx(tc-vec2(step.x,0));
	float r = getDepthTx(tc+vec2(step.x,0));
	float t = getDepthTx(tc+vec2(0,step.y));
	float b = getDepthTx(tc-vec2(0,step.y));
	result.xy = fistDepth(c,l,r);
	result.zw = fistDepth(c,t,b);
	return result;
}


void main()
{
	vec2 ndc = (gl_FragCoord.xy)/resolution;
	vec3 orginPos = texture2D(posTex,ndc).xyz;
	

	vec3 norm   = normalize( (mat3(modeViewInv)* worldNormal));
	if(abs(norm.z)>0.6)
		discard;

	if(length(orginPos-cameraPos)-length(worldPos-cameraPos)>0.004)
		;
	else
		discard;
		
		
	if(length(orginPos-worldPos)>8.004)
		;
	else
		discard;
		
	color0.xyzw = firstDisconect(ndc,worldPos);
	color0.x = 1;
	color0.z = 1;
}