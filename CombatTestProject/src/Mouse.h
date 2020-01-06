#pragma once
#include "NativeHeader.h"
#include "Viewport.h"

nativenamespace
{
	class Mouse
	{
	public:

		Mouse()
		{
			x = 0.0;
			y = 0.0;
			delta = 0;
			position = glm::vec2(0.f);
			oldPosition = glm::vec2(0.f);
			diffInPosition = glm::vec2(0.f);
			mouseMoved = false;
		}

		static GLFWwindow* glfwWindow;
		double x;
		double y;
		double delta;
		bool mouseMoved;
		glm::vec2 position;
		glm::vec2 oldPosition;
		glm::vec2 diffInPosition;

		glm::vec2 UpdatePosition(double X, double Y)
		{
			mouseMoved = true;
			x = X;
			y = Y;
			oldPosition = position;
			position.x = (float)X;
			position.y = (float)Y;
			diffInPosition = position - oldPosition;

			return position;
		}

		glm::vec2 getNDCpos()
		{
			x = (2.f*position.x + 1.f) / WIDTH - 1.f;
			y = -((2.f*position.y + 1.f) / HEIGHT - 1.f);
			return vec2(x, y);
		}

		bool IsButtonUp(int MouseButton)
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, MouseButton);
			if (mouse_button == GLFW_PRESS)
				return false;
			return true;
		}

		bool IsButtonDown(int MouseButton)
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, MouseButton);
			if (mouse_button == GLFW_PRESS)
				return true;
			return false;
		}

		bool IsWheelDown()
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, 4);
			if (mouse_button == GLFW_PRESS)
				return true;
			return false;
		}

		bool IsLeftButtonDown()
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT);
			if (mouse_button == GLFW_PRESS)
				return true;
			return false;
		}

		bool IsLeftButtonUp()
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT);
			if (mouse_button == GLFW_PRESS)
				return false;
			return true;
		}

		bool IsRightButtonDown()
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT);
			if (mouse_button == GLFW_PRESS)
				return true;
			return false;
		}

		bool IsRightButtonUp()
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT);
			if (mouse_button == GLFW_PRESS)
				return false;
			return true;
		}

		bool IsMiddleButtonDown()
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE);
			if (mouse_button == GLFW_PRESS)
				return true;
			return false;
		}

		bool IsMiddleButtonUp()
		{
			int mouse_button = glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE);
			if (mouse_button == GLFW_PRESS)
				return false;
			return true;
		}

		void GetPosition(double* mouse_x,double* mouse_y)
		{
			glfwGetCursorPos(glfwWindow,mouse_x, mouse_y);
		}

		double* GetPosition()
		{
			double result[2];
			glfwGetCursorPos(glfwWindow, &result[0], &result[1]);
			return result;
		}

		void ResetInput()
		{
			mouseMoved = false;
			delta = 0.0;
		}

		void PrintMousePos()
		{
			cout << "\n-> MousePos: X: " << x << endl;
			cout << "\-> MousePos: Y: " << y << endl;
		}

		~Mouse()
		{
		}
	};

}