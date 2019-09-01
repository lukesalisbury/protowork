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
#include <cstdint>

static void GetTextColour( uint32_t cchar, SDL_Colour & font_color ) {
	switch ( cchar ) {
		// Black
		case '0': { font_color.r = font_color.g = font_color.b = 0; break; }
		 // Dark Blue
		case '1':	{ font_color.r = font_color.g = 0; font_color.b = 170; break; }
		// Dark Green
		case '2': { font_color.r = font_color.b = 0; font_color.g = 170; break; }
		// Dark Aqua
		case '3': { font_color.r = 0; font_color.g = font_color.b = 170; break; }
		// Dark Red
		case '4': { font_color.g = font_color.b = 0; font_color.r = 170; break;	}
		// Dark Purple
		case '5': { font_color.g = 0; font_color.r = font_color.b = 170; break; }
		// Gold
		case '6': { font_color.b = 0;	font_color.r = 255;	font_color.g = 170;	break; }
		// Gray
		case '7': { font_color.r = font_color.g = font_color.b = 170; break; }
		// Dark Gray
		case '8': { font_color.r = font_color.g = font_color.b = 85; break; }
		// Blue
		case '9': { font_color.r = font_color.g = 85; font_color.b = 255;	break; }
		// Green
		case 'a': { font_color.r = font_color.b = 85; font_color.g = 255;	break; }
		// Aqua
		case 'b': { font_color.r = 85; font_color.g = font_color.b = 255; break; }
		// Red
		case 'c': { font_color.b = font_color.g = 85;	font_color.r = 255;	break; }
		// Light Purple
		case 'd': { font_color.g = 85; font_color.r = font_color.b = 255;	break; }
		// Yellow
		case 'e': { font_color.b = 85; font_color.r = font_color.g = 255;	break; }
		default: { font_color.r = font_color.g = font_color.b = 255; }
	}

}

static uint32_t NextTextChar(uint8_t *&  object ) {
	uint8_t single = *object;
	uint32_t cchar = single;
	if ( cchar <= 128 )	{

	} else if ( cchar < 224 ) {
		object++;
		uint32_t next = *object;

		cchar = ((cchar << 6) & 0x7ff) + (next & 0x3f);
	} else if ( cchar < 240 )	{
		uint32_t next;

		object++;
		next = (*object) & 0xff;
		cchar = ((cchar << 12) & 0xffff) + ((next << 6) & 0xfff);

		object++;
		next = (*object) & 0x3f;
		cchar += next;
	}	else if ( cchar < 245 )	{
		uint32_t next;

		object++;
		next = (*object) & 0xff;
		cchar = ((cchar << 18) & 0xffff) + ((next << 12) & 0x3ffff);

		object++;
		next = (*object) & 0xff;
		cchar += (next << 6) & 0xfff;

		object++;
		next = (*object) & 0x3f;
		cchar += next;
	}
	object++;
	return cchar;
}