
#include "heightfield.h"
#include "labhelper.h"

#include <iostream>
#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>
#include <stb_image.h>

using namespace glm;
using std::string;

HeightField::HeightField(void)
	: m_meshResolution(1000)
	, m_vao(UINT32_MAX)
	, m_positionBuffer(UINT32_MAX)
	, m_uvBuffer(UINT32_MAX)
	, m_indexBuffer(UINT32_MAX)
	, m_numIndices(0)
	, m_texid_hf(UINT32_MAX)
	, m_texid_diffuse(UINT32_MAX)
	, m_texid_shine(UINT32_MAX)
	, m_heightFieldPath("")
	, m_diffuseTexturePath("")
{
}

void HeightField::loadHeightField(const std::string& heigtFieldPath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	float* data = stbi_loadf(heigtFieldPath.c_str(), &width, &height, &components, 1);
	if (data == nullptr)
	{
		std::cout << "Failed to load image: " << heigtFieldPath << ".\n";
		return;
	}

	if (m_texid_hf == UINT32_MAX)
	{
		glGenTextures(1, &m_texid_hf);
	}
	glBindTexture(GL_TEXTURE_2D, m_texid_hf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data); // just one component (float)

	m_heightFieldPath = heigtFieldPath;
	std::cout << "Successfully loaded heigh field texture: " << heigtFieldPath << ".\n";
}

void HeightField::loadDiffuseTexture(const std::string& diffusePath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	uint8_t* data = stbi_load(diffusePath.c_str(), &width, &height, &components, 3);
	if (data == nullptr)
	{
		std::cout << "Failed to load image: " << diffusePath << ".\n";
		return;
	}

	if (m_texid_diffuse == UINT32_MAX)
	{
		glGenTextures(1, &m_texid_diffuse);
	}

	glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Successfully loaded diffuse texture: " << diffusePath << ".\n";
}

void HeightField::loadShininessTexture(const std::string& shinePath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	uint8_t* data = stbi_load(shinePath.c_str(), &width, &height, &components, 3);
	if (data == nullptr)
	{
		std::cout << "Failed to load image: " << shinePath << ".\n";
		return;
	}

	if (m_texid_shine == UINT32_MAX)
	{
		glGenTextures(1, &m_texid_shine);
	}

	glBindTexture(GL_TEXTURE_2D, m_texid_shine);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Successfully loaded diffuse texture: " << shinePath << ".\n";
}


void HeightField::generateMesh(int tesselation)
{
	// generate a mesh in range -1 to 1 in x and z
	// (y is 0 but will be altered in height field vertex shader)
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	std::vector<vec3> vertices;
	for (int j = 0; j <= tesselation; ++j)
	{
		for (int i = 0; i <= tesselation; ++i)
		{
			float x = 0, z = 0;
			x = (-1.0f + (float)i / (float)tesselation) * 1000;
			z = (-1.0f + (float)j / (float)tesselation) * 1000;
			vertices.push_back(vec3(x, 0, z));
		}
	}

	// Create a handle for the vertex position buffer
	glGenBuffers(1, &m_positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, (int32)vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false /*normalized*/, 0 /*stride*/, 0 /*offset*/);
	glEnableVertexAttribArray(0);

	std::vector<vec2> textureCoord;

	for (int j = 0; j <= tesselation; ++j)
	{
		for (int i = 0; i <= tesselation; ++i)
		{
			float x = 0, y = 0;
			x = (float)i / (float)tesselation;
			y = (float)j / (float)tesselation;

			textureCoord.push_back(vec2(x, y));
		}
	}
	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, (int32)textureCoord.size() * sizeof(vec2), &textureCoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < (unsigned int)vertices.size() - tesselation - 1; i++) {
		if ((i + 1) % (tesselation + 1) == 0) {
			continue;
		}
		indices.push_back(i);
		indices.push_back(i + tesselation + 1);
		indices.push_back(i + 1);

		indices.push_back(i + 1);
		indices.push_back(i + 1 + tesselation);
		indices.push_back(i + 2 + tesselation);
	}
	m_numIndices = static_cast<int>(indices.size());
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int32)indices.size() * sizeof(int), &indices[0],
		GL_STATIC_DRAW);
}

void HeightField::submitTriangles(void)
{
	if (m_vao == UINT32_MAX)
	{
		std::cout << "No vertex array is generated, cannot draw anything.\n";
		return;
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texid_hf);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_texid_shine);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
}

