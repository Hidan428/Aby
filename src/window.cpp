#include "window.hpp"

bool Window::_exitCallbackDefined = false;
bool Window::_glewAlreadyInitialized = false;
std::map<GLFWwindow*, Window*> Window::_reverseLookup;

void Window::terminateGLFW() {
	glfwTerminate();
}

void Window::enableVSync() {
	if(_reverseLookup.empty()) { // no context
		return;
	}

	glfwSwapInterval(1);
}

void Window::disableVSync() {	
	if(_reverseLookup.empty()) { // no context
		return;
	}
	
	glfwSwapInterval(0);
}

Window::Window(unsigned int width, unsigned int height, std::string name, 
	GLFWmonitor* monitor, Window const* share){
	_initSuccess = true;
	_win = nullptr;
	_errorMessage = nullptr;

	// init GLFW
	if(glfwInit() != GLFW_TRUE) {
		_initSuccess = false;
		_errorMessage = new std::string("Failed to init GLFW.");
		return;
	}

	// set the exit callback
	if(!_exitCallbackDefined) {
		atexit(terminateGLFW);
		_exitCallbackDefined = true;
	}

	_win = glfwCreateWindow(width, height, name.c_str(), monitor, share == nullptr ? nullptr : share->_win);

	if(_win == nullptr) {
		_initSuccess = false;
		_errorMessage = new std::string("Failed to create a window.");
		return;
	}

	makeContextCurrent();

	// init glew
	glewExperimental = GL_TRUE;
	if(!_glewAlreadyInitialized) {
		if(glewInit() != GLEW_OK) {
			_initSuccess = false;
			_errorMessage = new std::string("Failed to init GLEW.");
			return;
		}
		_glewAlreadyInitialized = true;
	}	

	_reverseLookup[_win] = this;

	_events = new Event(*this);
}

Window::~Window() {
	if(_errorMessage != nullptr) {
		delete _errorMessage;
	}

	if(_events != nullptr) {
		delete _events;
	}

	// remove from the lookup table
	_reverseLookup.erase(_win);
	glfwDestroyWindow(_win);
}

bool Window::success() const {
	return _initSuccess;
}

std::string const& Window::getError() const {
	return *_errorMessage;
}


void Window::makeContextCurrent() {
	glfwMakeContextCurrent(_win);
}


GLFWwindow* Window::getGLFWWindow() {
	return _win;
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(_win);
}

void Window::shouldClose(bool flag) {
	glfwSetWindowShouldClose(_win, flag);
}

void Window::swapBuffers() const {
	glfwSwapBuffers(_win);
}

void Window::getFramebufferSize(int & width, int & height) const {
	glfwGetFramebufferSize(_win, &width, &height);
}

void Window::setWindowedMode(unsigned int width, unsigned int height, int xpos, int ypos) {
	glfwSetWindowMonitor(_win, nullptr, xpos, ypos, width, height, 0);
}

void Window::setFullscreenMode(GLFWmonitor* monitor, unsigned int width, unsigned int height, 
	int refreshrate) {
	if(monitor == nullptr) {
		monitor = glfwGetPrimaryMonitor();
	}
	
	glfwSetWindowMonitor(_win, monitor, 0, 0, width, height, refreshrate);
}

void Window::setFullscreenMode(GLFWmonitor* monitor, int framerate) {
	if(monitor == nullptr) {
		monitor = glfwGetPrimaryMonitor();
	}
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	setFullscreenMode(glfwGetPrimaryMonitor(), mode->width, mode->height, framerate ? framerate : mode->refreshRate);
}

bool Window::isFullscreen() const {
	return glfwGetWindowMonitor(_win) != nullptr;
}

void Window::setSize(int width, int height) {
	glfwSetWindowSize(_win, width, height);
}

void Window::getSize(int &width, int &height) const {
	glfwGetWindowSize(_win, &width, &height);
}

void Window::setPos(int xpos, int ypos) {
	glfwSetWindowPos(_win, xpos, ypos);
}

void Window::getPos(int &xpos, int &ypos) const {
	glfwGetWindowPos(_win, &xpos, &ypos);
}

void Window::setTitle(std::string const& title) {
	glfwSetWindowTitle(_win, title.c_str());
}

void Window::show() {
	glfwShowWindow(_win);
}

void Window::hide() {
	glfwHideWindow(_win);
}

void Window::focus() {
	glfwFocusWindow(_win);
}

bool Window::isFocused() const {
	return glfwGetWindowAttrib(_win, GLFW_FOCUSED);
}

void Window::grabCursor() {
	glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::hideCursor() {
	glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Window::showCursor() {	
	glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

int Window::getAttrib(int attrib) const {
	return glfwGetWindowAttrib(_win, attrib);
}

Window::Event & Window::event() {
	return *_events;
}


/*************** Window builder *************************/


Window::Builder::Builder(unsigned int glMajorVersion, unsigned int glMinorVersion, bool useCoreProfile) {
	if(!glfwInit()) {
		return;
	}

	glfwDefaultWindowHints(); // resets the hints

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, useCoreProfile ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
}

Window::Builder const& Window::Builder::hint(int name, int value) const {
	glfwWindowHint(name, value);

	return *this;
}

Window Window::Builder::create(unsigned int width, unsigned int height, std::string name, 
			GLFWmonitor* monitor, Window const* share) const {
	return Window(width, height, name, monitor, share);
}

/****************** Window event ********************************/

Window::Event::Event(Window & win) {
	for(unsigned int i = 0; i < GLFW_KEY_LAST + 1; ++i) {
		_keysStates[i] = false;
	}

	for(unsigned int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
		_buttonsStates[i] = false;
	}

	for(unsigned int i = 0; i < Window::Event::Type::LAST_TYPE + 1; ++i) {
		_lastTime[i] = 0;
	}

	_firstEvent = true;

	_data.mouseX = 0;
	_data.mouseY = 0;
	_data.lastMouseX = 0;
	_data.lastMouseY = 0;
	_data.offsetX = 0;
	_data.offsetY = 0;
	_data.button = Window::Event::MouseButton::NONE;
	_data.scroll = 0;
	_data.keyState = _keysStates;
	_data.buttonState = _buttonsStates;
	_data.key = 0;
	_data.isKeyPress = false;
	_data.ctrlKey = false;
	_data.shiftKey = false;
	_data.altKey = 0;
	_data.windowHeight = 0;
	_data.windowWidth = 0;
	_data.windowPosX = 0;
	_data.windowPosY = 0;
	_data.timestamp = glfwGetTime();
	_data.type = Window::Event::Type::UNKNOWN;

	glfwSetKeyCallback(win.getGLFWWindow(), _windowKeyCallback);
	glfwSetScrollCallback(win.getGLFWWindow(), _windowScrollCallback);
	glfwSetWindowFocusCallback(win.getGLFWWindow(), _windowFocusCallback);
	glfwSetWindowSizeCallback(win.getGLFWWindow(), _windowResizeCallback);
	glfwSetWindowPosCallback(win.getGLFWWindow(), _windowMoveCallback);
	glfwSetCursorPosCallback(win.getGLFWWindow(), _windowMouseMoveCallback);
	glfwSetMouseButtonCallback(win.getGLFWWindow(), _windowMouseButtonCallback);
}

Window::Event::~Event() {
	for(auto& i : _mouseMoveCallbacks) {
		delete i;
	}

	for(auto& i : _mouseDownCallbacks) {
		delete i;
	}

	for(auto& i : _mouseUpCallbacks) {
		delete i;
	}

	for(auto& i : _scrollCallbacks) {
		delete i;
	}

	for(auto& i : _keyDownCallbacks) {
		delete i;
	}

	for(auto& i : _keyUpCallbacks) {
		delete i;
	}

	for(auto& i : _moveCallbacks) {
		delete i;
	}

	for(auto& i : _resizeCallbacks) {
		delete i;
	}

	for(auto& i : _focusCallbacks) {
		delete i;
	}

	for(auto& i : _blurCallbacks) {
		delete i;
	}
}

Window::Event::EventData const& Window::Event::getData() const {
	return _data;
}

double Window::Event::getTime() const {
	return glfwGetTime();
}

void Window::Event::onMouseMove(std::function<void (Window &, EventData const&)> const& callback) {
	_mouseMoveCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onMouseDown(std::function<void (Window &, EventData const&)> const& callback) {
	_mouseDownCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onMouseUp(std::function<void (Window &, EventData const&)> const& callback) {
	_mouseUpCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onMouseScroll(std::function<void (Window &, EventData const&)> const& callback) {
	_scrollCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onKeyDown(std::function<void (Window &, EventData const&)> const& callback) {
	_keyDownCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onKeyUp(std::function<void (Window &, EventData const&)> const& callback) {
	_keyUpCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onWindowMove(std::function<void (Window &, EventData const&)> const& callback) {
	_moveCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onWindowResize(std::function<void (Window &, EventData const&)> const& callback) {
	_resizeCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onFocus(std::function<void (Window &, EventData const&)> const& callback) {
	_focusCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::onBlur(std::function<void (Window &, EventData const&)> const& callback) {
	_blurCallbacks.push_back(new std::function<void (Window &, EventData const&)>(callback));
}

void Window::Event::update() {
	_data.scroll = 0;
}

void Window::Event::updateTime(Window::Event::Type type) {	
	_data.timestamp = glfwGetTime();
	if(_lastTime[type] == 0) {
		_data.timeInterval = _data.timestamp;
	}
	else {
		_data.timeInterval = _data.timestamp - _lastTime[type];
	}
	
	_lastTime[type] = _data.timestamp;
}

void _windowKeyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
	auto it =  Window::_reverseLookup.find(win);

	if(it != Window::_reverseLookup.end()) {
		auto& event = *it->second->_events;
		event.update();
		if(action == GLFW_PRESS || action == GLFW_REPEAT) {
			event._keysStates[key] = true;
			event._data.key = key;
			event._data.type = Window::Event::KEY_PRESS;
			event._data.isKeyPress = true;
		}
		else {
			event._keysStates[key] = false;
			event._data.key = key;
			event._data.type = Window::Event::KEY_RELEASE;
			event._data.isKeyPress = false;
		}

		event._data.altKey = mods & GLFW_MOD_ALT;
		event._data.shiftKey = mods & GLFW_MOD_SHIFT;
		event._data.ctrlKey = mods & GLFW_MOD_CONTROL;

		event.updateTime(event._data.type);

		if(action == GLFW_PRESS || action == GLFW_REPEAT) {
			for(auto& i : event._keyDownCallbacks) {
				i->operator()(*it->second, event._data);
			}
		}
		else {
			for(auto& i : event._keyUpCallbacks) {
				i->operator()(*it->second, event._data);
			}
		}
	}
}

void _windowMouseMoveCallback(GLFWwindow* win, double xpos, double ypos) {
	auto it =  Window::_reverseLookup.find(win);

	if(it != Window::_reverseLookup.end()) {
		auto& event = *it->second->_events;
		event.update();
		int lastMouseX = event._data.mouseX;
		int lastMouseY = event._data.mouseY;

		ypos = event._data.windowHeight - ypos;

		event._data.mouseX = xpos;
		event._data.mouseY = ypos;

		if(event._firstEvent) {
			event._data.lastMouseX = xpos;
			event._data.lastMouseY = ypos;
			event._firstEvent = false;
		}
		else {
			event._data.lastMouseX = lastMouseX;
			event._data.lastMouseY = lastMouseY;
		}

		event._data.offsetX = event._data.mouseX - event._data.lastMouseX;
		event._data.offsetY = event._data.mouseY - event._data.lastMouseY;

		event._data.type = Window::Event::MOUSE_MOVE;
		
		event.updateTime(event._data.type);

		for(auto& i : event._mouseMoveCallbacks) {
			i->operator()(*it->second, event._data);
		}
	}
}

void _windowMoveCallback(GLFWwindow* win, int xpos, int ypos) {
	auto it = Window::_reverseLookup.find(win);

	if(it != Window::_reverseLookup.end()) {
		auto& event = *it->second->_events;
		event.update();
		event._data.windowPosX = xpos;
		event._data.windowPosY = ypos;
		event._data.type = Window::Event::WINDOW_MOVE;
		
		event.updateTime(event._data.type);

		for(auto& i : event._moveCallbacks) {
			i->operator()(*it->second, event._data);
		}
	}
}

void _windowResizeCallback(GLFWwindow* win, int width, int height) {
	auto it = Window::_reverseLookup.find(win);

	if(it != Window::_reverseLookup.end()) {
		auto& event = *it->second->_events;
		event.update();
		event._data.windowWidth = width;
		event._data.windowHeight = height;
		event._data.type = Window::Event::WINDOW_RESIZE;
		
		event.updateTime(event._data.type);

		for(auto& i : event._resizeCallbacks) {
			i->operator()(*it->second, event._data);
		}
	}
}

void _windowFocusCallback(GLFWwindow* win, int focus) {
	auto it = Window::_reverseLookup.find(win);

	if(it != Window::_reverseLookup.end()) {
		auto& event = *it->second->_events;
		event.update();
		if(focus) {
			event._data.type = Window::Event::FOCUS;
		
			event.updateTime(event._data.type);

			for(auto& i : event._focusCallbacks) {
				i->operator()(*it->second, event._data);
			}
		}
		else {
			event._data.type = Window::Event::BLUR;
		
			event.updateTime(event._data.type);

			for(auto& i : event._blurCallbacks) {
				i->operator()(*it->second, event._data);
			}
		}
	}
}

void _windowScrollCallback(GLFWwindow* win, double offsetX, double offsetY) {
	auto it = Window::_reverseLookup.find(win);

	if(it != Window::_reverseLookup.end()) {
		auto& event = *it->second->_events;
		event.update();
		event._data.scroll = offsetY;
		event._data.type = Window::Event::MOUSE_SCROLL;
		
		event.updateTime(event._data.type);

		for(auto& i : event._scrollCallbacks) {
			i->operator()(*it->second, event._data);
		}
	}
}

void _windowMouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
	auto it = Window::_reverseLookup.find(win);

	if(it != Window::_reverseLookup.end()) {
		auto& event = *it->second->_events;
		event.update();
		event._buttonsStates[button] = action == GLFW_PRESS;
		event._data.button = (button == GLFW_MOUSE_BUTTON_LEFT) ? Window::Event::LEFT : (button == GLFW_MOUSE_BUTTON_MIDDLE ? 
			Window::Event::MIDDLE : button == GLFW_MOUSE_BUTTON_RIGHT ? Window::Event::RIGHT : Window::Event::NONE);
		event._data.altKey = mods & GLFW_MOD_ALT;
		event._data.shiftKey = mods & GLFW_MOD_SHIFT;
		event._data.ctrlKey = mods & GLFW_MOD_CONTROL;

		if(action == GLFW_PRESS) {
			event._data.type = Window::Event::MOUSE_PRESS;
		
			event.updateTime(event._data.type);

			for(auto& i : event._mouseDownCallbacks) {
				i->operator()(*it->second, event._data);
			}
		}
		else {
			event._data.type = Window::Event::MOUSE_RELEASE;
		
			event.updateTime(event._data.type);

			for(auto& i : event._mouseUpCallbacks) {
				i->operator()(*it->second, event._data);
			}
		}
	}
}
