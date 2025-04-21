#include "Input.h"
#include "Helper.h"

const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
	// Directional (arrow) Keys
	{"up", SDL_SCANCODE_UP},
	{"down", SDL_SCANCODE_DOWN},
	{"right", SDL_SCANCODE_RIGHT},
	{"left", SDL_SCANCODE_LEFT},

	// Misc Keys
	{"escape", SDL_SCANCODE_ESCAPE},

	// Modifier Keys
	{"lshift", SDL_SCANCODE_LSHIFT},
	{"rshift", SDL_SCANCODE_RSHIFT},
	{"lctrl", SDL_SCANCODE_LCTRL},
	{"rctrl", SDL_SCANCODE_RCTRL},
	{"lalt", SDL_SCANCODE_LALT},
	{"ralt", SDL_SCANCODE_RALT},

	// Editing Keys
	{"tab", SDL_SCANCODE_TAB},
	{"return", SDL_SCANCODE_RETURN},
	{"enter", SDL_SCANCODE_RETURN},
	{"backspace", SDL_SCANCODE_BACKSPACE},
	{"delete", SDL_SCANCODE_DELETE},
	{"insert", SDL_SCANCODE_INSERT},

	// Character Keys
	{"space", SDL_SCANCODE_SPACE},
	{"a", SDL_SCANCODE_A},
	{"b", SDL_SCANCODE_B},
	{"c", SDL_SCANCODE_C},
	{"d", SDL_SCANCODE_D},
	{"e", SDL_SCANCODE_E},
	{"f", SDL_SCANCODE_F},
	{"g", SDL_SCANCODE_G},
	{"h", SDL_SCANCODE_H},
	{"i", SDL_SCANCODE_I},
	{"j", SDL_SCANCODE_J},
	{"k", SDL_SCANCODE_K},
	{"l", SDL_SCANCODE_L},
	{"m", SDL_SCANCODE_M},
	{"n", SDL_SCANCODE_N},
	{"o", SDL_SCANCODE_O},
	{"p", SDL_SCANCODE_P},
	{"q", SDL_SCANCODE_Q},
	{"r", SDL_SCANCODE_R},
	{"s", SDL_SCANCODE_S},
	{"t", SDL_SCANCODE_T},
	{"u", SDL_SCANCODE_U},
	{"v", SDL_SCANCODE_V},
	{"w", SDL_SCANCODE_W},
	{"x", SDL_SCANCODE_X},
	{"y", SDL_SCANCODE_Y},
	{"z", SDL_SCANCODE_Z},
	{"0", SDL_SCANCODE_0},
	{"1", SDL_SCANCODE_1},
	{"2", SDL_SCANCODE_2},
	{"3", SDL_SCANCODE_3},
	{"4", SDL_SCANCODE_4},
	{"5", SDL_SCANCODE_5},
	{"6", SDL_SCANCODE_6},
	{"7", SDL_SCANCODE_7},
	{"8", SDL_SCANCODE_8},
	{"9", SDL_SCANCODE_9},
	{"/", SDL_SCANCODE_SLASH},
	{";", SDL_SCANCODE_SEMICOLON},
	{"=", SDL_SCANCODE_EQUALS},
	{"-", SDL_SCANCODE_MINUS},
	{".", SDL_SCANCODE_PERIOD},
	{",", SDL_SCANCODE_COMMA},
	{"[", SDL_SCANCODE_LEFTBRACKET},
	{"]", SDL_SCANCODE_RIGHTBRACKET},
	{"\\", SDL_SCANCODE_BACKSLASH},
	{"'", SDL_SCANCODE_APOSTROPHE}
};

const std::unordered_map<std::string, SDL_GameControllerButton> __keycode_to_controllerbutton = {
	// Controller mappings
	{"A", SDL_CONTROLLER_BUTTON_A},
	{"B", SDL_CONTROLLER_BUTTON_B},
	{"X", SDL_CONTROLLER_BUTTON_X},
	{"Y", SDL_CONTROLLER_BUTTON_Y},
	{"Back", SDL_CONTROLLER_BUTTON_BACK},
	{"Guide", SDL_CONTROLLER_BUTTON_GUIDE},
	{"Start", SDL_CONTROLLER_BUTTON_START},
	{"LeftStick", SDL_CONTROLLER_BUTTON_LEFTSTICK},
	{"RightStick", SDL_CONTROLLER_BUTTON_RIGHTSTICK},
	{"LeftShoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
	{"RightShoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
	{"DPadUp", SDL_CONTROLLER_BUTTON_DPAD_UP},
	{"DPadDown", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
	{"DPadLeft", SDL_CONTROLLER_BUTTON_DPAD_LEFT},
	{"DPadRight", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
	{"Misc1", SDL_CONTROLLER_BUTTON_MISC1}, /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
	{"Paddle1", SDL_CONTROLLER_BUTTON_PADDLE1}, /* Xbox Elite paddle P1 (upper left, facing the back) */
	{"Paddle2", SDL_CONTROLLER_BUTTON_PADDLE2}, /* Xbox Elite paddle P3 (upper right, facing the back) */
	{"Paddle3", SDL_CONTROLLER_BUTTON_PADDLE3}, /* Xbox Elite paddle P2 (lower left, facing the back) */
	{"Paddle4", SDL_CONTROLLER_BUTTON_PADDLE4}, /* Xbox Elite paddle P4 (lower right, facing the back) */
	{"Touchpad", SDL_CONTROLLER_BUTTON_TOUCHPAD}, /* PS4/PS5 touchpad button */
};

const std::unordered_map<Uint8, SDL_GameControllerButton> uint8_to_controllerbutton = {
	// Controller mappings
	{0, SDL_CONTROLLER_BUTTON_A},
	{1, SDL_CONTROLLER_BUTTON_B},
	{2, SDL_CONTROLLER_BUTTON_X},
	{3, SDL_CONTROLLER_BUTTON_Y},
	{4, SDL_CONTROLLER_BUTTON_BACK},
	{5, SDL_CONTROLLER_BUTTON_GUIDE},
	{6, SDL_CONTROLLER_BUTTON_START},
	{7, SDL_CONTROLLER_BUTTON_LEFTSTICK},
	{8, SDL_CONTROLLER_BUTTON_RIGHTSTICK},
	{9, SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
	{10, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
	{11, SDL_CONTROLLER_BUTTON_DPAD_UP},
	{12, SDL_CONTROLLER_BUTTON_DPAD_DOWN},
	{13, SDL_CONTROLLER_BUTTON_DPAD_LEFT},
	{14, SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
	{15, SDL_CONTROLLER_BUTTON_MISC1}, /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
	{16, SDL_CONTROLLER_BUTTON_PADDLE1}, /* Xbox Elite paddle P1 (upper left, facing the back) */
	{17, SDL_CONTROLLER_BUTTON_PADDLE2}, /* Xbox Elite paddle P3 (upper right, facing the back) */
	{18, SDL_CONTROLLER_BUTTON_PADDLE3}, /* Xbox Elite paddle P2 (lower left, facing the back) */
	{19, SDL_CONTROLLER_BUTTON_PADDLE4}, /* Xbox Elite paddle P4 (lower right, facing the back) */
	{20, SDL_CONTROLLER_BUTTON_TOUCHPAD}, /* PS4/PS5 touchpad button */
};

const std::unordered_map<std::string, SDL_GameControllerAxis> __keycode_to_controlleraxis = {
	// Controller mappings
	{"LeftX", SDL_CONTROLLER_AXIS_LEFTX},
	{"LeftY", SDL_CONTROLLER_AXIS_LEFTY},
	{"RightX", SDL_CONTROLLER_AXIS_RIGHTX},
	{"RightY", SDL_CONTROLLER_AXIS_RIGHTY},
	{"TriggerLeft", SDL_CONTROLLER_AXIS_TRIGGERLEFT},
	{"TriggerRight", SDL_CONTROLLER_AXIS_TRIGGERRIGHT}
};

const std::unordered_map<Uint8, SDL_GameControllerAxis> uint8_to_controlleraxis = {
	// Controller mappings
	{0, SDL_CONTROLLER_AXIS_LEFTX},
	{1, SDL_CONTROLLER_AXIS_LEFTY},
	{2, SDL_CONTROLLER_AXIS_RIGHTX},
	{3, SDL_CONTROLLER_AXIS_RIGHTY},
	{4, SDL_CONTROLLER_AXIS_TRIGGERLEFT},
	{5, SDL_CONTROLLER_AXIS_TRIGGERRIGHT}
};

void Input::Init()
{
	SDL_Init(SDL_INIT_GAMECONTROLLER);
	int nJoysticks = SDL_NumJoysticks();
	int nGameControllers = 0;
	for (int i = 0; i < nJoysticks; i++) {
		if (SDL_IsGameController(i)) {
			SDL_GameController* temp;
			temp = SDL_GameControllerOpen(i);
			controller_list.push_back(temp);
			nGameControllers++;
			controller_button_states.push_back(std::unordered_map<SDL_GameControllerButton, INPUT_STATE>());
			just_became_down_controller_buttons.push_back(std::vector<SDL_GameControllerButton>());
			just_became_up_controller_buttons.push_back(std::vector<SDL_GameControllerButton>());
			controller_axis_states.push_back(std::unordered_map<SDL_GameControllerAxis, Sint16>());
			controller_touchpad_states.push_back(glm::vec2());
		}
	}
	numControllers = nGameControllers;
	//std::cout << numControllers << std::endl;
	for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++) {
		keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
	}
	for (int i = 0; i < numControllers; i++) {
		for (int code = SDL_CONTROLLER_BUTTON_A; code < SDL_CONTROLLER_BUTTON_MAX; code++) {
			controller_button_states[i][static_cast<SDL_GameControllerButton>(code)] = INPUT_STATE_UP;
		}
	}
}

void Input::ProcessEvent(const SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_scancodes.push_back(e.key.keysym.scancode);
	}
	else if (e.type == SDL_KEYUP) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_scancodes.push_back(e.key.keysym.scancode);
	}
	else if (e.type == SDL_MOUSEMOTION) {
		Input::mouse_position = glm::vec2(e.motion.x, e.motion.y);
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN) {
		mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_buttons.push_back(e.button.button);
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
		mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_buttons.push_back(e.button.button);
	}
	else if (e.type == SDL_MOUSEWHEEL) {
		mouse_scroll_this_frame += e.wheel.preciseY;
	}
	else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
		int controllerIndex = getControllerIndexFromID(e.cbutton.which);
		if (controllerIndex != -1) {
			auto it = uint8_to_controllerbutton.find(e.cbutton.button);
			if (it != uint8_to_controllerbutton.end())
			{
				controller_button_states[controllerIndex][it->second] = INPUT_STATE_JUST_BECAME_DOWN;
				just_became_down_controller_buttons[controllerIndex].push_back(it->second);
			}
		}
	}
	else if (e.type == SDL_CONTROLLERBUTTONUP) {
		int controllerIndex = getControllerIndexFromID(e.cbutton.which);
		if (controllerIndex != -1) {
			auto it = uint8_to_controllerbutton.find(e.cbutton.button);
			if (it != uint8_to_controllerbutton.end())
			{
				controller_button_states[controllerIndex][it->second] = INPUT_STATE_JUST_BECAME_UP;
				just_became_up_controller_buttons[controllerIndex].push_back(it->second);
			}
		}
		
	}
	else if (e.type == SDL_CONTROLLERAXISMOTION) {
		int controllerIndex = getControllerIndexFromID(e.caxis.which);
		if (controllerIndex != -1) {
			auto it = uint8_to_controlleraxis.find(e.caxis.axis);
			if (it != uint8_to_controlleraxis.end())
			{
				controller_axis_states[controllerIndex][it->second] = e.caxis.value;
			}
		}
	}
	else if (e.type == SDL_CONTROLLERTOUCHPADDOWN) {
		int controllerIndex = getControllerIndexFromID(e.cbutton.which);
		if (controllerIndex != -1) {
			controller_button_states[controllerIndex][SDL_CONTROLLER_BUTTON_TOUCHPAD] = INPUT_STATE_JUST_BECAME_DOWN;
			just_became_down_controller_buttons[controllerIndex].push_back(SDL_CONTROLLER_BUTTON_TOUCHPAD);
			controller_touchpad_states[controllerIndex] = glm::vec2(e.ctouchpad.x, e.ctouchpad.y);
		}
	}
	else if (e.type == SDL_CONTROLLERTOUCHPADUP) {
		int controllerIndex = getControllerIndexFromID(e.cbutton.which);
		if (controllerIndex != -1) {
			controller_button_states[controllerIndex][SDL_CONTROLLER_BUTTON_TOUCHPAD] = INPUT_STATE_JUST_BECAME_UP;
			just_became_up_controller_buttons[controllerIndex].push_back(SDL_CONTROLLER_BUTTON_TOUCHPAD);
			controller_touchpad_states[controllerIndex] = glm::vec2(e.ctouchpad.x, e.ctouchpad.y);
		}
	}
	else if (e.type == SDL_CONTROLLERTOUCHPADMOTION) {
		int controllerIndex = getControllerIndexFromID(e.ctouchpad.which);
		if (controllerIndex != -1) {
			controller_touchpad_states[controllerIndex] = glm::vec2(e.ctouchpad.x, e.ctouchpad.y);
		}
	}
}

void Input::LateUpdate()
{
	for (const SDL_Scancode& code : just_became_down_scancodes) {
		keyboard_states[code] = INPUT_STATE_DOWN;
	}
	just_became_down_scancodes.clear();

	for (const SDL_Scancode& code : just_became_up_scancodes) {
		keyboard_states[code] = INPUT_STATE_UP;
	}
	just_became_up_scancodes.clear();
	for (const int button : just_became_down_buttons) {
		mouse_button_states[button] = INPUT_STATE_DOWN;
	}
	just_became_down_buttons.clear();

	for (const int button : just_became_up_buttons) {
		mouse_button_states[button] = INPUT_STATE_UP;
	}
	just_became_up_buttons.clear();
	mouse_scroll_this_frame = 0;

	for (int i = 0; i < numControllers; i++) {
		//std::cout << "Just: " << just_became_down_controller_buttons[i].size() << std::endl;
		for (const auto button : just_became_down_controller_buttons[i]) {
			controller_button_states[i][button] = INPUT_STATE_DOWN;
		}
		just_became_down_controller_buttons[i].clear();
		for (const auto button : just_became_up_controller_buttons[i]) {
			controller_button_states[i][button] = INPUT_STATE_UP;
		}
		just_became_up_controller_buttons[i].clear();
	}
}

bool Input::GetKey(SDL_Scancode keycode)
{
	return (keyboard_states[keycode] == INPUT_STATE_DOWN || keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyS(std::string keycode)
{
	auto it = __keycode_to_scancode.find(keycode);
	if (it != __keycode_to_scancode.end()) 
	{
		return (keyboard_states[it->second] == INPUT_STATE_DOWN || keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_DOWN);
	}
	return false;
}

bool Input::GetKeyDown(SDL_Scancode keycode)
{
	return (keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyDownS(std::string keycode)
{
	auto it = __keycode_to_scancode.find(keycode);
	if (it != __keycode_to_scancode.end())
	{
		return (keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_DOWN);
	}
	return false;
}

bool Input::GetKeyUp(SDL_Scancode keycode)
{
	return (keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_UP);
}

bool Input::GetKeyUpS(std::string keycode)
{
	auto it = __keycode_to_scancode.find(keycode);
	if (it != __keycode_to_scancode.end())
	{
		return (keyboard_states[it->second] == INPUT_STATE_JUST_BECAME_UP);
	}
	return false;
}

void Input::HideCursor()
{
	SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor()
{
	SDL_ShowCursor(SDL_ENABLE);
}

glm::vec2 Input::GetMousePosition()
{
	return mouse_position;
}

bool Input::GetMouseButton(int button)
{
	return (mouse_button_states[button] == INPUT_STATE_DOWN || mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetMouseButtonDown(int button)
{
	return (mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetMouseButtonUp(int button)
{
	return (mouse_button_states[button] == INPUT_STATE_JUST_BECAME_UP);
}

float Input::GetMouseScrollDelta()
{
	return mouse_scroll_this_frame;
}

int Input::GetNumControllers()
{
	return numControllers;
}

bool Input::GetControllerButton(SDL_GameControllerButton button, int cID)
{
	if (!ControllerConnected(cID)) return false;
	return controller_button_states[cID][button] == INPUT_STATE_DOWN || controller_button_states[cID][button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetControllerButtonS(std::string button, int cID)
{
	if (!ControllerConnected(cID)) return false;
	auto it = __keycode_to_controllerbutton.find(button);
	if (it != __keycode_to_controllerbutton.end())
	{
		return (controller_button_states[cID][it->second] == INPUT_STATE_JUST_BECAME_DOWN) || controller_button_states[cID][it->second] == INPUT_STATE_DOWN;
	}
	return false;
}

bool Input::GetControllerButtonDown(SDL_GameControllerButton button, int cID)
{
	if (!ControllerConnected(cID)) return false;
	return controller_button_states[cID][button] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetControllerButtonDownS(std::string button, int cID)
{
	if (!ControllerConnected(cID)) return false;
	auto it = __keycode_to_controllerbutton.find(button);
	if (it != __keycode_to_controllerbutton.end())
	{
		return (controller_button_states[cID][it->second] == INPUT_STATE_JUST_BECAME_DOWN);
	}
	return false;
}

bool Input::GetControllerButtonUp(SDL_GameControllerButton button, int cID)
{
	if (!ControllerConnected(cID)) return false;
	return just_became_up_controller_buttons[cID][button] == INPUT_STATE_JUST_BECAME_UP;
}

bool Input::GetControllerButtonUpS(std::string button, int cID)
{
	if (!ControllerConnected(cID)) return false;
	auto it = __keycode_to_controllerbutton.find(button);
	if (it != __keycode_to_controllerbutton.end())
	{
		return (controller_button_states[cID][it->second] == INPUT_STATE_JUST_BECAME_DOWN);
	}
	return false;
}

int Input::GetControllerAxis(SDL_GameControllerAxis axis, int cID)
{
	if (!ControllerConnected(cID)) return false;
	return controller_axis_states[cID][axis];
}

int Input::GetControllerAxisS(std::string axis, int cID)
{
	if (!ControllerConnected(cID)) return false;
	auto it = __keycode_to_controlleraxis.find(axis);
	if (it != __keycode_to_controlleraxis.end())
	{
		return (controller_axis_states[cID][it->second]);
	}
	return 0;
}

glm::vec2 Input::GetControllerTouchpad(int cID) //currently doesnt work?
{
	if (!ControllerConnected(cID)) return glm::vec2();

	SDL_GameController* temp = controller_list[cID];
	Uint8 state = 0;
	float x = 0;
	float y = 0;
	float pressure = 0;
	//std::cout << SDL_GameControllerGetNumTouchpadFingers(temp, 0) << std::endl;
	//std::cout << SDL_GameControllerGetTouchpadFinger(temp, 0, 0, &state, &x, &y, &pressure) << std::endl;
	SDL_GameControllerGetTouchpadFinger(temp, 0, 0, &state, &x, &y, &pressure);
	//std::cout << pressure << std::endl;
	return glm::vec2(x, y);

	//return controller_touchpad_states[cID];
}

void Input::PrintControllerMapping(int cID)
{
	if (!ControllerConnected(cID)) {
		std::cout << "out of bounds controller" << std::endl;
		return;
	}
	std::cout << SDL_GameControllerMapping(controller_list[cID]) << std::endl;
}

bool Input::ControllerConnected(int cID)
{
	if (controller_list.size() <= cID) return false;
	return SDL_GameControllerGetAttached(controller_list[cID]);
}

void Input::CheckControllers()
{
	int nJoysticks = SDL_NumJoysticks();
	int nGameControllers = 0;
	for (int i = 0; i < nJoysticks; i++) {
		if (SDL_IsGameController(i)) {
			nGameControllers++;
		}
	}
	if (nGameControllers < numControllers) {
		// Controller has been disconnected
		for (int i = 0; i < controller_list.size(); i++) {
			if (!SDL_GameControllerGetAttached(controller_list[i]) && controller_list[i] != nullptr) {
				//std::cout << "Disconnected controller " << i << std::endl;
				SDL_GameControllerClose(controller_list[i]);
				controller_list[i] = nullptr;
				free_controller_slots.push(i);
				for (int code = SDL_CONTROLLER_BUTTON_A; code < SDL_CONTROLLER_BUTTON_MAX; code++) {
					controller_button_states[i][static_cast<SDL_GameControllerButton>(code)] = INPUT_STATE_UP;
				}
			}
		}
	}
	else if (nGameControllers > numControllers) {
		// Controller has been connected
		for (int i = 0; i < nJoysticks; i++) {
			if (SDL_IsGameController(i)) {
				// new controller?
				SDL_GameController* temp = SDL_GameControllerOpen(i);
				bool other = false;
				for (int i = 0; i < controller_list.size(); i++) {
					if (controller_list[i] == temp) other = true;
				}
				if (other) continue;

				//std::cout << "Connected controller " << i << std::endl;

				if (free_controller_slots.empty()) {
					//std::cout << "New slot" << std::endl;
					controller_list.push_back(temp);
					controller_button_states.push_back(std::unordered_map<SDL_GameControllerButton, INPUT_STATE>());
					just_became_down_controller_buttons.push_back(std::vector<SDL_GameControllerButton>());
					just_became_up_controller_buttons.push_back(std::vector<SDL_GameControllerButton>());
					controller_axis_states.push_back(std::unordered_map<SDL_GameControllerAxis, Sint16>());
					controller_touchpad_states.push_back(glm::vec2());
					for (int code = SDL_CONTROLLER_BUTTON_A; code < SDL_CONTROLLER_BUTTON_MAX; code++) {
						controller_button_states[controller_button_states.size() - 1][static_cast<SDL_GameControllerButton>(code)] = INPUT_STATE_UP;
					}
				}
				else {
					int index = free_controller_slots.top();
					free_controller_slots.pop();
					//std::cout << "Freed slot " << index << std::endl;
					controller_list[index] = temp;
					controller_button_states[index] = std::unordered_map<SDL_GameControllerButton, INPUT_STATE>();
					just_became_down_controller_buttons[index] = std::vector<SDL_GameControllerButton>();
					just_became_up_controller_buttons[index] = std::vector<SDL_GameControllerButton>();
					controller_axis_states[index] = std::unordered_map<SDL_GameControllerAxis, Sint16>();
					controller_touchpad_states[index] = glm::vec2();
					for (int code = SDL_CONTROLLER_BUTTON_A; code < SDL_CONTROLLER_BUTTON_MAX; code++) {
						controller_button_states[index][static_cast<SDL_GameControllerButton>(code)] = INPUT_STATE_UP;
					}
				}
			}
		}
	}
	numControllers = nGameControllers;
}

int Input::getControllerIndexFromID(SDL_JoystickID id)
{
	SDL_GameController* temp = SDL_GameControllerFromInstanceID(id);
	if (temp == nullptr) return -1;
	for (int i = 0; i < controller_list.size(); i++) {
		if (controller_list[i] == temp) return i;
	}
	return -1;
}
