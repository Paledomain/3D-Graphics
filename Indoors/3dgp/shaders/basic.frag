// FRAGMENT SHADER
#version 330

in vec4 color;
out vec4 outColor;

in vec4 position;
in vec3 normal;

in vec2 texCoord0;


// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

//sampler
uniform sampler2D texture0;

//matrices
uniform mat4 matrixView;

struct POINT
{
	int on;
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	mat4 matrix;
};
uniform POINT lightPoint1, lightPoint2;

vec4 PointLight(POINT light)
{
	// Calculate Point Light
	vec4 color = vec4(0, 0, 0, 0);
	
	vec4 L = normalize((light.matrix * vec4(light.position, 1)) - position);
	vec3 L3 = L.xyz;

	float NdotL = dot(normal, L3);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	//specular lighting
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L3, normal);
	float RdotV = dot(R, V);//intensity
	if (NdotL > 0 && RdotV > 0)
	    color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	
	return color;

}

void main(void) 
{
	outColor = color;

	if (lightPoint1.on == 1) 
		outColor += PointLight(lightPoint1);

	if (lightPoint2.on == 1) 
		outColor += PointLight(lightPoint2);
	
	outColor *= texture(texture0, texCoord0);
}
 
