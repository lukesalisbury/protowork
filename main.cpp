#include <sys/stat.h>

#include "ourtypes.hpp"
#include "gametypes.hpp"
#include "texthandling.hpp"

#include "resources/joystick_generic.hpp"
#include "resources/joystick_xbox360.hpp"
#include "resources/keyboard_basic.hpp"
#include "resources/mouse_generic.hpp"
#include "resources/texture_blank.hpp"
#include "resources/font_bit.hpp"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ELIX_HASHMAP_IMPLEMENTATION
#include "elix_hashmap.hpp"

struct PlatformOne {
};
struct CodeOne {
	void * handle = nullptr;
	char handle_filename[20] = {0};
	int32_t counter = 0;
	struct stat code_handle_time;
	struct stat code_handle_check;

	uint8_t (* Init)( AppOne * app, GameOne * game, uint8_t state) = nullptr;
	uint8_t (* Destory)( AppOne * app, GameOne * game, uint8_t state) = nullptr;
	uint8_t (* Frame)( AppOne * app, GameOne * game, uint8_t state) = nullptr;
	uint8_t (* Reload)( AppOne * app, GameOne * game, uint8_t state) = nullptr; 
};

ControllerPad default_keyboard = { 
	"Keyboard", 
	{  
		{ KEYBOARD, 0, SDL_SCANCODE_A, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_S, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_D, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_Q, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_W, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_E, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_RIGHT, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_LEFT, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_DOWN, 0, 0 },
		{ KEYBOARD, 0, SDL_SCANCODE_UP, 0, 0 }
	},
	{	KEYBOARD, 0, {SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN}, {0,0,0}	},
	{	NOINPUT, 0, {SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN}, {0,0,0} }
};

ControllerPad default_gamepad = { 
	"joystick", 
	{  
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_A, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_B, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_X, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_Y, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_DPAD_LEFT, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_DPAD_DOWN, 0 },
		{ CONTROLBUTTON, 0, SDL_CONTROLLER_BUTTON_DPAD_UP, 0 }
	},
	{	CONTROLAXIS, 0, {-SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTX, -SDL_CONTROLLER_AXIS_LEFTY, SDL_CONTROLLER_AXIS_LEFTY, -SDL_CONTROLLER_AXIS_TRIGGERLEFT, SDL_CONTROLLER_AXIS_TRIGGERLEFT}, {0,0,0}	},
	{	CONTROLAXIS, 0, {SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY, SDL_CONTROLLER_AXIS_RIGHTY, SDL_CONTROLLER_AXIS_TRIGGERRIGHT, SDL_CONTROLLER_AXIS_TRIGGERRIGHT}, {0,0,0} }

};

void DrawTexture( AppOne * app, SDL_Texture * texture, SDL_Rect * src, SDL_Rect world_location, SDL_Color colour, uint8_t layer ) {
	SDL_Texture * t = !texture ? app->texture_blank : texture;

	if ( layer ) {
		world_location.x -= app->camera.x;
		world_location.y -= app->camera.y;
	}

	SDL_SetTextureColorMod(t, colour.r, colour.g, colour.b);
	SDL_SetTextureAlphaMod(t, colour.a);
	SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
	if ( SDL_RenderCopy(app->renderer, t, src, &world_location) == -1) {
		SDL_Log("DrawTextSimple: RenderCopy: %s", SDL_GetError());
	}

}

void DrawTextSimple(AppOne *app, FontTexture * font, const char * string, SDL_Rect position, float scale, uint8_t layer ) {
	uint8_t * object = (uint8_t*)string;
	uint8_t print = false;

	uint32_t line_padding = font->padding[1];
	SDL_Texture * selected_texture = nullptr;
	SDL_Rect * srcarea = nullptr;
	SDL_Color colour = {255,255,255,255};

	SDL_Point area = {position.x, position.y};
	SDL_Rect dimensions = {0, 0, 0, 0};

	SDL_Rect textarea = {position.x, position.y, 0, 0};
	SDL_Rect shadowarea = {position.x, position.y, 0, 0};
	SDL_Rect shadowdimensions = {0, 0, 0, 0};

	uint32_t cchar;

	while ( (cchar = NextTextChar(object)) > 0 ) {
		if (cchar == '\n' || cchar == '\r') {
			area.y += font->padding[1];
			area.x = position.x;
			colour = {255,255,255,255};
			print = false;
		} else if ( cchar <= 32 ) {
			area.x += font->padding[0];
			colour = {255,255,255,255};
			print = false;
		} else if ( cchar == 0xA7 ) {
			uint32_t nchar = NextTextChar(object);
			if ( nchar != 0xA7 ) {
				GetTextColour(nchar, colour);
			} else {
				cchar = nchar;
			}
		} else if ( cchar >= 0x2776 && cchar <= 0x27BF ) {
				//Button
		} else if ( cchar >= 0x25B2 && cchar <= 0x25D3 ) {
			  //Axis
		} else if ( cchar >= 0x24CD && cchar <= 0x24CE ) {
				//Pointer
		} else {
			print = true;
		}

	if ( print ) {
				selected_texture = font->base_texture;
				if ( cchar >= 32 && cchar <= 128 ) {
					srcarea = &font->offset[cchar-32];
				} else if ( cchar  == 0x2026 ) {
					srcarea = &font->offset[97]; //…
				} else if ( cchar >= 0x2776 && cchar <= 0x27BF ) {
					if ( app->control_default->button[cchar - 0x2776].sprite.id ) {
						selected_texture = (SDL_Texture*)elix_hashmap_value_hash(&app->textures, app->control_default->button[cchar - 0x2776].sprite.id);
						srcarea = &app->control_default->button[cchar - 0x2776].sprite.offset;
					} else {
						srcarea = &font->offset[96]; // �
					}
				} else if ( cchar >= 0x25B2 && cchar <= 0x25D3 ) {
					uint8_t axis = 0;
					if ( cchar == 0x25C0 ) // U+25C0	◀	x-
						axis = 0;
					else if ( cchar == 0x25B6 ) // U+25B6	▶	x+
						axis = 1;
					else if ( cchar == 0x25B2 ) // U+25B2	▲	y-
						axis = 2;
					else if ( cchar == 0x25BC ) // U+25BC	▼	y+
						axis = 3;
					else if ( cchar == 0x25D0 ) // U+25D0	◐	z-
						axis = 4;
					else if ( cchar == 0x25D1 ) // U+25D1	◑	z+
						axis = 5;
					else if ( cchar == 0x25C1 ) // U+25C1	◀	x-
						axis = 6;
					else if ( cchar == 0x25B7 ) // U+25B7	▶	x+
						axis = 7;
					else if ( cchar == 0x25B3 ) // U+25B3	▲	y-
						axis = 8;
					else if ( cchar == 0x25BD ) // U+25BD	▼	y+
						axis = 9;
					else if ( cchar == 0x25D2 ) // U+25D2	◐	z-
						axis = 10;
					else if ( cchar == 0x25D3 ) // U+25D3	◓	z+
						axis = 11;
					if (app->control_default->left_stick.sprite[axis%6].id ) {
						selected_texture = (SDL_Texture*)elix_hashmap_value_hash(&app->textures, app->control_default->left_stick.sprite[axis%6].id);
						srcarea = &app->control_default->left_stick.sprite[axis%6].offset;
					} else {
						srcarea = &font->offset[96]; // �
					}
				} else if ( cchar >= 0x24CD && cchar <= 0x24CE ) {
					srcarea = &font->offset[96]; // �
				} else {
					srcarea = &font->offset[96]; // �
				}

				if ( srcarea ) {
					textarea.x = area.x;
					textarea.y = area.y - ( srcarea->h - font->padding[1]);
					textarea.w = shadowarea.w = srcarea->w;
					textarea.h = shadowarea.h = srcarea->h;

					shadowarea.x = textarea.x + 1;
					shadowarea.y = textarea.y + 1;


					DrawTexture(app, selected_texture, srcarea, shadowarea, {14,14,14,255}, layer );
					DrawTexture(app, selected_texture, srcarea, textarea, colour, layer );

					line_padding = (srcarea->h > line_padding ? srcarea->h + 2 : line_padding);
					area.x += srcarea->w + 1;
			}
		}
	}
}

void DrawRenderItem( AppOne * app, RenderItem & item) {
	SDL_Texture * texture = (!item.texture ? app->texture_blank : item.texture);

	SDL_SetTextureColorMod(texture, item.colour.r, item.colour.g, item.colour.b);
	SDL_SetTextureAlphaMod(texture, item.colour.a);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	if ( SDL_RenderCopyF(app->renderer, texture, item.texture_area, &item.position) == -1) {
		SDL_Log("DrawTextSimple: RenderCopy: %s", SDL_GetError());
	}
}

void PlayWavAudio( AppOne * app, wav_audio & source ) {
	if ( SDL_QueueAudio(app->audio_device, source.buffer, source.length) != 0 ) {
		SDL_Log("SDL_QueueAudio: %s", SDL_GetError());
	}
	SDL_PauseAudioDevice(app->audio_device, 0);
}

void BuildBitFont( SDL_Renderer * renderer, FontTexture * font ) {
	font->offset_count = 98;
	font->offset = new SDL_Rect[font->offset_count]();
	font->base_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB1555, SDL_TEXTUREACCESS_STATIC, 128, 56);
	font->padding[0] = font->padding[1] = 10;

	SDL_SetTextureBlendMode( font->base_texture, SDL_BLENDMODE_NONE);
	SDL_UpdateTexture( font->base_texture, nullptr, font_eightbyeight, 256);

	uint8_t x = 0, y = 0;
	for ( uint8_t c = 0; c < 98; c++) {
		font->offset[c] = {x, y, 8, 8};
		x += 8;
		if ( x == 128 ) {
			x = 0;
			y += 8;
		}
	}
}

void FreeBitFont(SDL_Renderer * renderer, FontTexture * font) {
	SDL_DestroyTexture(font->base_texture);
	delete [] font->offset;
	font->offset_count = 0;
}

void LoadInputTexture( AppOne *app, const char * name, TextureSet * set, size_t set_size, uint8_t * data, size_t  data_size ) {
	char sprite_name_buffer[64];
	uint32_t id = elix_hash(name, elix_cstring_length(name));
	int32_t width, height;
	
	uint8_t * pixels = stbi_load_from_memory(data, data_size, &width, &height, nullptr, 4);
	SDL_Texture * texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
	SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_NONE);
	SDL_UpdateTexture( texture, nullptr, pixels, width*4);

	elix_hashmap_insert_hash(&app->textures, id, texture);

	for (size_t i = 0; i < set_size; i++) {
		SpriteRefTexture * sprite = new SpriteRefTexture();
		sprite->offset = { set[i].x, set[i].y, set[i].w, set[i].h};
		sprite->id = id;

		SDL_snprintf(sprite_name_buffer, 63, "%s:%s", name, set[i].name);

		elix_hashmap_insert(&app->textures_input, sprite_name_buffer, sprite);
	}
}

int32_t GetInputImage( char * name, const char * parent_name, InputDevice device, uint32_t device_number, int32_t symbol ) {
	if (device == NOINPUT ) {
		return 0;
	}
	switch (device)	{
		case KEYBOARD: {
				int32_t kecode = (int32_t)SDL_GetKeyFromScancode((SDL_Scancode)symbol);
				kecode &= ~0x40000000;
				return SDL_snprintf(name, 63, "keyboard:%d", kecode);
		}
		case MOUSEAXIS: {
			return SDL_snprintf(name, 63, "mouse:axis");
		}
		case MOUSEBUTTON: {
			return SDL_snprintf(name, 63, "mouse:button%d", symbol);
		}
		case CONTROLAXIS: {
			if ( symbol & 0x80000000 ) {
				return SDL_snprintf(name, 63, "%s:axisA-%d", parent_name, symbol&0x7FFFFFFF);
			} else {
				return SDL_snprintf(name, 63, "%s:axisA+%d", parent_name, symbol);
			}
			
		}
		case CONTROLBUTTON: {
			return SDL_snprintf(name, 63, "%s:button%d", parent_name, symbol);
		}
		case NOINPUT:
		default:
			return 0;
	}
}

void SetContolPadInputSprites(AppOne *app, ControllerPad & pad ) {
	char buffer[64];
	for (size_t i = 0; i < 10; i++) {
		if ( GetInputImage( buffer, pad.name, pad.button[i].device, pad.button[i].device_number, pad.button[i].sym ) ) {
			SpriteRefTexture * ptr = (SpriteRefTexture*)elix_hashmap_value(&app->textures_input, buffer);
			if ( ptr ) {
				pad.button[i].sprite = *ptr;
			} else {
				pad.button[i].sprite = {0};
			}
		}
	}
	for (size_t i = 0; i < 6; i++) {
		int32_t symbol = (i == 0 && pad.left_stick.device == CONTROLAXIS ? 0x80000000 : pad.left_stick.sym[i]); // Work around the fact yopu can't have -0;
		if ( GetInputImage( buffer, pad.name, pad.left_stick.device, pad.left_stick.device_number, symbol ) ) {
			SpriteRefTexture * ptr = (SpriteRefTexture*)elix_hashmap_value(&app->textures_input, buffer);
			if ( ptr ) {
				pad.left_stick.sprite[i]  = *ptr;
			} else {
				pad.button[i].sprite = {0};
			}
		}
	}
}

int16_t UpdatGetInput(AppOne * app, uint32_t & bits, size_t index, InputDevice device, uint32_t device_number, int32_t symbol) {
	if (device == NOINPUT) {
		return 0;
	}
	int16_t value = 0;
	int32_t keycount = 0;
	const uint8_t * keystate = SDL_GetKeyboardState(&keycount);
	int32_t mouse_position[2] = {0,0};
	uint32_t mouse_button = 0;
	SDL_GameController * controller;

	switch ( device ) {
		case KEYBOARD: {
			if ( keystate && symbol < keycount && symbol >= 0 ) {
				value = keystate[symbol];
			}
			break;
		}
		case MOUSEAXIS: {
			if ( symbol == 1 || symbol == 0 ) {
				mouse_button = SDL_GetMouseState(&mouse_position[0], &mouse_position[1]);
				value = mouse_position[symbol];
			}
			break;
		}
		case MOUSEBUTTON: {
			if ( symbol < 5 && symbol >= 0 ) {
				mouse_button = SDL_GetMouseState(&mouse_position[0], &mouse_position[1]);
				value = mouse_position[symbol] & SDL_BUTTON(symbol);
			}
			break;
		}
		case CONTROLAXIS: {
			SDL_GameControllerAxis s = (SDL_GameControllerAxis)(symbol & 0x7FFFFFFF);
			if ( s > SDL_CONTROLLER_AXIS_INVALID && s < SDL_CONTROLLER_AXIS_MAX ) {
				controller = SDL_GameControllerFromInstanceID(device_number);
				int16_t r = (int16_t)SDL_GameControllerGetAxis(controller, s) / 128;
				///TODO: Better Deadzone handling
				if ( s < SDL_CONTROLLER_AXIS_TRIGGERLEFT ) {
					if ( 50 > r && r > -50) // Poor Deadzone
						r = 0;
				}
				value = r;
			}
			break;
		}
		case CONTROLBUTTON: {
			SDL_GameControllerButton s = (SDL_GameControllerButton)symbol;
			controller = SDL_GameControllerFromInstanceID(device_number);
			if ( s < SDL_CONTROLLER_BUTTON_MAX ) {
				controller = SDL_GameControllerFromInstanceID(device_number);
				value = (int16_t)SDL_GameControllerGetButton(controller, s);
			}
			break;
		}
		case NOINPUT:
		default:
			return 0;
	}
	if ( value ) {
		bits |= 1 << index;
	}
	return value;
}

int8_t HandleEvents(AppOne * app, uint8_t previous_state) {
	SDL_Event event;
	uint8_t state = previous_state;
	SDL_GameController * controller = nullptr;

	app->internal_ms = GetDelta(SDL_GetPerformanceCounter() - app->realtime_counter);
	app->frame_ms = (app->internal_ms < 0 ? 0 : (app->internal_ms > 33 ? 33 : app->internal_ms)); // Make sure timestep is 30FPS, it will slow down game
	app->frame_s = (float)app->frame_ms / 1000.f;
	while( SDL_PollEvent(&event) ) {
			switch( event.type ) {
				case SDL_TEXTINPUT: {
					break;
				}
				case SDL_MOUSEMOTION: {
					app->cached_pointer[0] = event.motion.x;
					app->cached_pointer[1] = event.motion.y;
					break;
				}
				case SDL_MOUSEWHEEL: {
					if ( event.wheel.which != SDL_TOUCH_MOUSEID ) {
						
					}
					break;
				}
				case SDL_QUIT: {
					state = EXITING;
					break;
				}
				case SDL_WINDOWEVENT: {
					if ( event.window.windowID == SDL_GetWindowID( app->window ) ) {
						if ( state == NORMAL || state == PAUSED) {
							if ( event.window.event == SDL_WINDOWEVENT_MINIMIZED ) {
								state = PAUSED;
							} else if ( event.window.event == SDL_WINDOWEVENT_RESTORED ) {
								state = NORMAL;
							} else if ( event.window.event == SDL_WINDOWEVENT_MAXIMIZED ) {

							} else if ( event.window.event == SDL_WINDOWEVENT_ENTER ) {
								state = NORMAL;
							} else if ( event.window.event == SDL_WINDOWEVENT_LEAVE )	{
								state = PAUSED;
							} else if ( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
									SDL_RenderSetLogicalSize(app->renderer, event.window.data1, event.window.data2);
									SDL_RenderSetScale(app->renderer,  (float)event.window.data1 / (float)app->viewpoint.w, (float)event.window.data2 / (float)app->viewpoint.h);
							} else if ( event.window.event == SDL_WINDOWEVENT_CLOSE ) {
								state = EXITING;
							}
						}
					}
					break;
				}
				case SDL_CONTROLLERDEVICEADDED:
					if ( SDL_IsGameController(event.cdevice.which) ) {
						controller = SDL_GameControllerOpen(event.cdevice.which);
						SDL_Log("Controller Added: %s", SDL_GameControllerName(controller));
						//TODO: Better handling then this. as which refers to joystick not gamepad id
						const char * name = SDL_GameControllerName(controller);
						if ( event.cdevice.which >= 0 && event.cdevice.which < 4) {
							//TODO:Better handing of controller name
							SDL_snprintf(app->control_pads[1 + event.cdevice.which].name, 16, "%s", (name && name[0] == 'X' ? name : "Game pad"));
							SetContolPadInputSprites(app, app->control_pads[1 + event.cdevice.which]);
						}
					}
					break;
				case SDL_CONTROLLERDEVICEREMOVED:
					controller = SDL_GameControllerFromInstanceID(event.cdevice.which);
					SDL_Log("Controller Removed: %s", SDL_GameControllerName(controller));
					SDL_GameControllerClose(controller);
					break;
				case SDL_CONTROLLERAXISMOTION:
					controller = SDL_GameControllerFromInstanceID(event.caxis.which);
					break;
				case SDL_CONTROLLERBUTTONDOWN:
					controller = SDL_GameControllerFromInstanceID(event.cbutton.which);
					break;
				case SDL_CONTROLLERBUTTONUP:
					controller = SDL_GameControllerFromInstanceID(event.cbutton.which);
					break;
				case SDL_USEREVENT:

					break;
				case SDL_KEYDOWN:
				{
					switch ( event.key.keysym.sym )
					{
						case SDLK_ESCAPE:
						case SDLK_AC_BACK:
							state = EXITING;
							break;
						case SDLK_PAUSE:
							state = (state == NORMAL ? PAUSED : NORMAL);
							break;
						case SDLK_F4: //
						{
							if ( (event.key.keysym.mod & KMOD_ALT) )
							{
							}
							break;
						}

						case SDLK_F5: // Quick Save
						{
							break;
						}
						case SDLK_F6: // Quick Load
						{
							break;
						}
						case SDLK_SYSREQ:
						{
							app->flags = 1;
							break;
						}
						case SDLK_BACKSPACE:
						{
							event.key.keysym.sym = 8; //OS X had issue - Don't know if it still does in SDL2
							break;
						}
						case SDLK_DELETE:
						{
							event.key.keysym.sym = 127;
							break;
						}
						case SDLK_RETURN:
						{
							/* Full Screen */
							if ( (event.key.keysym.mod & KMOD_ALT) )
							{
									Uint32 flags = SDL_GetWindowFlags(app->window);
									//SDL_SetWindowFullscreen(app->window, !(flags & SDL_WINDOW_FULLSCREEN_DESKTOP));
								
							}
							break;
						}
						case SDLK_v:
						{
							/* Paste Text */
							if ( (event.key.keysym.mod & KMOD_CTRL) && SDL_HasClipboardText() )
							{

							}
							break;
						}

					}
					break;
				}
				default:
					break;
			}
	}

	for (ControllerPad &pad : app->control_pads) {
		uint32_t input_test = 0;
		pad.button[0].value = UpdatGetInput(app, input_test, 0, pad.button[0].device, pad.button[0].device_number, pad.button[0].sym);
		pad.button[1].value = UpdatGetInput(app, input_test, 1, pad.button[1].device, pad.button[1].device_number, pad.button[1].sym);
		pad.button[2].value = UpdatGetInput(app, input_test, 2, pad.button[2].device, pad.button[2].device_number, pad.button[2].sym);
		pad.button[3].value = UpdatGetInput(app, input_test, 3, pad.button[3].device, pad.button[3].device_number, pad.button[3].sym);
		pad.button[4].value = UpdatGetInput(app, input_test, 4, pad.button[4].device, pad.button[4].device_number, pad.button[4].sym);
		pad.button[5].value = UpdatGetInput(app, input_test, 5, pad.button[5].device, pad.button[5].device_number, pad.button[5].sym);
		pad.button[6].value = UpdatGetInput(app, input_test, 6, pad.button[6].device, pad.button[6].device_number, pad.button[6].sym);
		pad.button[7].value = UpdatGetInput(app, input_test, 7, pad.button[7].device, pad.button[7].device_number, pad.button[7].sym);
		pad.button[8].value = UpdatGetInput(app, input_test, 8, pad.button[8].device, pad.button[8].device_number, pad.button[8].sym);
		pad.button[9].value = UpdatGetInput(app, input_test, 9, pad.button[9].device, pad.button[9].device_number, pad.button[9].sym);
		switch (pad.left_stick.device) {
			case KEYBOARD: {
				int16_t key1, key2;
				key1 = UpdatGetInput(app, input_test, 10, KEYBOARD, pad.left_stick.device_number,pad.left_stick.sym[0]);
				key2 = UpdatGetInput(app, input_test, 10, KEYBOARD, pad.left_stick.device_number,pad.left_stick.sym[1]);
				pad.left_stick.value[0] = ( key1 && !key2 ? -255 : ( !key1 && key2 ? 255 : 0) );
	
				key1 = UpdatGetInput(app, input_test, 11, KEYBOARD, pad.left_stick.device_number,pad.left_stick.sym[2]);
				key2 = UpdatGetInput(app, input_test, 11, KEYBOARD, pad.left_stick.device_number,pad.left_stick.sym[3]);
				pad.left_stick.value[1] = ( key1 && !key2 ? -255 : ( !key1 && key2 ? 255 : 0) );

				key1 = UpdatGetInput(app, input_test, 12, KEYBOARD, pad.left_stick.device_number,pad.left_stick.sym[4]);
				key2 = UpdatGetInput(app, input_test, 12, KEYBOARD, pad.left_stick.device_number,pad.left_stick.sym[5]);
				pad.left_stick.value[2] = ( key1 && !key2 ? -255 : ( !key1 && key2 ? 255 : 0) );
				break;
			}
			case CONTROLAXIS: {
				pad.left_stick.value[0] = UpdatGetInput(app, input_test, 10, CONTROLAXIS, pad.left_stick.device_number,pad.left_stick.sym[1]);
				pad.left_stick.value[1] = UpdatGetInput(app, input_test, 11, CONTROLAXIS, pad.left_stick.device_number,pad.left_stick.sym[3]);
				pad.left_stick.value[2] = UpdatGetInput(app, input_test, 12, CONTROLAXIS, pad.left_stick.device_number,pad.left_stick.sym[5]);
				break;
			}
			default:
				break;
		}

		if (input_test) {
			app->control_default = &pad; //Switch default controller
		}
	}
	
	app->gametime_ms += app->frame_ms;
	app->realtime_counter = SDL_GetPerformanceCounter();
	app->animation_ms = ( state == NORMAL ? app->frame_ms : 0 ); //Only animate when normal

	return state;
}

void SetContolPadInput(AppOne *app, ControllerPad & pad ) {
	char buffer[64];
	for (size_t i = 0; i < 10; i++) {
		if ( GetInputImage( buffer, pad.name, pad.button[i].device, pad.button[i].device_number, pad.button[i].sym ) ) {
			SpriteRefTexture * ptr = (SpriteRefTexture*)elix_hashmap_value(&app->textures_input, buffer);
			if ( ptr ) {
				pad.button[i].sprite = *ptr;
			} else {
				pad.button[i].sprite = {0};
			}
		}
	}
	for (size_t i = 0; i < 6; i++) {	
		if ( GetInputImage( buffer, pad.name, pad.left_stick.device, pad.left_stick.device_number, pad.left_stick.sym[i] ) ) {
			SpriteRefTexture * ptr = (SpriteRefTexture*)elix_hashmap_value(&app->textures_input, buffer);
			if ( ptr ) {
				pad.left_stick.sprite[i]  = *ptr;
			} else {
				pad.button[i].sprite = {0};
			}
		}
	}
}


void UpdateGameCodeFunction(CodeOne & platform) {
	platform.Frame = (uint8_t (*)( AppOne * app, GameOne * game, uint8_t state))SDL_LoadFunction(platform.handle, "GameFrame");
	platform.Init = (uint8_t (*)( AppOne * app, GameOne * game, uint8_t state))SDL_LoadFunction(platform.handle, "GameInit");
	platform.Destory = (uint8_t (*)( AppOne * app, GameOne * game, uint8_t state))SDL_LoadFunction(platform.handle, "GameDestory");
	platform.Reload = (uint8_t (*)( AppOne * app, GameOne * game, uint8_t state))SDL_LoadFunction(platform.handle, "GameReload");
}

void LoadGameCode(CodeOne & platform) {
	SDL_snprintf(platform.handle_filename, 20, "gamecode.dll");
	#ifdef _DEBUG
	if (stat("gamecode.dll", &platform.code_handle_time) < 0) {
		SDL_Log("GameCode time not found");
	}
	SDL_snprintf(platform.handle_filename, 20, "gamecode_%x.tso", platform.counter++);
	remove(platform.handle_filename);
	rename("gamecode.dll", platform.handle_filename);
	#endif

	platform.handle = SDL_LoadObject(platform.handle_filename);
	UpdateGameCodeFunction(platform);
}

void ReloadGameCode(CodeOne & platform, AppOne & app) {
	if (stat("gamecode.dll", &platform.code_handle_check) == 0) {
		if ( platform.code_handle_check.st_mtime > platform.code_handle_time.st_mtime) {
			SDL_UnloadObject(platform.handle);

			SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
			SDL_RenderClear(app.renderer);
			DrawTextSimple(&app, &app.font_bitmap, "Reloading Game Code...", { 40,40, 100, 100}, 1.0f, 0);
			SDL_RenderPresent(app.renderer);
			SDL_Delay(333);

			SDL_snprintf(platform.handle_filename, 20, "gamecode_%x.tso", platform.counter++);
			rename("gamecode.dll", platform.handle_filename);
			platform.handle = SDL_LoadObject(platform.handle_filename);
			UpdateGameCodeFunction(platform);
		}
	}
}

void CleanupGameCode(CodeOne & platform) {
	SDL_UnloadObject(platform.handle);

	#ifdef _DEBUG
	rename(platform.handle_filename, "gamecode.dll");
	if ( platform.counter > 1) {
		while ( platform.counter > 0) {
			SDL_snprintf(platform.handle_filename, 20, "gamecode_%x.tso", --platform.counter);
			remove(platform.handle_filename);
		}
	}
	#endif
}


int main( int argc, char **argv ) {
	AppOne app = {0};
	GameOne game = {0};
	CodeOne platform;
	uint8_t state = 1;

	uint32_t fps_counter = 0;
	char debug[64];

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");

	app.window = SDL_CreateWindow("Only One… In a Thousand", 100, 100, 720, 480, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE ); // | SDL_RENDERER_PRESENTVSYNC
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
	
	app.fps_ms = GetTick();
	app.scale = {1.0f, 1.0f};
	app.camera = {0, 0, 720, 480};
	app.viewpoint = {0,0, 720, 480};

	app.texture_blank = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 8,8);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
	SDL_SetTextureBlendMode( app.texture_blank, SDL_BLENDMODE_NONE);
	if ( SDL_UpdateTexture(app.texture_blank, nullptr, &blank_texture, 8) ) {
		SDL_Log("SDL_UpdateTexture: %s", SDL_GetError());
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");

	LoadInputTexture(&app, "keyboard", keyboard_sheet_data, keyboard_sheet_size, keyboard_image_data, keyboard_image_size);
	LoadInputTexture(&app, "mouse", mouse_generic_sheet_data, mouse_generic_sheet_size, mouse_generic_image_data, mouse_generic_image_size);
	LoadInputTexture(&app, "Game pad", joystick_generic_sheet_data, joystick_generic_sheet_size, joystick_generic_image_data, joystick_generic_image_size);
	LoadInputTexture(&app, "XInput Controll", joystick_xbox360_sheet_data, joystick_xbox360_sheet_size, joystick_xbox360_image_data, joystick_xbox360_image_size);
	
	app.control_pads[0] = default_keyboard;
	app.control_pads[1] = default_gamepad;
	app.control_pads[2] = default_gamepad;
	app.control_pads[3] = default_gamepad;
	app.control_pads[4] = default_gamepad;
	
	for (size_t i = 1; i < 4; i++) {
		app.control_pads[1+i].button[0].device_number = i;
		app.control_pads[1+i].button[1].device_number = i;
		app.control_pads[1+i].button[2].device_number = i;
		app.control_pads[1+i].button[3].device_number = i;
		app.control_pads[1+i].button[4].device_number = i;
		app.control_pads[1+i].button[5].device_number = i;
		app.control_pads[1+i].button[6].device_number = i;
		app.control_pads[1+i].button[7].device_number = i;
		app.control_pads[1+i].button[8].device_number = i;
		app.control_pads[1+i].button[9].device_number = i;
		app.control_pads[1+i].left_stick.device_number = i;
		app.control_pads[1+i].right_stick.device_number = i;
	}
	
	app.control_default = &app.control_pads[0];

	SetContolPadInput(&app, app.control_pads[0]);

	BuildBitFont(app.renderer, &app.font_bitmap);

	SDL_RenderClear(app.renderer);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(app.renderer, nullptr);
	DrawTextSimple(&app, &app.font_bitmap, u8"loading", {600, 460, 120, 20}, 1.0f, 0);
	SDL_RenderPresent(app.renderer);

	SDL_AudioSpec audio_spec;
	SDL_AudioSpec audio_requested = {0};
	audio_requested.freq = 44100;
	audio_requested.format = AUDIO_S16;
	audio_requested.channels = 2;
	audio_requested.samples = 4096;

	app.audio_device = SDL_OpenAudioDevice(nullptr,0, &audio_requested, &audio_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	SDL_PauseAudioDevice(app.audio_device, 0);

	LoadGameCode(platform);

	while(state) {
		SDL_RenderClear(app.renderer);
		#ifdef _DEBUG
		ReloadGameCode(platform,app);
		#endif
		state = HandleEvents(&app, state);
		if ( platform.Frame ) {
			state = platform.Frame(&app, &game, state);
		} else {
			DrawTextSimple(&app, &app.font_bitmap, userinput_test, {0, 16, 40, 20}, 1.0f, 0);
		}

		for (size_t i = 0; i < app.display_queue.counter; i++) {
			DrawRenderItem(&app, app.display_queue.items[i]);
		}

		SDL_snprintf(debug, 64, "RenderQueue: %d \t FPS: %d", app.display_queue.counter, app.fps);
		DrawTextSimple(&app, &app.font_bitmap, debug, {0, app.viewpoint.h-20, 40, 20}, 1.0f, 0);

		SDL_RenderPresent(app.renderer);
		app.display_queue.counter = 0;


		fps_counter++;
		if ( app.fps_ms + 1000 <= GetDelta(app.realtime_counter) ) {
			app.fps = fps_counter;
			app.fps_ms += 1000;
			fps_counter = 0;

		}

	}
	platform.Destory(&app, &game, 0);
	CleanupGameCode(platform);

	FreeBitFont(app.renderer, &app.font_bitmap);
	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyWindow(app.window);

	return 0;
}