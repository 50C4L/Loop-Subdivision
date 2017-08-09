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
		_baseNormal = new std::vector<float>(36, 0.f);

		/*_baseVertices = std::vector<float>{
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
		};*/
		_baseVertices = std::vector<float>
		{
			0.0f, 1.f, 0.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 1.0f,
			0.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			0.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f
		};
		
		_baseIndices.reserve(12);
		for (unsigned int i = 0; i < 12; ++i)
		{
			_baseIndices.push_back(i);
			if ((i + 1) % 3 == 0)
			{
				_createTriangle(i - 2, i - 1, i);
			}
		}

		// copy current data
		_currentVertices = std::vector<float>(_baseVertices);
		_currentIndices = std::vector<unsigned int>(_baseIndices);

		// vertex array object
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// vertex buffer object
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, _baseVertices.size()*sizeof(float), _baseVertices.data(), GL_STATIC_DRAW);

		// index buffer object
		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _baseIndices.size()*sizeof(int), _baseIndices.data(), GL_STATIC_DRAW);

		// vertex attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		return _vao;
	}


	void CubeObject::findAdjacentTriangle()
	{
		for (auto it = _triList.begin(); it != _triList.end(); ++it)
		{
			// three edges
			for (int e = 0; e < 3; ++e)
			{
				int enext = (e + 1) % 3;
				// if no adjacent triangle for this edge 
				if (it->oppVert[e] == -1)
				{
					// check remain triangles
					for (auto it2 = it + 1; it2 != _triList.end(); ++it2)
					{
						// get oppisite vertex index if found shared edge
						int oppVertIndex,t2index;
						if ((oppVertIndex = _isShareEdge(it->vertex[enext], it->vertex[e],(*it2),t2index)) >= 0)
						{
							// get oppisite vertex of each edge
							it->oppVert[e] = oppVertIndex;
							it2->oppVert[t2index] = it->vertex[3 - (e + enext)];
							// get adjacent triangles
							it->adjTri[e] = it2 - _triList.begin();
							it2->adjTri[t2index] = it - _triList.begin();
						}
					}
				}
			}
		}
	}

	void CubeObject::subdivide()
	{
		// fill adjacency info
		findAdjacentTriangle();

		// initial
		int midVertNum = _triList.size() * 3;
		_tempTriList.clear();
		_currentVertices.clear();
		_currentIndices.clear();
		_tempTriList.reserve(_triList.size());
		_currentVertices.reserve(_triList.size() * 36);

		for (auto it = _triList.begin(); it != _triList.end(); ++it)
		{
			_split((*it),(it-_triList.begin()));
		}

		_tempTriList.swap(_triList);
		_triList.clear();
		_currentVertices.swap(_baseVertices);

		for (unsigned int i = 0; i < _tempTriList.size()*12; ++i)
		{
			_currentIndices.push_back(i);
			if ((i + 1) % 3 == 0)
			{
				_createTriangle(i - 2, i - 1, i);
			}
		}

		//// debug begins
		// vertex array object
		glGenVertexArrays(1, &_subVao);
		glBindVertexArray(_subVao);

		// vertex buffer object
		glGenBuffers(1, &_subVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _subVbo);
		glBufferData(GL_ARRAY_BUFFER, _baseVertices.size() * sizeof(float), _baseVertices.data(), GL_STATIC_DRAW);

		// index buffer object
		glGenBuffers(1, &_subEbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _subEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _currentIndices.size() * sizeof(int), _currentIndices.data(), GL_STATIC_DRAW);

		// vertex attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		// debug ends
	}

	void CubeObject::drawBaseObj()
	{
		glBindVertexArray(_vao);
		glDrawElements(GL_TRIANGLES, _baseIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void CubeObject::drawSubdividedObj()
	{
		glBindVertexArray(_subVao);
		glDrawElements(GL_TRIANGLES, _tempTriList.size() * 12, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void CubeObject::_split(Triangle tri, int triIndex)
	{
		try {
			vec3 curVert[3];
			vec3 oddVert[3];
			vec3 evenVert[3];
			// get coordinates for current 3 vertices
			curVert[0] = vec3(_baseVertices[tri.vertex[0] * 3], _baseVertices[tri.vertex[0] * 3 + 1], _baseVertices[tri.vertex[0] * 3 + 2]);
			curVert[1] = vec3(_baseVertices[tri.vertex[1] * 3], _baseVertices[tri.vertex[1] * 3 + 1], _baseVertices[tri.vertex[1] * 3 + 2]);
			curVert[2] = vec3(_baseVertices[tri.vertex[2] * 3], _baseVertices[tri.vertex[2] * 3 + 1], _baseVertices[tri.vertex[2] * 3 + 2]);

			for (int i = 0; i < 3; ++i)
			{
				// calculate the odd vertices
				int inext = (i + 1) % 3;
				// if its interior
				// v = 3/8*(a+b)+1/8*(c+d)
				if (tri.oppVert[i] >= 0)
				{
					vec3 oppVert = vec3(_baseVertices[tri.oppVert[i] * 3], _baseVertices[tri.oppVert[i] * 3 + 1], _baseVertices[tri.oppVert[i] * 3 + 2]);
					oddVert[i] = 0.375f*(curVert[i] + curVert[inext]) + 0.125f*(curVert[3 - i - inext] + oppVert);
				}
				// if its on boundary, oppisite vertex not exist
				// v = 1/2*(a+b)
				else
				{
					oddVert[i] = 0.5f*(curVert[i] + curVert[inext]);
				}

				// calculate the even vertices
				int next = (i + 1) % 3, prev = (i + 2) % 3;
				Triangle tempTri = tri;
				bool isInterior = false, isCalc = true;
				std::vector<unsigned int> neighborTri;
				std::vector<vec3> neighborVert;
				neighborTri.push_back(triIndex);
				neighborVert.push_back(curVert[prev]);
				neighborVert.push_back(curVert[next]);

				// find all neighbor vertices
				// copy current location
				int loc = i;
				// find current edge's oppvert
				int vI = tempTri.oppVert[loc];
				int tI = tempTri.adjTri[loc];
				neighborTri.push_back(tI);

				vec3 neigVert = vec3(_baseVertices[vI * 3], _baseVertices[vI * 3 + 1], _baseVertices[vI * 3 + 2]);
				neighborVert.push_back(neigVert);
				while (isCalc)
				{
					if (tI == -1)
						break;
					// get the adjacent triangle
					tempTri = _triList[tI];
					// find neighVert's location
					for (int j = 0; j < 3; ++j)
					{
						int v = tempTri.vertex[j];

						if (neigVert == vec3(_baseVertices[v * 3], _baseVertices[v * 3 + 1], _baseVertices[v * 3 + 2]))
						{
							// find current edge's oppvert
							loc = (j + 2) % 3;
							vI = tempTri.oppVert[loc];
							tI = tempTri.adjTri[loc];
							// if no adjacent triangle
							if (tI == -1)
							{
								isCalc = false;
								break;
							}
							// if loops back to start triangle
							if (std::find(neighborTri.begin(), neighborTri.end(), tI) != neighborTri.end())
							{
								// dump the last vertex as its duplicate
								neighborVert.pop_back();
								isInterior = true;
								isCalc = false;
								break;
							}

							neighborTri.push_back(tI);
							neigVert = vec3(_baseVertices[vI * 3], _baseVertices[vI * 3 + 1], _baseVertices[vI * 3 + 2]);
							neighborVert.push_back(neigVert);
							break;
						}
						else if (j + 1 >= 3)
						{
							std::cout << "Failed to find opp vert in adj triangle.\n";
						}
					}
				}


				// compute even vertex
				if (isInterior)
				{
					float beta;
					int n = neighborVert.size();
					if (n > 3)
						beta = 3.f / (8.f*n);
					else
						beta = 3.f / 16.f;

					vec3 sum(0.f, 0.f, 0.f);
					for each(vec3 nVert in neighborVert)
					{
						sum += nVert;
					}

					evenVert[i] = curVert[i] * (1 - n*beta) + sum*beta;
				}
				else
					evenVert[i] = 0.125f*(neighborVert.front() + neighborVert.back()) + 0.75f*curVert[i];
			}

			// build triangles
			// 3 outter triangles
			for (int n = 0; n < 3; ++n)
			{
				_currentVertices.push_back(evenVert[n].x);
				_currentVertices.push_back(evenVert[n].y);
				_currentVertices.push_back(evenVert[n].z);
				_currentVertices.push_back(oddVert[n].x);
				_currentVertices.push_back(oddVert[n].y);
				_currentVertices.push_back(oddVert[n].z);
				_currentVertices.push_back(oddVert[(n + 2) % 3].x);
				_currentVertices.push_back(oddVert[(n + 2) % 3].y);
				_currentVertices.push_back(oddVert[(n + 2) % 3].z);
			}
			// mid triangle
			for (int n = 0; n < 3; ++n)
			{
				_currentVertices.push_back(oddVert[n].x);
				_currentVertices.push_back(oddVert[n].y);
				_currentVertices.push_back(oddVert[n].z);
			}

		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	
	Triangle CubeObject::_createTriangle(int i0, int i1, int i2)
	{
		Triangle t;
		// get indcies
		t.vertex[0] = i0;
		t.vertex[1] = i1;
		t.vertex[2] = i2;
		t.normal[0] = i0;
		t.normal[1] = i1;
		t.normal[2] = i2;
		for (int i = 0; i < 3; ++i)
		{
			t.oppVert[i] = -1;
			t.adjTri[i] = -1;
		}

		// get coordinates for 3 vertices
		vec3 v1 = vec3(_baseVertices[i0 * 3], _baseVertices[i0 * 3 + 1], _baseVertices[i0 * 3 + 2]);
		vec3 v2 = vec3(_baseVertices[i1 * 3], _baseVertices[i1 * 3 + 1], _baseVertices[i1 * 3 + 2]);
		vec3 v3 = vec3(_baseVertices[i2 * 3], _baseVertices[i2 * 3 + 1], _baseVertices[i2 * 3 + 2]);
		
		// calculate face normal
		/*vec3 nor = cross(vec3(v2 - v1), vec3(v3 - v1));
		nor = normalize(nor);
		int k = i0 * 3;
		for (int i = k; i < (k + 9); i+=3)
		{
			(*_baseNormal)[i] = nor.x;
			(*_baseNormal)[i+1] = nor.y;
			(*_baseNormal)[i+2] = nor.z;
		}*/

		_triList.push_back(t);
		return t;
	}


	int CubeObject::_isShareEdge(int v1, int v2, Triangle tri2, int &in) const
	{
		int ei1 = v1 * 3, ei2 = v2 * 3;
		vec3 e1(_baseVertices[ei1], _baseVertices[ei1 + 1], _baseVertices[ei1 + 2]);
		vec3 e2(_baseVertices[ei2], _baseVertices[ei2 + 1], _baseVertices[ei2 + 2]);
		// look for same edge
		for (int i = 0; i < 3; ++i)
		{
			int inext = (i + 1) % 3;
			int ni1 = tri2.vertex[i]*3, ni2 = tri2.vertex[inext]*3;
			vec3 n1(_baseVertices[ni1], _baseVertices[ni1 + 1], _baseVertices[ni1 + 2]);
			vec3 n2(_baseVertices[ni2], _baseVertices[ni2 + 1], _baseVertices[ni2 + 2]);
			// Found! return index to the oppisite vertex 
			if (n1 == e1 && n2 == e2)
			{
				/*int oi = tri2.vertex[3 - (i + inext)] * 3;
				vec3 o((*_baseVertices)[oi], (*_baseVertices)[oi + 1], (*_baseVertices)[oi + 2]);
				std::cout << "(" << o.x << "," << o.y << "," << o.z << ") " << oi/3 << " \n";
				std::cout << "v1(" << n1.x << "," << n1.y << "," << n1.z << ")\n";
				std::cout << "v2(" << n2.x << "," << n2.y << "," << n2.z << ")\n";*/
				in = i;
				return tri2.vertex[3 - (i + inext)];
			}
		}
		in = -1;
		return -1;
	}
}