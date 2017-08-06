#pragma once

#include <iostream>
#include <vector>

namespace mm {
	struct Vec3f
	{
		float x, y, z;
	};

	struct Vertex
	{
		Vec3f pos;
		Vec3f normal;
	};

	// Triangle struct
	struct Triangle
	{
		int vertex[3];
		int normal[3];
		int adjTri[3];
	};

	class CubeObject
	{
	public:
		CubeObject() {};
		~CubeObject();
		unsigned int createBaseShape();
		void drawObj();

	private:
		Triangle createTriangle( int index1, int index2, int index3 );

		std::vector<float>			*_baseVertices;
		std::vector<float>			*_baseNormal;
		std::vector<Triangle>		_triList;
		std::vector<unsigned int>	*_baseIndices;
		unsigned int	_vao, _vbo, _ebo;
	};
}