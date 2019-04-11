// FRAGMENT SHADER
#version 330


in vec4 color;
in vec2 texCoord0;

out vec4 outColor;

in vec4 position;
in vec3 normal;

in float fogFactor;

// SAMPLER
uniform sampler2D texture0;

// ANIMATOR
uniform float time;
uniform float speedX;
uniform float speedY;

//ATTENUATION PARTS
uniform float att_const;
uniform float att_linear;
uniform float att_quadratic;

// MATERIALS
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

// MATRICES
uniform mat4 matrixView;

//FOG
uniform vec3 fogColour;



struct POINT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	mat4 matrix;
};
uniform int lightPointOn;
uniform POINT lightPoint1, lightPoint2, lightPoint3, lightPoint4, lightPoint5, lightPoint6, lightPoint7, lightPoint8, lightPoint9, lightPoint10, lightPoint11, lightPoint12;

vec4 PointLight(POINT light)
{
	// Calculate Point Light
	vec4 color = vec4(0, 0, 0, 0);
	
	//calculate distance from point light
	float dist = length(matrixView * vec4(light.position, 1) - position);

	float att = 1 / (att_const + att_linear * dist + att_quadratic * dist * dist);

	vec4 L = normalize((light.matrix * vec4(light.position, 1)) - position);
	vec3 L3 = L.xyz;

	float NdotL = dot(normal, L3);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	//specular lighting
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L3, normal);
	float RdotV = dot(R, V);//intensity
//	if (NdotL > 0 && RdotV > 0)
//	    color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	
	return color * att;

}

void main(void) 
{

	float xScrollValue = speedX * time;
	float yScrollValue = speedY * time;

	vec2 animator = vec2(xScrollValue, yScrollValue);

	outColor = color;

	if (lightPointOn == 1) 
	{
		outColor += PointLight(lightPoint1);
		outColor += PointLight(lightPoint2);
		outColor += PointLight(lightPoint3);
		outColor += PointLight(lightPoint4);
		outColor += PointLight(lightPoint5);
		outColor += PointLight(lightPoint6);
		outColor += PointLight(lightPoint7);
		outColor += PointLight(lightPoint8);
		outColor += PointLight(lightPoint9);
		outColor += PointLight(lightPoint10);
		outColor += PointLight(lightPoint11);
		outColor += PointLight(lightPoint12);
	}

	outColor *= texture(texture0, texCoord0 + animator);
	outColor = mix(vec4(fogColour, 1), outColor, fogFactor);

}
