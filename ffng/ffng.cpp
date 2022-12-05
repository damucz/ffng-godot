#include "ffng.h"
#include "Application.h"
#include "TimerAgent.h"
#include "GodotSDL.h"
#include <core/os/input.h>
#include <core/os/keyboard.h>
#include <core/error_macros.h>
#include <scene/main/viewport.h>

void ffng::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_input"), &ffng::_input);
	ClassDB::bind_method(D_METHOD("_frame_post_draw"), &ffng::_frame_post_draw);
	ClassDB::bind_method(D_METHOD("size_changed"), &ffng::size_changed);
	//ClassDB::bind_method(D_METHOD("_unhandled_input"), &ffng::_unhandled_input);
}

/*
void ffng::_register_methods() {
	_register_method("_input", &ffng::_input);
}
*/

void ffng::_frame_post_draw() {
	SDL_Purge();
}

void ffng::size_changed() {
	SDL_FitScreen();
}

ffng::ffng() {
}

ffng::~ffng() {
}

void ffng::_input(const Ref<InputEvent> event) 
{
	if (event->is_class("InputEventMouseButton")) {
		Ref<InputEventMouseButton> e = (Ref<InputEventMouseButton>)event;

		SDL_Event sdlevent;
		sdlevent.type = e->is_pressed() ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
		switch (e->get_button_index()) {
			case 1: sdlevent.button.button = SDL_BUTTON_LEFT; break;
			case 2: sdlevent.button.button = SDL_BUTTON_RIGHT; break;
			case 3: sdlevent.button.button = SDL_BUTTON_MIDDLE; break;
		}
		Vector2 pos = get_local_mouse_position();
		sdlevent.button.x = pos.x;
		sdlevent.button.y = pos.y;
		SDL_PushEvent(sdlevent);
	}
	/*
	else if (event->is_class("InputEventKey")) {
		Ref<InputEventKey> e = (Ref<InputEventKey>)event;

		SDLKey k = 255;
		switch (e->get_scancode()) {
			case KEY_LEFT: k = SDLK_LEFT; break;
			case KEY_RIGHT: k = SDLK_RIGHT; break;
			case KEY_UP: k = SDLK_UP; break;
			case KEY_DOWN: k = SDLK_DOWN; break;
			case KEY_I: k = SDLK_i; break;
			case KEY_J: k = SDLK_j; break;
			case KEY_K: k = SDLK_k; break;
			case KEY_L: k = SDLK_l; break;
			case KEY_W: k = SDLK_w; break;
			case KEY_A: k = SDLK_a; break;
			case KEY_S: k = SDLK_s; break;
			case KEY_D: k = SDLK_d; break;
			case KEY_SPACE: k = SDLK_SPACE; break;				// switch fish
			case KEY_F1: k = SDLK_F1; break;					// help
			case KEY_F2: k = SDLK_F2; break;					// save
			case KEY_F3: k = SDLK_F3; break;					// load
			case KEY_F5: k = SDLK_F5; break;					// show step number
			case KEY_F6: k = SDLK_F6; break;					// subtitles
			case KEY_F10: k = SDLK_F10; break;					// menu
			case KEY_BACKSPACE: k = SDLK_BACKSPACE; break;		// restart
			case KEY_MINUS: k = SDLK_MINUS; break;				// undo
			case KEY_KP_SUBTRACT: k = SDLK_KP_MINUS; break;		// undo
			case KEY_PLUS: k = SDLK_PLUS; break;				// redo
			case KEY_KP_ADD: k = SDLK_KP_PLUS; break;			// redo
			case KEY_EQUAL: k = SDLK_EQUALS; break;				// redo
			case KEY_TAB: k = SDLK_TAB; break;					// next level
			case KEY_ENTER: k = SDLK_RETURN; break;				// play selected
			case KEY_ESCAPE: k = SDLK_ESCAPE; break;			// quit
		}

		if (k != 255) {
			SDL_Event sdlevent;
			sdlevent.type = e->is_pressed() ? SDL_KEYDOWN : SDL_KEYUP;
			sdlevent.key.keysym.mod = KMOD_NONE;
			sdlevent.key.keysym.sym = k;
			sdlevent.key.keysym.unicode = 0;
			SDL_PushEvent(sdlevent);
		}
	}
	*/
}

/*
void ffng::_unhandled_input(const Ref<InputEvent> event) {
	WARN_PRINT("xyz");
	Input* _input = Input::get_singleton();
}
*/

void ffng::_notification(int what) {
	switch (what) {
		case NOTIFICATION_READY: {
			set_process_internal(true);
			set_physics_process_internal(false);
			set_process_input(true);
			set_process_unhandled_input(true);
			break;
		}
		case NOTIFICATION_ENTER_TREE: {
			VS::get_singleton()->connect("frame_post_draw", this, "_frame_post_draw");
			get_tree()->get_root()->connect("size_changed", this, "size_changed");
			SDL_node = this;
    		app = memnew(Application);
		    app->init(0, nullptr);
			break;
		}
		case NOTIFICATION_EXIT_TREE: {
			app->shutdown();
    		memdelete(app);
			VS::get_singleton()->disconnect("frame_post_draw", this, "_frame_post_draw");
			get_tree()->get_root()->disconnect("size_changed", this, "size_changed");
			break;
		}
		case NOTIFICATION_INTERNAL_PROCESS: {
            if (!is_visible_in_tree()) return;

			auto* input = Input::get_singleton();
			for (int i = 0; i < SDLK_LAST; ++i)
			{
				static KeyList keys[SDLK_LAST] = {
					KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,
					KEY_I, KEY_J, KEY_K, KEY_L,
					KEY_W, KEY_A, KEY_S, KEY_D,
					KEY_SPACE, KEY_F1, KEY_F2, KEY_F3, KEY_F5, KEY_F6, KEY_F10,
					KEY_BACKSPACE, KEY_MINUS, KEY_KP_SUBTRACT, KEY_PLUS, KEY_KP_ADD,
					KEY_EQUAL, KEY_TAB, KEY_ENTER, KEY_ESCAPE,
				};

				bool pressed = input->is_key_pressed(keys[i]);
				if (pressed != (SDL_GetKeyState(nullptr)[i] == 1)) {
					SDL_Event sdlevent;
					sdlevent.type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
					sdlevent.key.keysym.mod = KMOD_NONE;
					sdlevent.key.keysym.sym = i;
					sdlevent.key.keysym.unicode = 0;
					SDL_PushEvent(sdlevent);
				}
				SDL_SetKeyPressed(i, pressed);
			}

            if (TimerAgent::agent()->timeToRun()) {
				Vector2 pos = get_local_mouse_position();
				Uint8 pressed = 0;
				if (input->is_mouse_button_pressed(1)) pressed |= SDL_BUTTON_LEFT;
				if (input->is_mouse_button_pressed(2)) pressed |= SDL_BUTTON_RIGHT;
				if (input->is_mouse_button_pressed(3)) pressed |= SDL_BUTTON_MIDDLE;
				SDL_SetMouseState((int)pos.x, (int)pos.y, pressed);

				update();
			}
			break;
		}
		case NOTIFICATION_DRAW: {
			app->run();
			break;
		}
		default:
			break;
	}
}

