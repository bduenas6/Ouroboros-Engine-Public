#ifndef INPUT_H
#define INPUT_H

#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

class Input
{
public:
	static void Init(); // Call before main loop begins.
	static void ProcessEvent(const SDL_Event& e); // Call every frame at start of event loop.
	static void LateUpdate(); // Call at frame end.

	static bool GetKey(SDL_Scancode keycode);
	static bool GetKeyS(std::string keycode);
	static bool GetKeyDown(SDL_Scancode keycode);
	static bool GetKeyDownS(std::string keycode);
	static bool GetKeyUp(SDL_Scancode keycode);
	static bool GetKeyUpS(std::string keycode);
	static void HideCursor();
	static void ShowCursor();

	static glm::vec2 GetMousePosition();

	static bool GetMouseButton(int button);
	static bool GetMouseButtonDown(int button);
	static bool GetMouseButtonUp(int button);
	static float GetMouseScrollDelta();

	static int GetNumControllers();
	static bool GetControllerButton(SDL_GameControllerButton button, int cID);
	static bool GetControllerButtonS(std::string button, int cID);
	static bool GetControllerButtonDown(SDL_GameControllerButton button, int cID);
	static bool GetControllerButtonDownS(std::string button, int cID);
	static bool GetControllerButtonUp(SDL_GameControllerButton button, int cID);
	static bool GetControllerButtonUpS(std::string button, int cID);
	static int GetControllerAxis(SDL_GameControllerAxis axis, int cID);
	static int GetControllerAxisS(std::string axis, int cID);
	static glm::vec2 GetControllerTouchpad(int cID);
	static void PrintControllerMapping(int cID);
	static bool ControllerConnected(int cID);
	static void CheckControllers();

private:
	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static inline std::vector<SDL_Scancode> just_became_down_scancodes;
	static inline std::vector<SDL_Scancode> just_became_up_scancodes;

	static inline glm::vec2 mouse_position;
	static inline std::unordered_map<int, INPUT_STATE> mouse_button_states;
	static inline std::vector<int> just_became_down_buttons;
	static inline std::vector<int> just_became_up_buttons;

	static inline float mouse_scroll_this_frame = 0;
	
	static inline int numControllers;
	static inline std::vector<std::unordered_map<SDL_GameControllerButton, INPUT_STATE>> controller_button_states;
	static inline std::vector<std::vector<SDL_GameControllerButton>> just_became_down_controller_buttons;
	static inline std::vector<std::vector<SDL_GameControllerButton>> just_became_up_controller_buttons;
	static inline std::vector<std::unordered_map<SDL_GameControllerAxis, Sint16>> controller_axis_states;
	static inline std::vector<glm::vec2> controller_touchpad_states;
	static inline std::vector<SDL_GameController*> controller_list;
	static int getControllerIndexFromID(SDL_JoystickID id);
	static inline std::priority_queue<int, std::vector<int>, std::greater<int>> free_controller_slots;
};

#endif