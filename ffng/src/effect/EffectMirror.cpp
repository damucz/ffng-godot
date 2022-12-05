/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "EffectMirror.h"

#if DANDAN
#include "SurfaceLock.h"
#include "PixelTool.h"
#endif
#include "servers/visual/visual_server_globals.h"
#include "servers/visual/visual_server_viewport.h"

const char *EffectMirror::NAME = "mirror";
EffectMirror::EffectMirror() {
    auto* vs = VS::get_singleton();

    RID shader;
    if (SDL_shaders.has(NAME))
    {
        shader = SDL_shaders[NAME];
    }
    else
    {
        shader = vs->shader_create();
        vs->shader_set_code(shader,
                R"(
                shader_type canvas_item;
                uniform vec4 mask;
                uniform sampler2D screen_texture;
                uniform vec2 screen_texture_pixel_size;
                uniform vec2 surface_pos;
                uniform float mirror_border;

                void fragment(){
                    //vec4 screen_color = vec4(1.0, 0.0, 0.0, 1.0);
                    vec2 uv = UV / TEXTURE_PIXEL_SIZE;
                    uv = vec2(surface_pos.x - uv.x + mirror_border, surface_pos.y + uv.y);
                    uv *= screen_texture_pixel_size;
                    vec4 screen_color = texture(screen_texture, uv);

                    vec4 color = texture(TEXTURE, UV);
                    vec4 delta = abs(color - mask);
                    if (length(delta) < 0.1)
                        color = screen_color;
                    COLOR = color;
                }
                )");
        SDL_shaders[NAME] = shader;
    }

    DEV_ASSERT(shader.is_valid());
    m_material = vs->material_create();
    vs->material_set_shader(m_material, shader);
}
//-----------------------------------------------------------------
/**
 * Mirror effect. Draw left side inside.
 * The pixel in the middle will be used as a mask.
 * NOTE: mirror object should be drawn as the last.
 */
void
EffectMirror::blit(SDL_Surface *screen, SDL_Surface *surface, int x, int y)
{
#if DANDAN
    SurfaceLock lock1(screen);
    SurfaceLock lock2(surface);

    SDL_Color mask = PixelTool::getColor(surface,
            surface->w / 2, surface->h / 2);

    for (int py = 0; py < surface->h; ++py) {
        for (int px = 0; px < surface->w; ++px) {
            SDL_Color pixel = PixelTool::getColor(surface, px, py);
            if (px > MIRROR_BORDER && PixelTool::colorEquals(pixel, mask)) {
                SDL_Color sample = PixelTool::getColor(screen,
                        x - px + MIRROR_BORDER, y + py);
                PixelTool::putColor(screen, x + px, y + py, sample);
            }
            else {
                if (pixel.unused == 255) {
                    PixelTool::putColor(screen, x + px, y + py, pixel);
                }
            }
        }
    }
#else
    auto* vs = VS::get_singleton();

    // Make a copy (viewport) from the current screen.
    SDL_Surface* screen_copy = memnew(SDL_Surface);
    *screen_copy = *screen;
    screen_copy->type = SDL_Surface::Type::VIEWPORT;

    screen->viewport = RID();
    screen->canvas = RID();
    screen->canvas_items.clear();

    // Reset screen to empty state.
    _SDL_CreateViewport(screen, screen->w, screen->h, true);
    _SDL_CreateCanvas(screen);
    // And blit the original screen to the new one.
    SDL_BlitSurface(screen_copy, nullptr, screen, nullptr);

    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;

    RID screen_texture = vs->viewport_get_texture(screen_copy->viewport);
    vs->material_set_param(m_material, "mask", Color(0, 1.0, 1.0, 1.0));
    vs->material_set_param(m_material, "screen_texture", screen_texture);
    vs->material_set_param(m_material, "screen_texture_pixel_size", Vector2(1.0f / screen->w, 1.0f / screen->h));
    vs->material_set_param(m_material, "surface_pos", Vector2((float)x, (float)y));
    vs->material_set_param(m_material, "mirror_border", (float)MIRROR_BORDER);
    SDL_BlitSurface(surface, nullptr, screen, &dest_rect, m_material);

    SDL_FreeSurface(screen_copy);
#endif
}

