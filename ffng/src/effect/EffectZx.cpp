/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "EffectZx.h"

#if DANDAN
#include "SurfaceLock.h"
#include "PixelTool.h"
#include "PixelIterator.h"
#endif
#include "Random.h"

const char *EffectZx::NAME = "zx";
const double EffectZx::STRIPE_STANDARD = 38.5;
const double EffectZx::STRIPE_NARROW = 3.4;
//-----------------------------------------------------------------
/**
 * Read colors from all four corners.
 */
EffectZx::EffectZx()
{
    m_zx = ZX1;
    m_phase = 0;
    m_countHeight = 0;
    m_stripeHeight = STRIPE_STANDARD;

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
                uniform float count_height;
                uniform float stripe_height;
                uniform float zx;

                void fragment(){
                    vec4 colorZX1 = texture(TEXTURE, vec2(0.0, 0.0));
                    vec4 colorZX2 = texture(TEXTURE, vec2(0.0, 1.0));
                    vec4 colorZX3 = texture(TEXTURE, vec2(1.0, 0.0));
                    vec4 colorZX4 = texture(TEXTURE, vec2(1.0, 1.0));

                    vec4 color1;
                    vec4 color2;
                    if (zx < 2.5) {
                        if (zx < 1.5) { color1 = colorZX1; color2 = colorZX2; }
                        else { color1 = colorZX2; color2 = colorZX1; }
                    }
                    else {
                        if (zx < 3.5) { color1 = colorZX3; color2 = colorZX4; }
                        else { color1 = colorZX4; color2 = colorZX3; }
                    }

                    float shift = count_height * TEXTURE_PIXEL_SIZE.y;
                    float height = stripe_height * TEXTURE_PIXEL_SIZE.y;
                    //vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
                    vec4 color = mod(UV.y + shift, 2.0 * height) > height ? color1 : color2;

                    vec4 bg = texture(TEXTURE, UV);
                    COLOR = vec4(color.rgb, color.a * bg.a);
                }
                )");
    }

    DEV_ASSERT(shader.is_valid());
    m_material = vs->material_create();
    vs->material_set_shader(m_material, shader);
}
//-----------------------------------------------------------------
/**
 * Update sprite height as ZX Spectrum does.
 */
    void
EffectZx::updateEffect()
{
    m_phase = (m_phase + 1) % 500;
    if (m_phase == 1) {
        m_zx = ZX1;
        m_stripeHeight = STRIPE_STANDARD;
    }
    else if (2 <= m_phase && m_phase <= 51) {
        m_stripeHeight = (m_stripeHeight * 3
                * (0.97 + Random::randomReal(0.06))
                + STRIPE_STANDARD) / 4.0;
    }
    else if (m_phase == 52) {
        m_zx = ZX3;
        m_stripeHeight = STRIPE_NARROW;
    }
    else {
        m_stripeHeight = (m_stripeHeight * 3
                * (0.95 + Random::randomReal(0.1))
                + STRIPE_NARROW) / 4.0;
    }
}
//-----------------------------------------------------------------
/**
 * Draw ZX spectrum loading.
 */
    void
EffectZx::blit(SDL_Surface *screen, SDL_Surface *surface, int x, int y)
{
#if DANDAN
    SurfaceLock lock1(screen);
    SurfaceLock lock2(surface);

    Uint32 colorZX1 = PixelTool::convertColor(screen->format,
            PixelTool::getColor(surface, 0, 0));
    Uint32 colorZX2 = PixelTool::convertColor(screen->format,
            PixelTool::getColor(surface, 0, surface->h - 1));
    Uint32 colorZX3 = PixelTool::convertColor(screen->format,
            PixelTool::getColor(surface, surface->w - 1, 0));
    Uint32 colorZX4 = PixelTool::convertColor(screen->format,
            PixelTool::getColor(surface, surface->w - 1, surface->h - 1));

    PixelIterator pit(surface);
    for (int py = 0; py < surface->h; ++py) {
        m_countHeight++;
        if (m_countHeight > m_stripeHeight) {
            m_countHeight -= m_stripeHeight;
            switch (m_zx) {
                case ZX1:
                    m_zx = ZX2;
                    break;
                case ZX2:
                    m_zx = ZX1;
                    break;
                case ZX3:
                    m_zx = ZX4;
                    break;
                default:
                    m_zx = ZX3;
                    break;
            }
        }

        Uint32 usedColor;
        switch (m_zx) {
            case ZX1:
                usedColor = colorZX1;
                break;
            case ZX2:
                usedColor = colorZX2;
                break;
            case ZX3:
                usedColor = colorZX3;
                break;
            default:
                usedColor = colorZX4;
                break;
        }

        for (int px = 0; px < surface->w; ++px) {
            if (!pit.isTransparent()) {
                PixelTool::putPixel(screen,
                        x + px, y + py, usedColor);
            }
            pit.inc();
        }
    }
#else
    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;

    auto* vs = VS::get_singleton();
    vs->material_set_param(m_material, "count_height", m_countHeight);
    vs->material_set_param(m_material, "stripe_height", m_stripeHeight);
    vs->material_set_param(m_material, "zx", m_zx);
    SDL_BlitSurface(surface, nullptr, screen, &dest_rect, m_material);

    if (((int)((surface->h + m_countHeight) / m_stripeHeight) % 2) == 1) {
        switch (m_zx) {
            case ZX1:
                m_zx = ZX2;
                break;
            case ZX2:
                m_zx = ZX1;
                break;
            case ZX3:
                m_zx = ZX4;
                break;
            default:
                m_zx = ZX3;
                break;
        }
    }
    m_countHeight = fmod(surface->h + m_countHeight, m_stripeHeight);
#endif
}

