#version 430
in vec3 worldPos;
in vec3 worldNormal;
in vec2 tc;
in int out_vertexId;

layout( location = 0 ) out vec4 FragColor0;
layout( location = 1 ) out vec4 FragColor1;
layout( location = 2 ) out vec4 FragColor2;
layout( location = 3 ) out vec4 FragColor3;

uniform sampler2D objectTex;
uniform vec3 objectDiffuseColor;
uniform mat4 modelView; // Projection * ModelView

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform int hasTex;
uniform int objectId;
uniform float reflectFactor;
void main()
{
	float reflectValue = reflectFactor;
	vec3 N = normalize(worldNormal);
	FragColor1 = vec4(N,reflectValue);
	vec3 L = normalize(lightPos-worldPos);
	float kd=max(dot(N,L),0);
	vec3 diffuse;
	if(bool(hasTex))
	{
		diffuse = texture2D(objectTex,tc).xyz;
	}
	else
	{
		diffuse = objectDiffuseColor;
	}
	diffuse = texture2D(objectTex,tc).xyz;
	float ka = 0.2;
	FragColor0.xyz = diffuse*(kd+ka)+vec3(0.2,0.2,0.2);
	
}
