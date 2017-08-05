#include "System.h"
#include <iostream>
#include <string>
#include <vector>
#include "Vector.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define Z_NEAR 1.0
#define Z_FAR 2000.0


namespace mm
{
	int halfSubdivision(Triangle tri, std::vector<Triangle> *list, int iteration)
	{
		int totalIt = iteration;
		if (iteration <= 0)
			return 3;
		// calculate 3 new points
		Vertex3f A, B, C, D, E, F;
		A = tri.A; B = tri.B; C = tri.C;
		D.x = (A.x + B.x) / 2.f; D.y = (A.y + B.y) / 2.f; D.z = (A.z + B.z) / 2.f;
		E.x = (B.x + C.x) / 2.f; E.y = (B.y + C.y) / 2.f; E.z = (B.z + C.z) / 2.f;
		F.x = (A.x + C.x) / 2.f; F.y = (A.y + C.y) / 2.f; F.z = (A.z + C.z) / 2.f;

		Triangle t1 = { A,D,F }, t2 = { F,D,E }, t3 = { D,B,E }, t4 = { F,E,C };

		// create 4 triangles
		list->push_back(t1);
		list->push_back(t2);
		list->push_back(t3);
		list->push_back(t4);

		iteration -= 1;
		if (iteration > 0)
		{
			list->pop_back();
			list->pop_back();
			list->pop_back();
			list->pop_back();

			halfSubdivision(t1, list, iteration);
			halfSubdivision(t2, list, iteration);
			halfSubdivision(t3, list, iteration);
			halfSubdivision(t4, list, iteration);
		}

		return (int)pow(4, totalIt);
	}

	System::System()
	{
		// Initial glfw
		std::cout << "Initializing glfw" << std::endl;
		if (!glfwInit())
		{
			std::cout << "Failed to initial glfw!\n";
			exit(1);
		}

		_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Subdivision", NULL, NULL);
		if (!_window)
		{
			std::cout << "Failed to initial glfw window!\n";
			glfwTerminate();
			exit(1);
		}

		glfwMakeContextCurrent(_window);

#ifdef _WIN32
		// Inistial glew
		std::cout << "Initializing glew" << std::endl;
		glewInit();
		GLenum err = glewInit();
		if (GLEW_OK != err)
			exit(1);
		std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
#endif
		std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

		// Clear screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}


	void System::start()
	{
		_init();
		_resize(SCREEN_WIDTH, SCREEN_HEIGHT);
		while (!glfwWindowShouldClose(_window))
		{
			_render();
			glfwPollEvents();
		}
	}

	// destroy
	void System::shutdown()
	{
		glfwTerminate();
	}

	void System::_init()
	{
		// triangle
		Triangle tri = { 0.0f, 0.5f, 0.0f,
						 0.5f, -0.5f, 0.0f,
						-0.5f, -0.5f, 0.0f };

		std::vector<Triangle> *lists = new std::vector<Triangle>;

		_vertCount = halfSubdivision( tri, lists, 7 )*3;

		// vertex buffer object
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, lists->size()*sizeof(Triangle), lists->data(), GL_DYNAMIC_DRAW);

		// vertex array object
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		_initShader();
	}

	void System::_render()
	{
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		glUseProgram(_prog);
		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLES, 0, _vertCount);

		glfwSwapBuffers(_window);
	}

	void System::_resize(int width, int height)
	{
		/*if (height == 0)	height = 1;
		float ratio = (1.0f * width) / (1.0f * height);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, width, height);
		gluPerspective(90, ratio, Z_NEAR, Z_FAR);
		glMatrixMode(GL_MODELVIEW);*/
	}

	char* System::_readFile(char* path, int &size)
	{
		std::ifstream::pos_type length;
		char* temp = 0;
		std::string text;

		std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			length = file.tellg();
			size = (int)length;
			temp = new char[length];
			file.seekg(0, std::ios::beg);
			file.read(temp, length);
			file.close();
			text.assign(temp);

			std::cout << "File " << path << " has been loaded.\n";
		}
		else
		{
			std::cout << "Cannot open file " << path << std::endl;
			return temp;
		}
		return temp;
	}

	void System::_initShader()
	{
		GLuint frag, vert;
		char *vs, *fs;

		int vertLen, fragLen;
		vs = _readFile("mm.vert", vertLen);
		fs = _readFile("mm.frag", fragLen);

		vert = glCreateShader(GL_VERTEX_SHADER);
		frag = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vert, 1, &vs, &vertLen);
		glShaderSource(frag, 1, &fs, &fragLen);

		int isCompiled;
		glCompileShader(vert);
		glGetShaderiv(vert, GL_COMPILE_STATUS, &isCompiled);
		if (!isCompiled)
		{
			_printShaderInfo(vert);
			std::cout << "Failed to compile vertex shader.\n";
		}

		glCompileShader(frag);
		glGetShaderiv(frag, GL_COMPILE_STATUS, &isCompiled);
		if (!isCompiled)
		{
			_printShaderInfo(frag);
			std::cout << "Failed to compile fragment shader.\n";
		}

		_prog = glCreateProgram();

		glAttachShader(_prog, vert);
		glAttachShader(_prog, frag);
		glLinkProgram(_prog);
	}

	void System::_printShaderInfo(int shader)
	{
		int logLen, charsWritten = 0;
		char* infoLog = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			infoLog = new char[logLen];
			glGetShaderInfoLog(shader, logLen, &charsWritten, infoLog);
			std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
		}
		delete[] infoLog;
	}
}