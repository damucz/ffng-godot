#include "GodotSDL.h"
#include "servers/visual_server.h"
#include "core/os/time.h"
#include "core/error_macros.h"
#include "core/os/dir_access.h"
#include "core/os/os.h"
#include "core/engine.h"

Node2D* SDL_node = nullptr;
int SDL_window_width = 0;
int SDL_window_height = 0;
Uint8 SDL_keys[SDLK_LAST];
List<SDL_Event> SDL_events;

List<SDL_Surface*> SDL_to_be_cleared;
List<SDL_Surface*> SDL_to_be_removed;

Dictionary SDL_shaders;

int SDL_mouse_x = -1;
int SDL_mouse_y = -1;
Uint8 SDL_mouse_buttons = 0;


void _SDL_CreateViewport(SDL_Surface* surface, int width, int height, bool screen) {
    auto* vs = VS::get_singleton();
    RID viewport = vs->viewport_create();
    vs->viewport_set_hide_scenario(viewport, true);
    vs->viewport_set_disable_3d(viewport, true);
    vs->viewport_set_disable_environment(viewport, true);
    vs->viewport_set_hdr(viewport, false);
    //vs->viewport_set_msaa(viewport, screen ? VS::VIEWPORT_MSAA_8X : VS::VIEWPORT_MSAA_DISABLED);      not supported in javascript, disabled for now
    vs->viewport_set_active(viewport, true);
    vs->viewport_set_size(viewport, width, height);
    vs->viewport_set_usage(viewport, VS::VIEWPORT_USAGE_2D_NO_SAMPLING);
    vs->viewport_set_update_mode(viewport, VS::VIEWPORT_UPDATE_DISABLED);
    vs->viewport_set_clear_mode(viewport, VS::VIEWPORT_CLEAR_NEVER);
    vs->viewport_set_transparent_background(viewport, !screen);
    vs->viewport_set_vflip(viewport, true);

    surface->viewport = viewport;

    // screen:
    //vs->viewport_attach_to_screen(viewport, Rect2(0, 0, (real_t)width, (real_t)height), 0);
    //vs->viewport_set_render_direct_to_screen(viewport, true);
}

void _SDL_CreateCanvas(SDL_Surface* surface) {
    DEV_ASSERT(surface->viewport.is_valid());
    DEV_ASSERT(!surface->canvas.is_valid());

    auto* vs = VS::get_singleton();
    RID canvas = vs->canvas_create();
    vs->viewport_attach_canvas(surface->viewport, canvas);
    surface->canvas = canvas;

    _SDL_CreateCanvasItem(surface);

    SDL_to_be_cleared.push_back(surface);
    vs->viewport_set_update_mode(surface->viewport, VS::VIEWPORT_UPDATE_ONCE);
}

void _SDL_CreateCanvasItem(SDL_Surface* surface) {
    DEV_ASSERT(surface->canvas.is_valid());
    auto* vs = VS::get_singleton();
    RID canvas_item = vs->canvas_item_create();
    vs->canvas_item_set_parent(canvas_item, surface->canvas);
    //vs->canvas_item_set_copy_to_backbuffer(canvas_item, true, Rect2());
    surface->canvas_items.push_back(canvas_item);
}

void _SDL_CheckCanvasAndMaterial(SDL_Surface* surface, RID material) {
    auto* vs = VS::get_singleton();
    if (!surface->canvas.is_valid()) {
        _SDL_CreateCanvas(surface);
    }
    if (material != surface->_last_material) {
        _SDL_CreateCanvasItem(surface);
        if (material.is_valid())
            vs->canvas_item_set_material(surface->canvas_items.back()->get(), material);
        surface->_last_material = material;
    }
}

void SDL_Quit() {
    for (int i = 0; i < SDL_shaders.size(); ++i)
    {
        VS::get_singleton()->free(SDL_shaders.get_value_at_index(i));
    }
    SDL_shaders.clear();
}

SDL_Surface* SDL_SetVideoMode(int width, int height, Uint8 bpp, Uint32 videoflags) {
    SDL_Surface* screen = SDL_CreateRGBSurface(videoflags, width, height, bpp, 0, 0, 0, 0);

    //SDL_FitScreen();      Called later after the backbuffer is swapped.

    return screen;
}

void SDL_Flip(SDL_Surface* screen) {
    DEV_ASSERT(screen->type == SDL_Surface::Type::VIEWPORT);
    auto* vs = VS::get_singleton();

    RID canvas_item = SDL_node->get_canvas_item();
    vs->canvas_item_clear(canvas_item);
    //vs->canvas_item_set_parent(screen->canvas_items.back()->get(), SDL_node->get_canvas_item());

    RID texture = vs->viewport_get_texture(screen->viewport);
    //vs->texture_set_flags(texture, VS::TEXTURE_FLAG_REPEAT | VS::TEXTURE_FLAG_FILTER);
    vs->canvas_item_add_texture_rect(canvas_item, {0, 0, (real_t)screen->w, (real_t)screen->h}, texture);

    //WARN_PRINT(vformat("screen flipped (%d x %d)", screen->w, screen->h));

    SDL_FitScreen(screen);
}

void _SDL_PurgeCanvas(SDL_Surface* surface) {
    auto* vs = VS::get_singleton();
    for (List<RID>::Element *E = surface->canvas_items.front(); E; E = E->next()) {
        RID canvas_item = E->get();
        vs->free(canvas_item);
    }
    surface->canvas_items.clear();
    // Calling from SDL_to_be_removed, canvas is allowed to be null.
    if (surface->canvas.is_valid()) {
        vs->viewport_remove_canvas(surface->viewport, surface->canvas);
        vs->free(surface->canvas);
        surface->canvas = RID();
    }

    surface->_textures.clear();
    surface->_fonts.clear();
}

void SDL_Purge() {
    {
        for (List<SDL_Surface*>::Element *E = SDL_to_be_cleared.front(); E; E = E->next()) {
            SDL_Surface* surface = E->get();
            _SDL_PurgeCanvas(surface);
        }
        SDL_to_be_cleared.clear();
    }

    {
        for (List<SDL_Surface*>::Element *E = SDL_to_be_removed.front(); E; E = E->next()) {
            SDL_Surface* surface = E->get();
            _SDL_PurgeCanvas(surface);
            memdelete(surface);
        }
        SDL_to_be_removed.clear();
    }
}

void SDL_FitScreen(SDL_Surface* screen) {
    if (!Engine::get_singleton()->is_editor_hint()) {
        // When screen is null, the game video mode stays the same, but a window has been resized.
        if (!screen || screen->w != SDL_window_width || screen->h != SDL_window_height) {
            if (screen) {
                SDL_window_width = screen->w;
                SDL_window_height = screen->h;
            }

            // center node
            Size2 window_size = OS::get_singleton()->get_window_size();
            Size2 game_size((real_t)SDL_window_width, (real_t)SDL_window_height);
            real_t scale = 1.0f;
            if (window_size.aspect() > game_size.aspect()) {
                scale = window_size.height / game_size.height;
                SDL_node->set_position({(window_size.width - game_size.width * scale) / 2.0f, 0});
            }
            else {
                scale = window_size.width / game_size.width;
                SDL_node->set_position({0, (window_size.height - game_size.height * scale) / 2.0f});
            }
            SDL_node->set_scale({scale, scale});
        }
    }
}

SDL_Surface* SDL_CreateRGBSurface(Uint32 flags, int width, int height, Uint8 bpp, Uint8 Rmask, Uint8 Gmask, Uint8 Bmask, Uint8 Amask) {
    SDL_Surface* surface = memnew(SDL_Surface);
    surface->type = SDL_Surface::Type::EMPTY;
    surface->w = width;
    surface->h = height;
    return surface;
}

void SDL_BlitSurface(SDL_Surface* surface, SDL_Rect* src_rect, SDL_Surface* screen, SDL_Rect* dest_rect, RID material) {
    SDL_Rect srect;
    if (src_rect) {
        srect = *src_rect;
    }
    else {
        srect.x = 0;
        srect.y = 0;
        srect.w = 0;
        srect.h = 0;
    }
    if (srect.w == 0)
        srect.w = surface->w;
    if (srect.h == 0)
        srect.h = surface->h;
    SDL_Rect drect;
    if (dest_rect) {
        drect = *dest_rect;
    }
    else {
        drect.x = 0;
        drect.y = 0;
        drect.w = 0;
        drect.h = 0;
    }
    if (drect.w == 0)
        drect.w = srect.w;
    if (drect.h == 0)
        drect.h = srect.h;

    auto* vs = VS::get_singleton();
    switch (screen->type) {
        case SDL_Surface::Type::EMPTY: {
            _SDL_CreateViewport(screen, screen->w, screen->h);
            screen->type = SDL_Surface::Type::VIEWPORT;
            SDL_BlitSurface(surface, src_rect, screen, dest_rect);
            break;
        }
        case SDL_Surface::Type::TEXTURE: {
            DEV_ASSERT(screen->texture.is_valid());
            _SDL_CreateViewport(screen, screen->w, screen->h);
            screen->type = SDL_Surface::Type::VIEWPORT;
            _SDL_CreateCanvas(screen);
            Rect2 sr((real_t)srect.x, (real_t)srect.y, (real_t)srect.w, (real_t)srect.h);
            Rect2 dr((real_t)drect.x, (real_t)drect.y, (real_t)drect.w, (real_t)drect.h);
            vs->canvas_item_add_texture_rect(screen->canvas_items.back()->get(), {0, 0, (real_t)screen->w, (real_t)screen->h}, screen->texture);
            SDL_BlitSurface(surface, src_rect, screen, dest_rect);
            break;
        }
        case SDL_Surface::Type::VIEWPORT: {
            DEV_ASSERT(screen->viewport.is_valid());
            Rect2 sr((real_t)srect.x, (real_t)srect.y, (real_t)srect.w, (real_t)srect.h);
            Rect2 dr((real_t)drect.x, (real_t)drect.y, (real_t)drect.w, (real_t)drect.h);
            RID stexture;
            switch(surface->type) {
                case SDL_Surface::Type::TEXTURE: {
                    DEV_ASSERT(surface->texture.is_valid());
                    //WARN_PRINT(vformat("blit texture (%s) to screen/viewport (%s)", String(sr), String(dr)));
                    stexture = surface->texture;
                    break;
                }
                case SDL_Surface::Type::VIEWPORT: {
                    DEV_ASSERT(surface->viewport.is_valid());
                    //WARN_PRINT(vformat("blit viewport (%s) to screen/viewport (%s)", String(sr), String(dr)));
                    stexture = vs->viewport_get_texture(surface->viewport);
                    
                    vs->viewport_set_parent_viewport(surface->viewport, screen->viewport);

                    vs->viewport_set_active(surface->viewport, false);
                    vs->viewport_set_active(screen->viewport, false);

                    vs->viewport_set_active(surface->viewport, true);
                    vs->viewport_set_active(screen->viewport, true);
                   
                    break;
                }
            }

            _SDL_CheckCanvasAndMaterial(screen, material);
            vs->canvas_item_add_texture_rect_region(screen->canvas_items.back()->get(), dr, stexture, sr);
            //vs->draw(false);
            break;
        }
        case SDL_Surface::Type::NONE:
        default:
            DEV_ASSERT(false);
    }
    DEV_ASSERT(screen->type == SDL_Surface::Type::VIEWPORT && screen->viewport.is_valid());
}

void SDL_FillRect(SDL_Surface* screen, SDL_Rect* rect, SDL_Color color) {
    SDL_Rect r;
    if (rect) {
        r = *rect;
    }
    else {
        r.x = 0;
        r.y = 0;
        r.w = screen->w;
        r.h = screen->h;
    }
    auto* vs = VS::get_singleton();
    Rect2 rr((real_t)r.x, (real_t)r.y, (real_t)r.w, (real_t)r.h);
    Color c{(float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.unused / 255.0f};

    _SDL_CheckCanvasAndMaterial(screen);
    vs->canvas_item_add_rect(screen->canvas_items.back()->get(), rr, c);
}

void SDL_FreeSurface(SDL_Surface* surface) {
    if (surface) {
        SDL_to_be_removed.push_back(surface);
    }
}

void SDL_ResetKeyStates() {
    for (int i = 0; i < SDLK_LAST; ++i) {
        SDL_keys[i] = 0;
    }
}

Uint8* SDL_GetKeyState(void*) {
    return SDL_keys;
}

void SDL_SetKeyPressed(SDLKey key, bool pressed) {
    SDL_keys[key] = pressed ? 1 : 0;
}

Uint8 SDL_GetMouseState(int* x, int* y) {
    *x = SDL_mouse_x;
    *y = SDL_mouse_y;
    return SDL_mouse_buttons;
}

void SDL_SetMouseState(int x, int y, Uint8 pressed) {
    SDL_mouse_x = x;
    SDL_mouse_y = y;
    SDL_mouse_buttons = pressed;
}

void SDL_PushEvent(const SDL_Event& event) {
    SDL_events.push_back(event);
    DEV_ASSERT(SDL_events.size() < 128);
}

bool SDL_PollEvent(SDL_Event& event) {
    if (SDL_events.empty())
        return false;
    else {
        event = SDL_events.front()->get();
        SDL_events.pop_front();
        return true;
    }
}

Uint32 SDL_GetTicks() {
    return (Uint32)Time::get_singleton()->get_ticks_msec();
}

void SDL_Delay(Uint32 delay) {
    // implemented in node's update
}

const char* SDL_GetError() {
    return "SDL error";
}

int Godot_mkdir(const char* name, int permissions) {
    DirAccess* da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
    Error error = da->make_dir(name);
    memdelete(da);
    return error;
}
