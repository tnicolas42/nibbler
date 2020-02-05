#include "NibblerOpenGL.hpp"
#include "Logging.hpp"
#include "debug.hpp"
#include "Material.hpp"

std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
}

const float	NibblerOpenGL::_cubeVertices[] = {
	// positions			// normals
	0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,  // 0l
	-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,  // 0l
	-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,  // 0l

	-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,  // 0r
	0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,  // 0r
	0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,  // 0r


	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,  // 1l
	0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,  // 1l
	0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,  // 1l

	0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,  // 1r
	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,  // 1r
	0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,  // 1r


	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,  // 2l
	0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,  // 2l
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,  // 2l

	0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,  // 2r
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,  // 2r
	-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,  // 2r


	-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,  // 3l
	-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,  // 3l
	-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,  // 3l

	-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,  // 3r
	-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,  // 3r
	-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,  // 3r


	-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,  // 4l
	0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,  // 4l
	0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,  // 4l

	0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,  // 4r
	-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,  // 4r
	-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,  // 4r


	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,  // 5l
	0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,  // 5l
	0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,  // 5l

	0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,  // 5r
	-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,  // 5r
	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f   // 5r
};

NibblerOpenGL::NibblerOpenGL() :
  _win(nullptr),
  _event(new SDL_Event()),
  _context(0),
  _lastLoopMs(0) {
	// init logging
	#if DEBUG
		logging.setLoglevel(LOGDEBUG);
		logging.setPrintFileLine(LOGWARN, true);
		logging.setPrintFileLine(LOGERROR, true);
		logging.setPrintFileLine(LOGFATAL, true);
	#else
		logging.setLoglevel(LOGINFO);
	#endif
}

NibblerOpenGL::~NibblerOpenGL() {
	logInfo("exit OpenGL");
	delete _event;
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_win);
    SDL_Quit();
}

NibblerOpenGL::NibblerOpenGL(NibblerOpenGL const &src) {
	*this = src;
}

NibblerOpenGL &NibblerOpenGL::operator=(NibblerOpenGL const &rhs) {
	if (this != &rhs) {
		_win = rhs._win;
		_surface = rhs._surface;
		_event = rhs._event;
	}
	return *this;
}

bool NibblerOpenGL::_init() {
	logInfo("loading OpenGL");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        logErr("while loading OpenGL: " << SDL_GetError());
        SDL_Quit();
		return false;
    }


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	_win = SDL_CreateWindow((_gameInfo->title + " OpenGL").c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		_gameInfo->width, _gameInfo->height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (_win == nullptr) {
        logErr("while loading OpenGL: " << SDL_GetError());
		return false;
	}

	_context = SDL_GL_CreateContext(_win);
    if (_context == 0) {
        logErr("while loading OpenGL: " << SDL_GetError());
        return false;
    }

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        logErr("while loading OpenGL: failed to init glad");
        return false;
	}

	glEnable(GL_MULTISAMPLE);  // anti aliasing
	// glEnable(GL_CULL_FACE);  // face culling
	glEnable(GL_BLEND);  // enable blending (used in textRender)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	try {
		_cubeShader = new Shader(CUBE_VS_PATH, CUBE_FS_PATH);
	}
	catch (Shader::ShaderError & e) {
        logErr("while loading OpenGL: " << e.what());
		return false;
	}

	_cam = new Camera(
		glm::vec3(4.179586, 32.320001, 34.207786),
		glm::vec3(0.217805, 0.593419, -0.774865),
		-74.3, -53.6);

	float angle = _cam->zoom;
	float ratio = static_cast<float>(_gameInfo->width) / _gameInfo->height;
	float nearD = 0.1f;
	float farD = 100;
	_projection = glm::perspective(glm::radians(angle), ratio, nearD, farD);

	glGenVertexArrays(1, &_cubeShaderVAO);
    glGenBuffers(1, &(_cubeShaderVBO));

    glBindBuffer(GL_ARRAY_BUFFER, _cubeShaderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(NibblerOpenGL::_cubeVertices), NibblerOpenGL::_cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(_cubeShaderVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	_cubeShader->use();
	_cubeShader->setMat4("projection", _projection);
	// set cube material
	Material material;
	_cubeShader->setVec3("material.specular", material.specular);
	_cubeShader->setFloat("material.shininess", material.shininess);

	// set direction light
	_cubeShader->setVec3("dirLight.direction", -0.2f, -0.8f, 0.6f);
	_cubeShader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_cubeShader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
	_cubeShader->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

	// set point light
	_cubeShader->setBool("pointLight.enabled", false);

	_lastLoopMs = getMs().count();

    return true;
}

void NibblerOpenGL::updateInput() {
	uint64_t time = getMs().count();
	float dtTime = (time - _lastLoopMs) / 1000.0;
	_lastLoopMs = time;
	while (SDL_PollEvent(_event)) {
		if (_event->window.event == SDL_WINDOWEVENT_CLOSE)
			input.quit = true;
		if (_event->key.type == SDL_KEYDOWN) {
			if (_event->key.keysym.sym == SDLK_ESCAPE)
				input.quit = true;

			else if (_event->key.keysym.sym == SDLK_SPACE)
				input.paused = !input.paused;
			else if (_event->key.keysym.sym == SDLK_r)
				input.restart = true;

			else if (_event->key.keysym.sym == SDLK_UP)
				input.direction = Direction::MOVE_UP;
			else if (_event->key.keysym.sym == SDLK_DOWN)
				input.direction = Direction::MOVE_DOWN;
			else if (_event->key.keysym.sym == SDLK_LEFT)
				input.direction = Direction::MOVE_LEFT;
			else if (_event->key.keysym.sym == SDLK_RIGHT)
				input.direction = Direction::MOVE_RIGHT;

			else if (_event->key.keysym.sym == SDLK_1)
				input.loadGuiID = 0;
			else if (_event->key.keysym.sym == SDLK_2)
				input.loadGuiID = 1;
			else if (_event->key.keysym.sym == SDLK_3)
				input.loadGuiID = 2;
		}

		if (_event->type == SDL_MOUSEMOTION) {
			_cam->processMouseMovement(_event->motion.xrel, -_event->motion.yrel);
		}
	}

	const Uint8 * keystates = SDL_GetKeyboardState(NULL);

	bool isRun = false;
	if (keystates[SDL_SCANCODE_LSHIFT])
		isRun = true;
	if (keystates[SDL_SCANCODE_W])
		_cam->processKeyboard(CamMovement::Forward, dtTime, isRun);
	if (keystates[SDL_SCANCODE_S])
		_cam->processKeyboard(CamMovement::Backward, dtTime, isRun);
	if (keystates[SDL_SCANCODE_A])
		_cam->processKeyboard(CamMovement::Left, dtTime, isRun);
	if (keystates[SDL_SCANCODE_D])
		_cam->processKeyboard(CamMovement::Right, dtTime, isRun);
	if (keystates[SDL_SCANCODE_E])
		_cam->processKeyboard(CamMovement::Up, dtTime, isRun);
	if (keystates[SDL_SCANCODE_Q])
		_cam->processKeyboard(CamMovement::Down, dtTime, isRun);
}

bool NibblerOpenGL::draw(std::deque<Vec2> & snake, std::deque<Vec2> & food) {
    glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, _gameInfo->width, _gameInfo->height);
    glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);

	_cubeShader->use();
	_cubeShader->setMat4("view", _cam->getViewMatrix());
	_cubeShader->setVec3("viewPos", _cam->pos);
	glBindVertexArray(_cubeShaderVAO);

	// std::cout << glm::to_string(_cam->getViewMatrix()) << std::endl;


	glm::mat4 model(1.0);
	glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);

	// draw board
	for (int i = 0; i < _gameInfo->boardSize; i++) {
		for (int j = 0; j < _gameInfo->boardSize; j++) {
			pos.x = i;
			pos.z = j;
			uint32_t color = ((i + j) & 1) ? SQUARE_COLOR_1 : SQUARE_COLOR_2;
			model = glm::translate(glm::mat4(1.0), pos);
			_cubeShader->setVec4("color", TO_OPENGL_COLOR(color));
			_cubeShader->setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	// draw snake
	pos.y = 1;
	int		i = 0;
	float	max = (snake.size() == 1) ? 1 : snake.size() - 1;
	for (auto it = snake.begin(); it != snake.end(); it++) {
		pos.x = it->x;
		pos.z = it->y;
		uint32_t	color = mixColor(SNAKE_COLOR_1, SNAKE_COLOR_2, i / max);
		model = glm::translate(glm::mat4(1.0), pos);
		_cubeShader->setVec4("color", TO_OPENGL_COLOR(color));
		_cubeShader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		i++;
	}
	// draw food
	pos.y = 1;
	for (auto it = food.begin(); it != food.end(); it++) {
		pos.x = it->x;
		pos.z = it->y;
		model = glm::translate(glm::mat4(1.0), pos);
		_cubeShader->setVec4("color", TO_OPENGL_COLOR(FOOD_COLOR));
		_cubeShader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

    SDL_GL_SwapWindow(_win);
	checkError();
	return true;
}

extern "C" {
	ANibblerGui *makeNibblerOpenGL() {
		return new NibblerOpenGL();
	}
}
