#version 460

// This lets us write to our 2 color buffers.
// For the bloom effect, we put the blurred parts over the main color image
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;
layout (location = 2) out float shouldGammaCorrect;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float exponential;
	float linear;
	float constant;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform bool flashLightOn;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform sampler2D theTexture;
uniform Material material;

uniform vec3 eyePosition;

vec4 CalcLightByDirection(Light light, vec3 direction)
{
	vec4 ambientColor = vec4(light.color, 1.0f) * light.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(normal), -normalize(direction)), 0.0f);
	vec4 diffuseColor = vec4(light.color * light.diffuseIntensity * diffuseFactor, 1.0f);
	
	vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if(diffuseFactor > 0.0f)
	{
		vec3 fragToEye = normalize(eyePosition - fragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(normal)));
		
		float specularFactor = dot(fragToEye, reflectedVertex);
		if(specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	return (ambientColor + diffuseColor + specularColor);
}

vec4 CalcDirectionalLight()
{
	return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLight(PointLight pLight)
{
    vec3 direction = fragPos - pLight.position;
    float distance = length(direction);
    direction = normalize(direction);
    
    vec4 color = CalcLightByDirection(pLight.base, direction);
    float attenuation = pLight.exponential * distance * distance +
                        pLight.linear * distance +
                        pLight.constant;
    
    return color / attenuation;
}

vec4 CalcSpotLight(SpotLight sLight)
{
    vec3 rayDirection = normalize(fragPos - sLight.base.position);
    float slFactor = dot(rayDirection, sLight.direction);

	if (slFactor <= sLight.edge)
		return vec4(0, 0, 0, 0);

	vec4 color = CalcPointLight(sLight.base);
    return color * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));
}

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0; i < pointLightCount; i++)
		totalColor += CalcPointLight(pointLights[i]);
	
	return totalColor;
}

void main()
{
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
    if (flashLightOn)
        finalColor += CalcSpotLight(spotLight);
	
	fragColor = texture(theTexture, texCoord) * finalColor;

    //float brightness = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    bloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shouldGammaCorrect = 0.0;
}
