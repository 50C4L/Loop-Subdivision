#pragma once
#include <fstream>
#ifdef _WIN32
	#include <glew.h>
#endif
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glfw3.h>


namespace mm
{

	class System
	{
	public:
		System();
		void start();
		void shutdown();

	private:
		void _render();
		void _resize( int width, int height );
		void _init();
		void _initShader();
		void _printShaderInfo(int shader);
		char* _readFile( char* path, int &size );
		
		unsigned int _vao, _vbo, _ibo, _prog;
		int _vertCount;
		float *_cubeVert;
		unsigned int *_cubeIndices;

		GLFWwindow *_window;
	};

}