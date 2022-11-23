/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "EffectDisintegrate.h"

#if DANDAN
#include "SurfaceLock.h"
#include "PixelTool.h"
#endif
#include "Random.h"

const char *EffectDisintegrate::NAME = "disintegrate";
//-----------------------------------------------------------------
/**
 * Start as not disintegrated.
 */
EffectDisintegrate::EffectDisintegrate()
{
    m_disint = DISINT_START;

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
                uniform float disint;

                // https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
                float rand(vec2 co){
                   return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
                }

                void fragment(){
                    COLOR = disint < rand(UV) * 256.0 ? vec4(0.0, 0.0, 0.0, 0.0) : texture(TEXTURE, UV);
                }
                )");
    }

    DEV_ASSERT(shader.is_valid());
    m_material = vs->material_create();
    vs->material_set_shader(m_material, shader);
}
//-----------------------------------------------------------------
void
EffectDisintegrate::updateEffect()
{
    if (m_disint > 0) {
        m_disint -= DISINT_SPEED;
        if (m_disint < 0) {
            m_disint = 0;
        }
    }
}
//-----------------------------------------------------------------
/**
 * Returns true for object for who the disint effect is finished.
 */
bool
EffectDisintegrate::isDisintegrated() const
{
    return 0 == m_disint;
}
//-----------------------------------------------------------------
bool
EffectDisintegrate::isInvisible() const
{
    return isDisintegrated();
}
//-----------------------------------------------------------------
/**
 * Disintegration effect.
 * Draw only some pixels.
 */
void
EffectDisintegrate::blit(SDL_Surface *screen, SDL_Surface *surface,
        int x, int y)
{
#if DANDAN
    SurfaceLock lock1(screen);
    SurfaceLock lock2(surface);

    for (int py = 0; py < surface->h; ++py) {
        for (int px = 0; px < surface->w; ++px) {
            if (Random::aByte(py * surface->w + px) < m_disint) {
                SDL_Color pixel = PixelTool::getColor(surface, px, py);
                if (pixel.unused == 255) {
                    PixelTool::putColor(screen, x + px, y + py, pixel);
                }
            }
        }
    }
#else
    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;

    auto* vs = VS::get_singleton();
    vs->material_set_param(m_material, "disint", (float)m_disint);
    SDL_BlitSurface(surface, nullptr, screen, &dest_rect, m_material);
#endif
}

