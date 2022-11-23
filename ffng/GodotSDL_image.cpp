#include "GodotSDL_image.h"
#include "core/io/resource_loader.h"

SDL_Surface* IMG_Load(const char* filename) {
    auto* surface = memnew(SDL_Surface);
    surface->type = SDL_Surface::Type::TEXTURE;
    Error error = OK;
    Ref<Texture> texture = ResourceLoader::load(filename, "", false, &error);
    DEV_ASSERT(error == OK);
	if (error != OK) {
		ERR_PRINT(vformat("Can't load texture: \"%s\"", String(filename)));
        return nullptr;
    }
    else {
        WARN_PRINT(vformat("texture %s loaded (%d x %d)", filename, texture->get_width(), texture->get_height()));
        surface->w = texture->get_width();
        surface->h = texture->get_height();
        surface->texture = texture->get_rid();

        surface->_textures.push_back(texture);
    }
    return surface;
}

std::string IMG_GetError() {
    return "";
}
