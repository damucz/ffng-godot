/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "EffectReverse.h"

#if DANDAN
#include "SurfaceLock.h"
#include "PixelTool.h"
#endif

const char *EffectReverse::NAME = "reverse";
//-----------------------------------------------------------------
EffectReverse::EffectReverse()
{
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

                void fragment(){
                    COLOR = texture(TEXTURE, vec2(1.0 - UV.x, UV.y));
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
 * Reverse left and right.
 */
void
EffectReverse::blit(SDL_Surface *screen, SDL_Surface *surface, int x, int y)
{
#if DANDAN
    SurfaceLock lock1(screen);
    SurfaceLock lock2(surface);

    for (int py = 0; py < surface->h; ++py) {
        for (int px = 0; px < surface->w; ++px) {
            SDL_Color pixel = PixelTool::getColor(surface, px, py);
            if (pixel.unused == 255) {
                PixelTool::putColor(screen,
                        x + surface->w - 1 - px, y + py, pixel);
            }
        }
    }
#else
    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;

    auto* vs = VS::get_singleton();
    SDL_BlitSurface(surface, nullptr, screen, &dest_rect, m_material);
#endif
}

