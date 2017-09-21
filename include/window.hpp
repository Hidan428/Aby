#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <cstdlib>
#include <string>
#include <functional>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
*	GFLW Wrapper. Only things useful for this project has been added. Prefer directly using the GLFW.
*/
class Window {

	public:
		class Builder {
			public:
				Builder(unsigned int glMajorVersion, unsigned int glMinorVersion, bool useCoreProfile = true);
					Builder const& hint(int name, int value) const;
				Window create(unsigned int width, unsigned int height, std::string name, 
					GLFWmonitor* monitor = nullptr, Window const* share = nullptr) const;
		};

		class Event {
			public:
				typedef enum Type {
					UNKNOWN,
					MOUSE_MOVE, // the mouse moved
					MOUSE_PRESS, // a mouse button was pressed
					MOUSE_RELEASE, // a mouse button was released
					MOUSE_SCROLL, // the mouse wheel was used
					KEY_PRESS, // a key was pressed
					KEY_RELEASE, // a key was released
					WINDOW_MOVE, // the window was moved
					WINDOW_RESIZE, // the window was resized
					FOCUS, // the window gained focus
					BLUR, // the window losed focus
					LAST_TYPE
				} Type;

				typedef enum MouseButton {
					NONE,
					LEFT,
					MIDDLE,
					RIGHT
				} MouseButton;

				typedef struct EventData {
					/*
					*	Mouse attribute
					*/
					bool const* buttonState; // reference the the buttonsStates object
					double mouseX; // the X position of the mouse
					double mouseY; // the Y position of the mouse
					double lastMouseX; // the last X position of the mouse
					double lastMouseY; // the last Y position of the mouse
					double offsetX; // the difference between the new position of the mouse and the last on the X axis
					double offsetY; // the difference between the new position of the mouse and the last on the Y axis
					MouseButton button; // 1 : left, 2 : middle, 3 : right
					float scroll; // the scroll offset

					/**
					*	Keyboard attribute
					*/
					bool const* keyState; // reference to the keyState object
					int key; // the last key changed (or 0 if no key was pressed or released)
					bool isKeyPress; // this was a key press (if false, was a key released)
					bool ctrlKey; // whether the modifier CTRL key is pressed or not
					bool shiftKey; // whether the modifier SHIFT key is pressed or not
					bool altKey; // whether the modifier ALT key is pressed or not

					/*
					*	Window attributes
					*/
					int windowWidth; // the width of the window
					int windowHeight; // the height of the window
					int windowPosX; // the X position of the window
					int windowPosY; // the Y position of the window

					double timestamp; // the time of creation of the event using GLFW time function (relative to GLFW init)
					double timeInterval; // the time interval between the current time and the last time this type of event was fired.
					Type type; // the type of the event

				} EventData;
			protected:
				bool _keysStates[GLFW_KEY_LAST + 1]; // whether the keys are pressed or released
				bool _buttonsStates[GLFW_MOUSE_BUTTON_LAST]; // whether the mouse buttons are pressed are released
				double _lastTime[Type::LAST_TYPE + 1]; // the last time this event was fired
				EventData _data;
				bool _firstEvent;

				std::vector<std::function<void (Window &, EventData const&)>*> _mouseMoveCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _mouseDownCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _mouseUpCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _scrollCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _keyDownCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _keyUpCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _moveCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _resizeCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _focusCallbacks;
				std::vector<std::function<void (Window &, EventData const&)>*> _blurCallbacks;

				void update();
				void updateTime(Type type);

			public:
				Event(Window & win);

				~Event();

				EventData const& getData() const;

				double getTime() const;

				/**
				*	Add an event listener on the mouse move.
				*/
				void onMouseMove(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when a mouse button is pressed
				*/
				void onMouseDown(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when a mouse button is released
				*/
				void onMouseUp(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when the mouse wheel is used
				*/
				void onMouseScroll(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when a key is pressed
				*/
				void onKeyDown(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when a key is released
				*/
				void onKeyUp(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when the window is moved
				*/
				void onWindowMove(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when this window is resized
				*/
				void onWindowResize(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when the window receives the focus
				*/
				void onFocus(std::function<void (Window &, EventData const&)> const& callback);

				/**
				*	Adds an event listener on when the window loses focus.
				*/
				void onBlur(std::function<void (Window &, EventData const&)> const& callback);

				friend void _windowKeyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
				friend void _windowMouseMoveCallback(GLFWwindow* win, double xpos, double ypos);
				friend void _windowMoveCallback(GLFWwindow* win, int xpos, int ypos);
				friend void _windowResizeCallback(GLFWwindow* win, int width, int height);
				friend void _windowFocusCallback(GLFWwindow* win, int focus);
				friend void _windowScrollCallback(GLFWwindow* win, double offsetX, double offsetY);
				friend void _windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		};

	protected:
		static bool _exitCallbackDefined;
		static bool _glewAlreadyInitialized;
		static std::map<GLFWwindow*, Window*> _reverseLookup;

		GLFWwindow* _win;
		bool _initSuccess;
		std::string* _errorMessage;

		Event *_events;

		static void terminateGLFW();
	public:

		/**
		*	Enables vsync on the current context (use makeContextCurrent() to switch context).
		*	A window MUST have been created before calling this method.
		*/
		static void enableVSync();

		/**
		*	Disables vsync on the current context (use makeContextCurrent() to switch context).
		*	A window MUST have been created before calling this method.
		*/
		static void disableVSync();

		/**
		*	Create a new window and openGL context. Initialize glfw and glew if needed
		*	@param[in] width - The width of the window's screen.
		*	@param[in] height - The height of the window's screen.
		*	@param[in] name - The window title.
		*	@param[in] allowResize - Whether the user is allowed to resize the window or not.
		*	@param[in] monitor - The monitor to use for fullscreen mode. Windowed mode if nullptr.
		*	@param[in] share - The window to share the new context with. The context is not shared if the value is nullptr.
		*/
		Window(unsigned int width, unsigned int height, std::string name, GLFWmonitor* monitor = nullptr,
			Window const* share = nullptr);

		~Window();

		/**
		*	Returns whether the object creation was a success or not.
		*	In case of error, check getError() to get the most recent error.
		*	@return true in cas of success, false otherwise.
		*/
		bool success() const;

		/**
		* Returns the last most recent error message.
		*	@return The most recent error message or nullptr if no error occured.
		*/
		std::string const& getError() const;

		/**
		*	Make this window's openGL context the current in the calling thread.
		*/
		void makeContextCurrent();

		/**
		*	Returns the GLFW window object of this Window.
		*	@return The GLFW window object.
		*/
		GLFWwindow* getGLFWWindow();

		/**
		*	Returns the closing flag of the window.
		*	@return true if the window should close. False otherwise.
		*/
		bool shouldClose() const;

		/**
		*	Set the closing flag of the window.
		*	@param[in] flag - True if the window should close. False otherwise.
		*/
		void shouldClose(bool flag);

		/**
		*	Swap the back and front buffers.
		*/
		void swapBuffers() const;

		/**
		*	Returns the size of the window's framebuffer
		*	@param[out] width - The width of the framebuffer
		*	@param[out] height - The height of the framebuffer
		*/
		void getFramebufferSize(int & width, int & height) const;

		/**
		*	Set the window to windowed mode.
		*	@param[in] width - The width of the window's screen.
		*	@param[in] height - The height of the window's screen.
		*	@param[in] xpos - The distance from the top-left corner on the x axis.
		*	@param[in] ypos - The distance from the top-left corner on the y axis.
		*/
		void setWindowedMode(unsigned int width, unsigned int height, int xpos = 50, int ypos = 50);

		/**
		*	Set the window to fullscreen mode.
		*	@param[in] monitor - The monitor to set the window on. If nullptr, go on the primary monitor.
		*	@param[in] width - The width of the window's screen.
		*	@param[in] height - The height of the window's screen.
		*	@param[in] refreshrate - The refreshrate in Hz.
		*/
		void setFullscreenMode(GLFWmonitor* monitor, unsigned int width, unsigned int height, 
			int refreshrate = GLFW_DONT_CARE);

		/**
		*	Set the window to fullscreen mode. Change the resolution and refreshrate to
		*	the one of the monitor.
		*	@param[in] monitor - The monitor to set the window on. If nullptr, go on the primary monitor.
		*	@param[in] framerate - The refreshrate to set to the monitor. Using the monitor refreshrate if 0.
		*/
		void setFullscreenMode(GLFWmonitor* monitor = glfwGetPrimaryMonitor(), int framerate = 0);

		/**
		*	Returns whether the window is currently in fullscreen mode or not.
		*	@return true if the window is fullscreen. False otherwise.
		*/
		bool isFullscreen() const;

		/**
		*	Change the resolution of the window.
		*	@param[in] width - The width of the window's screen.
		*	@param[in] height - The height of the window's screen.
		*/
		void setSize(int width, int height);

		/**
		*	Returns the resolution of the window's screen.
		*	@param[out] width - The width of the window's screen.
		*	@param[out] height - The height of the window's screen.
		*/
		void getSize(int &width, int &height) const;

				/**
		*	Change the position of the window.
		*	@param[in] xpos - The distance from the top-left corner on the x axis.
		*	@param[in] ypos - The distance from the top-left corner on the y axis.
		*/
		void setPos(int xpos, int ypos);

		/**
		*	Returns the position of the window's screen.
		*	@param[out] xpos - The distance from the top-left corner on the x axis.
		*	@param[out] ypos - The distance from the top-left corner on the y axis.
		*/
		void getPos(int &xpos, int &ypos) const;

		/**
		*	Changes the title of the window.
		*	@param[in] title - The UTF-8 encoded title.
		*/
		void setTitle(std::string const& title);

		/**
		*	Show the window if hidden.
		*/
		void show();

		/**
		*	Hide The window if shown.
		*/
		void hide();

		/**
		*	Give user focus on the window.
		*/
		void focus();

		/**
		*	Indicates whether the window has the focus or not.
		*	@return True if the window has the focus, false otherwise.
		*/
		bool isFocused() const;

		/**
		*	Hide and grabs the cursor.
		*/
		void grabCursor();

		/**
		*	Hide the cursor when over the window.
		*/
		void hideCursor();

		/**
		*	Set the cursor to its default, visible state.
		*/
		void showCursor();

		/**
		*	Returns an attribute of the window (using GLFW window attributes constants).
		*	@param[in] attrib - The GLFW window attribute constant that correspond to the attribute to query.
		*	@return The value of the attribute.
		*/
		int getAttrib(int attrib) const;

		Event & event();

		friend void _windowKeyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
		friend void _windowMouseMoveCallback(GLFWwindow* win, double xpos, double ypos);
		friend void _windowMoveCallback(GLFWwindow* win, int xpos, int ypos);
		friend void _windowResizeCallback(GLFWwindow* win, int width, int height);
		friend void _windowFocusCallback(GLFWwindow* win, int focus);
		friend void _windowScrollCallback(GLFWwindow* win, double offsetX, double offsetY);
		friend void _windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

void _windowKeyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
void _windowMouseMoveCallback(GLFWwindow* win, double xpos, double ypos);
void _windowMoveCallback(GLFWwindow* win, int xpos, int ypos);
void _windowResizeCallback(GLFWwindow* win, int width, int height);
void _windowFocusCallback(GLFWwindow* win, int focus);
void _windowScrollCallback(GLFWwindow* win, double offsetX, double offsetY);
void _windowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
#endif // _WINDOW_HPP_