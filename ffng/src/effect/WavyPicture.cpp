/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "WavyPicture.h"

#include "TimerAgent.h"

#include <math.h>

//-----------------------------------------------------------------
/**
 * Load surface.
 * Default is no waves.
 */
WavyPicture::WavyPicture(const Path &file, const V2 &loc)
    : Picture(file, loc)
{
    m_amp = 0;
    m_periode = m_surface->w;
    m_speed = 0;

    auto* vs = VS::get_singleton();

    RID shader;
    if (SDL_shaders.has("wavy"))
    {
        shader = SDL_shaders["wavy"];
    }
    else
    {
        shader = vs->shader_create();
        vs->shader_set_code(shader,
                R"(
                shader_type canvas_item;
                uniform float wave_amp = 0;
                uniform float wave_periode = 0;
                uniform float wave_shift = 0;

                void fragment(){
                    float k = TEXTURE_PIXEL_SIZE.x;
                	vec2 wave_uv_offset;
                    wave_uv_offset.x = k*(0.5 + wave_amp * sin(UV.y / (k*wave_periode) + wave_shift));

                    COLOR = texture(TEXTURE, UV + wave_uv_offset);
                }
                )");
        SDL_shaders["wavy"] = shader;
    }

    m_material = vs->material_create();
    vs->material_set_shader(m_material, shader);

    //m_surface->material = m_material;
}
//-----------------------------------------------------------------
WavyPicture::~WavyPicture()
{
    VS::get_singleton()->free(m_material);
}
//-----------------------------------------------------------------
/**
 * Blit entire surface to [x,y].
 * Do vertical waves with phase shift.
 */
void
WavyPicture::drawOn(SDL_Surface *screen)
{
    if (m_amp == 0) {
        Picture::drawOn(screen);
        return;
    }

#if DANDAN
    //NOTE: Wamp = Wamp_in_orig/2.0
    //NOTE: Wspeed = 1.0/Wspd_in_orig
    SDL_Rect dest_rect;
    SDL_Rect line_rect;
    line_rect.w = m_surface->w;
    line_rect.h = 1;
    SDL_Rect pad;
    pad.h = 1;

    float shift = TimerAgent::agent()->getCycles() * m_speed;

    for (int py = 0; py < m_surface->h; ++py) {
        //NOTE: C99 has lrintf and sinf
        Sint16 shiftX = static_cast<Sint16>(0.5 +
                m_amp * sin(py / m_periode + shift));
        line_rect.x = shiftX;
        line_rect.y = py;
        dest_rect.x = m_loc.getX();
        dest_rect.y = m_loc.getY() + py;
        SDL_BlitSurface(m_surface, &line_rect, screen, &dest_rect);

        pad.x = (shiftX < 0) ? 0 : m_surface->w - shiftX;
        pad.y = py;
        pad.w = abs(shiftX);
        dest_rect.x = m_loc.getX() + pad.x;
        dest_rect.y = m_loc.getY() + py;
        SDL_BlitSurface(m_surface, &pad, screen, &dest_rect);
    }
#else
    SDL_Rect dest_rect;
    dest_rect.x = m_loc.getX();
    dest_rect.y = m_loc.getY();

    float shift = TimerAgent::agent()->getCycles() * m_speed;
    VS::get_singleton()->material_set_param(m_material, "wave_amp", m_amp);
    VS::get_singleton()->material_set_param(m_material, "wave_periode", m_periode);
    VS::get_singleton()->material_set_param(m_material, "wave_shift", shift);

    SDL_BlitSurface(m_surface, nullptr, screen, &dest_rect, m_material);
#endif
}
