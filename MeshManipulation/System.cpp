#include "System.h"
#include <iostream>
#include <string>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Helper.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define Z_NEAR 1.0f
#define Z_FAR 1000.0f


namespace mm
{
	// subdivision func - old code
	/*int halfSubdivision(Triangle tri, std::vector<Triangle> *list, int iteration)
	{
		int totalIt = iteration;
		if (iteration <= 0)
		{
			list->push_back(tri);
			return 1;
		}
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
	}*/

	// Constructor
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

	// start()
	void System::start()
	{
		_init();
		
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

	// _inti();
	void System::_init()
	{
		_cube = new CubeObject();
		_cube->createBaseShape();
		/*for (int i = 0; i < 1; ++i)
			_cube->subdivide();*/

		/*glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);*/

		_initShader();
	}

	// _render()
	void System::_render()
	{
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);
		_resize(width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		glUseProgram(_prog);
		_cube->drawBaseObj();
		_cube->drawSubdividedObj();

		glfwSwapBuffers(_window);
	}

	// _resize()
	void System::_resize(int width, int height)
	{
		if (height == 0)	height = 1;
		float ratio = (1.0f * width) / (1.0f * height);
		
		glViewport(0, 0, width, height);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.f), width*1.f / height, Z_NEAR, Z_FAR);
		glm::mat4 view;
		view = glm::lookAt(glm::vec3(0.f, 2.0f, 2.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f,1.f,-1.f));
		view = glm::translate(view, glm::vec3(0.f, 0.f, 0.f));
		static float angle = 45.0f;
		angle += 0.01f;
		if (angle >= 360.f) angle = 0.f;
		glm::mat4 model(1.f);
		model = glm::rotate(model, angle, glm::vec3(0.f, 1.f, 0.f));
		
		glUseProgram(_prog);
		int locProj = glGetUniformLocation(_prog, "projection");
		int locView = glGetUniformLocation(_prog, "view");
		int locModel = glGetUniformLocation(_prog, "model");
		glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
	}

	// _readFile()
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

	// _initShader()
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

	// _printShaderInfor()
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