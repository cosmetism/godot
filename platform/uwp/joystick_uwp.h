/*************************************************************************/
/*  joystick_uwp.h                                                       */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#ifndef JOYSTICK_UWP_H
#define JOYSTICK_UWP_H

#include "main/input_default.h"

ref class JoystickUWP sealed {

internal:

	void register_events();
	uint32_t process_controllers(uint32_t p_last_id);

	JoystickUWP();
	JoystickUWP(InputDefault* p_input);

private:

	enum {
		MAX_CONTROLLERS = 4,
	};

	enum ControllerType {
		GAMEPAD_CONTROLLER,
		ARCADE_STICK_CONTROLLER,
		RACING_WHEEL_CONTROLLER,
	};

	struct ControllerDevice {

		Windows::Gaming::Input::IGameController^ controller_reference;

		int id;
		bool connected;
		ControllerType type;

		ControllerDevice() {
			id = -1;
			connected = false;
			type = ControllerType::GAMEPAD_CONTROLLER;
		}
	};

	ControllerDevice controllers[MAX_CONTROLLERS];

	InputDefault* input;

	void OnGamepadAdded(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ value);
	void OnGamepadRemoved(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ value);

	InputDefault::JoyAxis axis_correct(double p_val, bool p_negate = false, bool p_trigger = false) const;
};

#endif
