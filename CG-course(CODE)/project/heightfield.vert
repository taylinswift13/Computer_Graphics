#version 420
///////////////////////////////////////////////////////////////////////////////
// Input vertex attributes
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoordIn;
layout(location = 2) in vec2 texCoordUp;
layout(location = 3) in vec2 texCoordRight;
layout(location = 4) in vec2 texCoordDown;
layout(location = 5) in vec2 texCoordLeft;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////

uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
layout(binding = 1) uniform sampler2D m_texid_hf;
uniform int meshResolution;
vec3 normalIn;


///////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
///////////////////////////////////////////////////////////////////////////////
out vec2 texCoord;
out vec3 positionOut;
out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;
out vec3 normalOut;

void main()
{
	//float height = dot(texture2D(m_texid_hf, texCoordIn.xy).rgb, vec3(0.299, 0.587, 0.114));
	float scale = meshResolution/10.0f;
	float height = texture2D(m_texid_hf, texCoordIn).r * scale;
	vec3 position = vec3(position.x, height,position.z);
	viewSpacePosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
	
	float meshResolutionf = meshResolution * 1.0f;
	float offset =  6.0f;
	float heightUp =  texture2D(m_texid_hf,texCoordIn + vec2(0,offset)).r * scale;
	float heightRight =  texture2D(m_texid_hf,texCoordIn + vec2(offset,0)).r * scale;
	float heightDown =  texture2D(m_texid_hf,texCoordIn + vec2(0,-offset)).r * scale;
	float heightLeft =  texture2D(m_texid_hf,texCoordIn + vec2(-offset,0)).r * scale;

	vec3 vertex = vec3(position.x, height, position.z);
	vec3 vertexUp = vec3(position.x, heightUp, position.z + offset);
	vec3 vertexRight = vec3(position.x + offset, heightRight, position.z);
	vec3 deltaVectorUp = vec3(0, heightUp - height, offset);
	vec3 deltaVectorRight = vec3(offset, heightRight - height, 0);

	//normalIn = normalize(cross(deltaVectorUp, deltaVectorRight));
	//normalOut = normalIn;
	positionOut = position;

	//Try 1
	//normalIn = normalize(vec3( heightLeft-heightRight, 2.0f, heightUp-heightDown));

	//Try2
    //const vec2 size = vec2(2.0,0.0);
	//const ivec3 off = ivec3(-1,0,1);
    //vec4 wave = texture(m_texid_hf, texCoordIn);
    //float s11 = wave.x;
    //float s01 = textureOffset(m_texid_hf, texCoordIn, off.xy).x;
    //float s21 = textureOffset(m_texid_hf, texCoordIn, off.zy).x;
    //float s10 = textureOffset(m_texid_hf, texCoordIn, off.yx).x;
    //float s12 = textureOffset(m_texid_hf, texCoordIn, off.yz).x;
	//vec3 va = normalize(vec3(size.x, s21-s01, size.y));      
	//vec3 vb = normalize(vec3(size.y, s12-s10, -size.x));
    //vec4 bump = normalize(vec4( cross(va,vb), s11 ));
	//normalIn  = bump.xyz;

	//Try3
	//normalIn = normalize(vec3(-(heightUp-height),offset,-(heightRight-height)));

	//Try4
	
	//normalIn *= sign(dot(normalIn, vertex+vertexRight+vertexUp));


	//Try5

	//viewSpaceNormal = (normalMatrix * vec4(normalIn, 0.0)).xyz;
	texCoord = texCoordIn;
}
