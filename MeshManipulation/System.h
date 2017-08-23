#pragma once
#include <fstream>
#ifdef _WIN32
	#include <glew.h>
#endif
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glfw3.h>
#include "Helper.h"


namespace mm
{

	class System
	{
	public:
		System();
		void start();
		void shutdown();

	private:
		void _input();
		void _render();
		void _resize( int width, int height );
		void _init();
		void _initShader();
		void _printShaderInfo(int shader);
		char* _readFile( char* path, int &size );
		
		unsigned int _vao, _vbo, _ibo;
		unsigned int _prog;

		CubeObject *_cube, *_cube2, *_cube3, *_cube4;

		GLFWwindow *_window;
	};

}