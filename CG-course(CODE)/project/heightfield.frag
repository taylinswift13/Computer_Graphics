#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;


in vec2 texCoord;
out vec4 fragmentColor;
layout(binding = 1) uniform sampler2D m_texid_diffuse;

uniform vec3 material_color = vec3(1, 1, 1);
uniform float material_metalness = 0;
uniform float material_fresnel = 0;
uniform float material_shininess = 0;
uniform vec3 material_emission = vec3(0);

uniform int has_color_texture = 0;
layout(binding = 0) uniform sampler2D colorMap;
uniform int has_emission_texture = 0;
layout(binding = 5) uniform sampler2D emissiveMap;

layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
uniform float environment_multiplier;

uniform vec3 point_light_color = vec3(1.0, 1.0, 1.0);
uniform float point_light_intensity_multiplier = 50.0;

#define PI 3.14159265359

// This simple fragment shader is meant to be used for debug purposes
// When the geometry is ok, we will migrate to use shading.frag instead.

void main()
{
	fragmentColor=vec4(1.0);
	fragmentColor = texture2D(m_texid_diffuse, texCoord.xy);
}
