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
		int oppVert[3];
		int adjTri[3];
	};

	class CubeObject
	{
	public:
		CubeObject() {};
		~CubeObject();
		unsigned int createBaseShape();
		void findAdjacentTriangle();
		void subdivide();
		void drawBaseObj();
		void drawSubdividedObj();

	private:
		Triangle _createTriangle( int index1, int index2, int index3 );
		int _isShareEdge( int v1, int v2, Triangle tri2,int &in ) const;
		void _split(Triangle tri, int triIndex);

		std::vector<float>			_baseVertices;
		std::vector<float>			_currentVertices;
		std::vector<float>			*_baseNormal;
		std::vector<Triangle>		_triList;
		std::vector<Triangle>		_tempTriList;
		std::vector<unsigned int>	_baseIndices;
		std::vector<unsigned int>	_currentIndices;
		unsigned int	_subVao, _subVbo, _subEbo;
		unsigned int	_vao, _vbo, _ebo;
	};
}