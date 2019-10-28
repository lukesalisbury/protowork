/***********************************************************************************************************************
Copyright © Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter
it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If
   you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not
   required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original
   software.
3. This notice may not be removed or altered from any source distribution.
***********************************************************************************************************************/
#include <SDL2/SDL.h>
#include "elix_hashmap.hpp"

inline uint64_t GetTick() {
	return (SDL_GetPerformanceCounter()*1000) / SDL_GetPerformanceFrequency();
}
inline uint64_t GetDelta(uint64_t counter) {
	return (((double)counter / (double)SDL_GetPerformanceFrequency()) * 1000);
}

#if SDL_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 9)
struct SDL_FRect
{
	float x,y,w,h;
};

struct SDL_FPoint {
	float x,y;
};

inline int SDL_RenderCopyF(SDL_Renderer * renderer, SDL_Texture * texture,const SDL_Rect* srcrect, const SDL_FRect* dstrect) {
	SDL_Rect * pdst = nullptr;
	SDL_Rect dst;
	if ( dstrect ) {
		dst.w = (int)dstrect->w; dst.x = (int)dstrect->x; dst.y = (int)dstrect->y; dst.h = (int)dstrect->h;
		pdst = &dst;
	}
	return SDL_RenderCopy(renderer, texture, srcrect, pdst);
}
#endif

enum {
	EXITING,
	NORMAL,
	PAUSED
};

typedef enum {
	NOINPUT,
	KEYBOARD,
	MOUSEAXIS,
	MOUSEBUTTON,
	MOUSEWHEEL,
	CONTROLAXIS,
	CONTROLBUTTON,
	CONTROLHAT,
	CONTROLBALL,
	TOUCHSCREEN,
	VIRTUALBUTTON,
	VIRTUALAXIS,
	HARDWARE
} InputDevice;


struct wav_audio {
	SDL_AudioSpec spec;
	uint32_t length;
	uint8_t * buffer;
};


struct FontTexture {
	SDL_Texture * base_texture;
	SDL_Rect * offset;
	size_t offset_count;
	uint16_t padding[2];
};
struct SpriteTexture {
	SDL_Texture * base_texture;
	SDL_Rect offset;
};

struct SpriteRefTexture  {
	uint32_t id;
	SDL_Rect offset;
};

#define BUTTON_NOTHING 0
#define BUTTON_RELEASED 1
#define BUTTON_PRESSED 2
#define BUTTON_HELD 3
union ControllerValue {
	int16_t raw;
	struct {
		uint16_t state:2;
		uint16_t timer:14;
	};
};
struct ControllerButton{
	InputDevice device;
	uint32_t device_number;
	int16_t sym;
	ControllerValue value;
	SpriteRefTexture sprite;
};

struct ControllerAxis {
	InputDevice device;
	uint32_t device_number;
	int16_t sym[6]; // x-x+/y-y+/z-z++
	int16_t value[3]; // x/y/z
	SpriteRefTexture sprite[6];  // x-x+/y-y+/z-z++
};

struct ControllerPad {
	char name[24];
	ControllerButton button[10];
	ControllerAxis left_stick;
	ControllerAxis right_stick;
};

struct TextureSet {
	char name[16];
	uint16_t x,y,w,h;
};

struct RenderItem {
	SDL_FRect position;
	SDL_Color colour;
	SDL_Texture * texture;
	SDL_Rect * texture_area;
	int16_t zindex;
	uint8_t layer;
};

struct RenderQueue {
	RenderItem items[8196];
	uint16_t counter;
};


struct AppOne {
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * render_texture;

	SDL_AudioDeviceID audio_device;

	// Timings
	uint64_t realtime_counter;
	uint64_t gametime_ms;
	uint64_t internal_ms;
	uint64_t frame_ms;
	uint64_t animation_ms;
	float frame_s;
	
	uint32_t fps;
	uint32_t fps_ms;
	///
	uint32_t flags;

	///
	SDL_FPoint scale;
	SDL_FRect camera;
	SDL_Rect viewpoint;

	///
	RenderQueue display_queue;

	/// Inputs
	int32_t cached_pointer[3]; /* x, y, active */
	ControllerPad control_pads[5]; /* One for keyboard, rest for gamepads */ 
	ControllerPad * control_default;

	/// Resources

	SDL_Texture * texture_blank;
	FontTexture font_bitmap;
	elix_hashmap textures;
	elix_hashmap textures_input;
};


