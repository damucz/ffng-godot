#include "GodotSDL_ttf.h"
#include <string>

int TTF_Init() {
    return 0;
}

void TTF_Quit() {

}

TTF_Font* TTF_OpenFont(const char *file, int height) {
    Ref<DynamicFont> font(memnew(DynamicFont));
    font->set_size(height);
    font->set_outline_color({0, 0, 0});
    font->set_outline_size(1);
    Ref<DynamicFontData> data = ResourceLoader::load(file);
    font->set_font_data(data);

    TTF_Font* f = memnew(TTF_Font);
    f->font = font;
    f->height = height;
    return f;
}

void TTF_CloseFont(TTF_Font* font) {
    memdelete(font);
}

int TTF_FontHeight(TTF_Font* font) {
    return font->height;
}

void TTF_SizeUTF8(TTF_Font* font, const char* test, int* width, int* height) {
    Size2 size = font->font->get_string_size(String::utf8(test));
    if (width)
        *width = (int)ceil(size.width);
    if (height)
        *height = (int)ceil(size.height) + 2;       // with outline
}

SDL_Surface* TTF_RenderUTF8_Shaded(TTF_Font* font, const char* content, const SDL_Color& color, const SDL_Color& bg) {
    SDL_Surface* surface = memnew(SDL_Surface);
    TTF_SizeUTF8(font, content, &surface->w, &surface->h);
    _SDL_CreateViewport(surface, surface->w, surface->h);
    surface->type = SDL_Surface::Type::VIEWPORT;
    _SDL_CreateCanvas(surface);

    // bg is always transparent in FF, otherwise we would be required to draw a rect here.
    Color c((float)color.r/255.0f, (float)color.g/255.0f, (float)color.b/255.0f);
    font->font->draw(surface->canvas_items.back()->get(), Point2(0, font->font->get_ascent()), String::utf8(content), c);
    VS::get_singleton()->draw(false);

    surface->_fonts.push_back(font->font);

    return surface;
}

std::string TTF_GetError()
{
    return "";
}