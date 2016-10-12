#version 430
in vec3 worldPos;
in vec3 worldNormal;
in vec2 tc;
in int out_vertexId;
//layout(early_fragment_tests) in;
layout( location = 0 ) out vec4 FragColor0;

uniform sampler2D posTex;
uniform vec3 objectDiffuseColor;
uniform mat4 modelView; 

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform int hasTex;
uniform int objectId;
uniform float reflectFactor;


void main()
{
	vec3 gBufferPos = texture2D(posTex,tc).xyz;
	if(length(gBufferPos-cameraPos)>length(worldPos-cameraPos))
	{
		discard;
	}
	if(worldPos.x < -26.5745)
	{
		discard;
	}
	float ka = 0.2;
	FragColor0.xyz = vec3(1,0,0);
	
}
