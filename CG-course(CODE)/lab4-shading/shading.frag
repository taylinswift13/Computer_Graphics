#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

///////////////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////////////
uniform vec3 material_color;
uniform float material_metalness;
uniform float material_fresnel;
uniform float material_shininess;
uniform vec3 material_emission;

uniform int has_color_texture;
layout(binding = 0) uniform sampler2D colorMap;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
uniform float environment_multiplier;

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
uniform vec3 point_light_color = vec3(1.0, 1.0, 1.0);
uniform float point_light_intensity_multiplier = 50.0;

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359

///////////////////////////////////////////////////////////////////////////////
// Input varyings from vertex shader
///////////////////////////////////////////////////////////////////////////////
in vec2 texCoord;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 viewInverse;
uniform vec3 viewSpaceLightPosition;

///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 fragmentColor;


vec3 calculateDirectIllumiunation(vec3 wo, vec3 n, vec3 base_color)
{
	vec3 direct_illum = base_color;
	///////////////////////////////////////////////////////////////////////////
	// Task 1.2 - Calculate the radiance Li from the light, and the direction
	//            to the light. If the light is backfacing the triangle,
	//            return vec3(0);
	///////////////////////////////////////////////////////////////////////////
	vec3 Li = point_light_intensity_multiplier * point_light_color/pow(length(viewSpaceLightPosition - viewSpacePosition),2);
	vec3 wi= normalize(viewSpaceLightPosition - viewSpacePosition);
	if(dot(n, wi) <= 0) return vec3(0);
	///////////////////////////////////////////////////////////////////////////
	// Task 1.3 - Calculate the diffuse term and return that as the result
	///////////////////////////////////////////////////////////////////////////
	vec3 diffuse_term = base_color /PI * dot(n,wi) * Li;

	///////////////////////////////////////////////////////////////////////////
	// Task 2 - Calculate the Torrance Sparrow BRDF and return the light
	//          reflected from that instead
	///////////////////////////////////////////////////////////////////////////
		//incoming light direction(to fragmnet)
	
	//half vector of incoming&outgoing light
	vec3 wh= normalize(wi + wo);
	//the fresnel term
	float F = material_fresnel + (1 - material_fresnel) * pow( 1 - dot(wh,wi), 5);
	// the Microfacet Distribution Function
	float D = (material_shininess + 2) *  pow(dot(n,wh), material_shininess) / (2* PI);
	// the shadowing/masking function
	float part1 = 2*dot(n,wh) * dot(n,wo) /dot(wh , wo);
	float part2 = 2*dot(n,wh) * dot(n,wi) /dot(wh , wo);
	float G = min(1, min(part1, part2));

	float BRDF = F*D*G/(4*dot(n,wo)*dot(n,wi));
	///////////////////////////////////////////////////////////////////////////
	// Task 3 - Make your shader respect the parameters of our material model.
	///////////////////////////////////////////////////////////////////////////
	vec3 dielectric_term = BRDF * dot(n, wi) * Li + (1 - F) * diffuse_term;
	vec3 metal_term = BRDF * base_color * dot(n,wi) *Li;
	direct_illum = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;
	return direct_illum;
	//return diffuse_term;
	//return BRDF * dot(n, wi) * Li;
}

vec3 calculateIndirectIllumination(vec3 wo, vec3 n, vec3 base_color)
{
	vec3 indirect_illum = vec3(0.f);
	///////////////////////////////////////////////////////////////////////////
	// Task 5 - Lookup the irradiance from the irradiance map and calculate
	//          the diffuse reflection
	///////////////////////////////////////////////////////////////////////////
	vec3 nws = normalize((viewInverse * vec4(n , 0)).xyz);
    float theta = acos(max(-1.0f, min(1.0f, nws.y)));
    float phi = atan(nws.z, nws.x);
    if(phi < 0.0f)
    {
    	phi = phi + 2.0f * PI;
    }
	//spherical coodinates of world space normal
    vec2 lookup_1 = vec2(phi / (2.0 * PI), 1 - theta / PI);

    vec3 irradiance = (environment_multiplier * texture(irradianceMap, lookup_1)).rgb ;
    vec3 diffuse_term = base_color * (1.0 / PI) * irradiance;
    
    ///////////////////////////////////////////////////////////////////////////
    // Task 6 - Look up in the reflection map from the perfect specular
    //          direction and calculate the dielectric and metal terms.
    ///////////////////////////////////////////////////////////////////////////
   float roughness=sqrt(sqrt(2/(material_shininess+2)));

	vec3 wows = normalize((viewInverse * vec4(wo , 0)).xyz);
	vec3 wiws= normalize(reflect(-wows,nws).xyz);

	//spherical coodinates of world space wo reflection 
	 float theta_2 = acos(max(-1.0f, min(1.0f, wiws.y)));
    float phi_2 = atan(wiws.z, wiws.x);
    if(phi_2 < 0.0f)
    {
    	phi_2 = phi_2 + 2.0f * PI;
    }
	//spherical coodinates of world space normal
    vec2 lookup_2 = vec2(phi_2 / (2.0 * PI), 1 - theta_2 / PI);

	 vec3 Li = environment_multiplier * textureLod(reflectionMap, lookup_2, roughness*7).rgb;
	 vec3 wi= normalize(viewSpaceLightPosition - viewSpacePosition);
	vec3 wh = normalize(wi + wo);
	vec3 whws = normalize(wiws + wows);
	//the fresnel term
	float F = material_fresnel + (1 - material_fresnel ) * pow( 1 - dot(whws,wiws), 5);
    vec3 dielectric_term = F * Li + ( 1- F ) * diffuse_term;
    vec3 metal_term =F * base_color * Li;
	indirect_illum = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;
	return indirect_illum;
}


void main()
{
	///////////////////////////////////////////////////////////////////////////
	// Task 1.1 - Fill in the outgoing direction, wo, and the normal, n. Both
	//            shall be normalized vectors in view-space.
	///////////////////////////////////////////////////////////////////////////
	vec3 wo = normalize(-viewSpacePosition);
	vec3 n = normalize(viewSpaceNormal);

	vec3 base_color = material_color;
	if(has_color_texture == 1)
	{
		base_color *= texture(colorMap, texCoord).rgb;
	}

	vec3 direct_illumination_term = vec3(0.0);
	{ // Direct illumination
		direct_illumination_term = calculateDirectIllumiunation(wo, n, base_color);
	}

	vec3 indirect_illumination_term = vec3(0.0);
	{ // Indirect illumination
		indirect_illumination_term = calculateIndirectIllumination(wo, n, base_color);
	}

	///////////////////////////////////////////////////////////////////////////
	// Task 1.4 - Make glowy things glow!
	///////////////////////////////////////////////////////////////////////////
	vec3 emission_term = material_emission;

	vec3 final_color = direct_illumination_term + indirect_illumination_term + emission_term;

	// Check if we got invalid results in the operations
	if(any(isnan(final_color)))
	{
		final_color.rgb = vec3(1.f, 0.f, 1.f);
	}

	fragmentColor.rgb = final_color;
}
