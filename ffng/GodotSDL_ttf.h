#ifndef GODOTSDL_TTF_HEADER
#define GODOTSDL_TTF_HEADER

#include "GodotSDL.h"
#include "scene/resources/dynamic_font.h"

class TTF_Font
{
public:
    int height;

    Ref<DynamicFont> font;
};

int TTF_Init();     // error < 0
void TTF_Quit();
TTF_Font* TTF_OpenFont(const char *file, int height);
void TTF_CloseFont(TTF_Font* font);
int TTF_FontHeight(TTF_Font* font);
void TTF_SizeUTF8(TTF_Font* font, const char* test, int* width, int* height);
SDL_Surface* TTF_RenderUTF8_Shaded(TTF_Font* font, const char* content, const SDL_Color& color, const SDL_Color& bg);
std::string TTF_GetError();

#endif
