// VERTEX SHADER
#version 330

// Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

//Fog
uniform float fogDensity;

// Lights
struct AMBIENT
{	int on;
	vec3 color;
};
struct DIRECTIONAL
{	int on;
	vec3 direction;
	vec3 diffuse;
};

struct POINT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	mat4 matrix;
};

uniform AMBIENT lightAmbient;
uniform DIRECTIONAL lightDir;
uniform POINT lightPoint1;

layout (location = 0) in vec3 aVertex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;


// Output (sent to Fragment Shader)
out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;
out float fogFactor;

vec4 compAmbient(vec3 material, AMBIENT light)
{
	return vec4(material * light.color, 1);
}

vec4 compDirectional(vec3 material, DIRECTIONAL light)
{
	vec3 L = normalize(mat3(matrixView) * light.direction).xyz;
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		return vec4(light.diffuse * material * NdotL, 1);
	else
		return vec4(0, 0, 0, 1);
}

void main(void) 
{


	// calculate position & normal
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;
	
	//calculate the fogFactor  in the following way:
	fogFactor = exp2(-fogDensity * length(position));
	
	// calculate the colour
	color = vec4(0, 0, 0, 1);
	normal = normalize(mat3(matrixModelView) * aNormal);
	
	// ambient & emissive light
	if (lightAmbient.on == 1)
		color += compAmbient(materialAmbient, lightAmbient);

	// directional lights
	if (lightDir.on == 1)
		color += compDirectional(materialDiffuse, lightDir);

	// calculate texture coordinate
	texCoord0 = aTexCoord;

}
