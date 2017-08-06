#ifdef _WIN32
	#include <glew.h>
#endif
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include <iostream>
#include "Helper.h"

using namespace glm;

namespace mm
{
	CubeObject::~CubeObject()
	{
	}

	unsigned int CubeObject::createBaseShape()
	{
		_baseNormal = new std::vector<float>(108, 0.f);

		_baseVertices = new std::vector<float>{
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f
		};

		_baseIndices = new std::vector<unsigned int>;
		for (unsigned int i = 0; i < 36; ++i)
		{
			_baseIndices->push_back(i);
			if ((i + 1) % 3 == 0)
			{
			std::cout << createTriangle(i - 2, i - 1, i).vertex[0] << std::endl;
			}
		}

		// vertex array object
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// vertex buffer object
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, _baseVertices->size()*sizeof(float), _baseVertices->data(), GL_STATIC_DRAW);

		// index buffer object
		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _baseIndices->size()*sizeof(int), _baseIndices->data(), GL_STATIC_DRAW);

		// vertex attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		return _vao;
	}

	void CubeObject::drawObj()
	{
		glBindVertexArray(_vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	Triangle CubeObject::createTriangle(int i0, int i1, int i2)
	{
		Triangle t;
		// get indcies
		t.vertex[0] = i0;
		t.vertex[1] = i1;
		t.vertex[2] = i2;
		t.normal[0] = i0;
		t.normal[1] = i1;
		t.normal[2] = i2;
		// get coordinates for 3 vertices
		vec3 v1 = vec3((*_baseVertices)[i0 * 3], (*_baseVertices)[i0 * 3 + 1], (*_baseVertices)[i0 * 3 + 2]);
		vec3 v2 = vec3((*_baseVertices)[i1 * 3], (*_baseVertices)[i1 * 3 + 1], (*_baseVertices)[i1 * 3 + 2]);
		vec3 v3 = vec3((*_baseVertices)[i2 * 3], (*_baseVertices)[i2 * 3 + 1], (*_baseVertices)[i2 * 3 + 2]);
		
		// calculate face normal
		vec3 nor = cross(vec3(v2 - v1), vec3(v3 - v1));
		nor = normalize(nor);
		for (int i = i0; i < (i0 + 9); i+=3)
		{
			(*_baseNormal)[i] = nor.x;
			(*_baseNormal)[i+1] = nor.y;
			(*_baseNormal)[i+2] = nor.z;
		}

		_triList.push_back(t);
		return t;
	}
}