#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>

#include <GLFW/glfw3.h>

#include "error.hpp"
#include "window.hpp"



int main() {
	Window win = Window::Builder(3, 3, true)
		.hint(GLFW_RESIZABLE, true)
		.hint(GLFW_SAMPLES, 4)
		.create(800, 600, "Aby - The 3D labyrinth game");

	if(!win.success()) {
		std::cout << "Error while creating the window : " << win.getError() << std::endl;
		return EXIT_FAILURE;
	}
	
	std::cout << "OpenGL version : " << glGetString(GL_VERSION) << std::endl;

	win.event().onKeyDown([](Window &win, Window::Event::EventData const& data) {
		if(data.key == GLFW_KEY_ESCAPE) {
			win.shouldClose(true);
			return;
		}
	});

	while(!win.shouldClose()) {
		glfwPollEvents();
		win.swapBuffers();
	}

	return EXIT_SUCCESS;
}
