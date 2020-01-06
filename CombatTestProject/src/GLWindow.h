#pragma once
#include "NativeHeader.h"
//#include "Engine.h"
#include "Input.h"
#include "Viewport.h"

nativenamespace{

	class GLWindow {

	public:

		int smoothness = 8;
		Viewport viewport;
		GLFWwindow* glfwWindow;
		string name;

		void Open(int windowWidth = defaultWIDTH,int windowHeight = defaultHEIGHT,string Name = WINDOW_TILE)
		{
			viewport.Size(windowWidth, windowHeight);
			name = Name;
			InitGLFW();
			InitGLEW();
		}

		void SetCurrentContext(GLFWwindow * w)
		{
			glfwMakeContextCurrent(w);
			glfwSwapInterval(1);
		}

		void PrintGLFWVersion()
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 10);
			int major;
			int minor;
			int version;
			glfwGetVersion(&version, &major, &minor);
			cout << "\nGLFW::Version: " << version << "." << major << "." << minor << endl;
			SetConsoleTextAttribute(hConsole, 15);
		}

		void Close()
		{
			glfwDestroyWindow(glfwWindow);
			glfwTerminate();
		}

		~GLWindow()
		{
			Close();
		}

	private:

		bool glew_started = false;

		void InitGLFW()
		{
			if (!glfwInit())
			{
				cout << "FAILED to initialise GLFW" << endl;
				exit(EXIT_FAILURE);
			}

			//glfwSetErrorCallback(&error_callback);
			glfwWindowHint(GLFW_SAMPLES, smoothness);  //multi sample buffers
			glfwWindowHint(GLFW_SRGB_CAPABLE, TRUE);
			glfwWindowHint(GLFW_FOCUSED, 1);
			PrintGLFWVersion();
			glfwWindow = glfwCreateWindow(viewport.width, viewport.height, name.c_str(), NULL, NULL);
			if (!glfwWindow)
			{
				glfwTerminate();
				exit(EXIT_FAILURE);
			}

			SetCurrentContext(glfwWindow);
			glfwSetWindowPos(glfwWindow, 500, 50);
			glfwSetWindowUserPointer(glfwWindow, this);

			HookEvents();
			checkGLError("GLFW Init check");
		}

		void InitGLEW()
		{
			if (glew_started)
				return;

			GLenum glewErr = glewInit();
			if (GLEW_OK != glewErr)
			{
				cout << "\nGLEW Error: " << glewErr << endl;
				return;
			}

			cout << "\n-> GLEW OK" << endl;
			const GLubyte* version = glGetString(GL_VERSION);
			cout << ".....OpenGL Version " << version << endl;

			//initial opengl state
			bool enableMultisample = true;
			if (enableMultisample)
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);

			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glClearDepth(1.0f);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			checkGLError("Initialize GLEW check");

			glew_started = true;
		}

		static void OnError(int error, const char* description)
		{
		}

		static void OnKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Input::keyboard.OnKeyboard(key, scancode, action, mods);
		}

		static void OnMouseMove(GLFWwindow* window, double x, double y)
		{
			Input::mouse.UpdatePosition(x, y);
		}

		static void OnMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
		{
			Input::mouse.delta = deltaY;
		}

		static void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
		{
			//cout << "Mouse button: " << button << endl;
		}

		static void OnWindowResized(GLFWwindow* window, GLsizei width, GLsizei height)
		{
			GLWindow* w = (GLWindow*)glfwGetWindowUserPointer(window);
			w->viewport.Size(width, height);

			/*
			if (width>height)
			{
				glViewport((width - height) / 2, 0, std::min(width, height), std::min(width, height));
			}
			else
				glViewport(0, (height - width) / 2, std::min(width, height), std::min(width, height));
				*/
		}

		static void OnDrop(GLFWwindow* window, int count, const char** paths)
		{
		}

		static void OnFocus(GLFWwindow* window, int focused)
		{
		}

		void HookEvents()
		{
			glfwSetKeyCallback(glfwWindow, &OnKeyboard);
			glfwSetCursorPosCallback(glfwWindow, &OnMouseMove);
			glfwSetScrollCallback(glfwWindow, &OnMouseScroll);
			glfwSetMouseButtonCallback(glfwWindow, &OnMouseButton);
			glfwSetWindowSizeCallback(glfwWindow, &OnWindowResized);
			glfwSetDropCallback(glfwWindow, &OnDrop);
			glfwSetWindowFocusCallback(glfwWindow, &OnFocus);
			glfwSetWindowUserPointer(glfwWindow, this);
			//glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetInputMode(glfwWindow, GLFW_STICKY_MOUSE_BUTTONS, 1);

			Mouse::glfwWindow = glfwWindow;
		}
	};
}

