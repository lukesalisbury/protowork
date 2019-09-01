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

#include "ourtypes.hpp"
#include "gametypes.hpp"
#include "texthandling.hpp"

#define ELIX_HASHMAP_IMPLEMENTATION
#include "elix_hashmap.hpp"

#define IS_BIT_SET(value, bit) (value) & ((1<<(bit)))
#define SET_BIT(value, bit) (value) |= ((1<<(bit)))
#define BETWEEN(value, min, max ) if ( value > max ) { value = max; } else if ( value < min ) { value = min; }

static float sinTable[8] = {0, 0.707f, 1, 0.707f, 0, -0.707f, -1, -0.707f };

inline bool OnCamera( SDL_FRect camera, RenderItem & item) {
	return true;
	if ( (item.position.x + item.position.w) < camera.x ) {
		return false;
	}
	if ( item.position.x > camera.x + camera.w) {
		return false;
	}
	if ( (item.position.y + item.position.h) < camera.y) {
		return false;
	}
	if ( item.position.y > camera.y + camera.h) {
		return false;
	}
	return true;
}

void PushRenderitem( AppOne* app, RenderItem item ) {
	size_t q = app->display_queue.counter;
	if (app->display_queue.counter >= 8191) {
		return; 
	}
	for (size_t i = app->display_queue.counter; i > 0; i--) {
		if ( app->display_queue.items[i].layer < item.layer ) {
			app->display_queue.items[i+1] = app->display_queue.items[i];
		} else if ( app->display_queue.items[i].layer > item.layer ) {
			break;
		} else if ( item.zindex < app->display_queue.items[i].zindex ) {
			app->display_queue.items[i+1] = app->display_queue.items[i];
		} else {
			break;
		}
		q = i;
	}

	app->display_queue.items[q] = item;
	app->display_queue.counter++;
}
void CalulateRenderItemPositionF( AppOne * app, RenderItem & item, SDL_Rect dimensions, SDL_FPoint location, SDL_FPoint scale) {
	item.position.x = (float)(dimensions.x) + location.x;
	item.position.y = (float)(dimensions.y) + location.y;
	item.position.w = (float)(dimensions.w);
	item.position.h = (float)(dimensions.h);
	if ( item.layer ) {
		item.position.x -= app->camera.x;
		item.position.y -= app->camera.y;
		if ( scale.x != 1.0f ) {
			item.position.x *= scale.x;
			item.position.w *= scale.x;
		}
		if ( scale.y != 1.0f ) {
			item.position.y *= scale.y;
			item.position.h *= scale.y;
		}
	}
}

void CalulateRenderItemPosition( AppOne * app, RenderItem & item, SDL_Rect dimensions, SDL_Point location, SDL_FPoint scale) {
	item.position.x = (float)(dimensions.x + location.x);
	item.position.y = (float)(dimensions.y + location.y);
	item.position.w = (float)(dimensions.w);
	item.position.h = (float)(dimensions.h);
	if ( item.layer ) {
		item.position.x -= app->camera.x;
		item.position.y -= app->camera.y;
		if ( scale.x != 1.0f ) {
			item.position.x *= scale.x;
			item.position.w *= scale.x;
		}
		if ( scale.y != 1.0f ) {
			item.position.y *= scale.y;
			item.position.h *= scale.y;
		}
	}
}


void QueueTextureF( AppOne * app, SDL_Texture * texture, SDL_Rect * texture_area, SDL_FPoint world_location, SDL_Rect shape, SDL_Color colour, int16_t zindex, uint8_t layer ) {

	RenderItem item = {	{0,0,0,0}, colour, texture, texture_area, zindex, layer };
	CalulateRenderItemPositionF(app, item, shape, world_location, app->scale);

	if ( !layer || OnCamera( app->camera, item) ) {
		PushRenderitem( app, item );
	}
}

void QueueTexture( AppOne * app, SDL_Texture * texture, SDL_Rect * texture_area, SDL_Point world_location, SDL_Rect shape, SDL_Color colour, int16_t zindex, uint8_t layer ) {

	RenderItem item = {	{0,0,0,0}, colour, texture, texture_area, zindex, layer };
	CalulateRenderItemPosition(app, item, shape, world_location, app->scale);

	if ( !layer || OnCamera( app->camera, item) ) {
		PushRenderitem( app, item );
	}
}

void QueueTextSimple(AppOne *app, FontTexture * font, const char * string, SDL_Rect position, float scale, int16_t zindex, uint8_t layer ) {
	uint8_t * object = (uint8_t*)string;
	uint8_t print = false;

	uint32_t line_padding = font->padding[1];
	SDL_Texture * selected_texture = nullptr;
	SDL_Rect * srcarea = nullptr;
	SDL_Color colour = {255,255,255,255};

	SDL_Point area = {position.x, position.y};
	SDL_Rect dimensions = {0, 0, 0, 0};

	SDL_Point textarea = {position.x, position.y};
	SDL_Point shadowarea = {position.x, position.y};
	SDL_Rect shadowdimensions = {0, 0, 0, 0};

	uint32_t cchar;

	while ( (cchar = NextTextChar(object)) > 0  ) {
		if (cchar == '\n' || cchar == '\r') {
			area.y += line_padding;
			area.x = position.x;
			colour = {255,255,255,255};
			print = false;
			line_padding = font->padding[1];
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
				print = true;
		} else if ( cchar >= 0x25B2 && cchar <= 0x25D3 ) {
			  //Axis
				print = true;
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
					dimensions.w = shadowdimensions.w = srcarea->w;
					dimensions.h = shadowdimensions.h = srcarea->h;

					shadowarea.x = textarea.x + 1;
					shadowarea.y = textarea.y + 1;

					QueueTexture(app, selected_texture, srcarea, shadowarea, shadowdimensions, {14,14,14,255}, 0, layer );
					QueueTexture(app, selected_texture, srcarea, textarea, dimensions, colour, 1, layer );

					line_padding = (srcarea->h > line_padding ? srcarea->h + 2 : line_padding);
					area.x += srcarea->w + 1;
			}
		}
	}
}



extern "C" __declspec(dllexport) uint8_t GameFrame(AppOne * app, GameOne * game, uint8_t state ) {
	char debug[128];
	


	return state;
}

