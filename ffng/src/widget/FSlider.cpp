/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "FSlider.h"

#include "OptionAgent.h"
#include "MouseStroke.h"
#if DANDAN
#include "SurfaceTool.h"
#endif
#include "minmax.h"

//-----------------------------------------------------------------
/**
 * Create slider for this param and optinaly specify min and max value.
 */
FSlider::FSlider(const std::string &param, int minValue, int maxValue)
    : m_param(param)
{
    m_min = minValue;
    m_max = maxValue;
}
//-----------------------------------------------------------------
/**
 * Scale value to slider rate.
 * @return position on slider
 */
int
FSlider::value2slide(int value)
{
    int slide = value - m_min;
    slide = max(slide, m_min);
    slide = min(slide, m_max);
    return slide * PIXELS_PER_VALUE;
}
//-----------------------------------------------------------------
/**
 * Scale slider rate to param value.
 * @return integer value
 */
int
FSlider::slide2value(int slide)
{
    int value = slide + m_min;
    double fraction = static_cast<double>(value) / PIXELS_PER_VALUE;
    return static_cast<int>(fraction + 0.5);
}
//-----------------------------------------------------------------
void
FSlider::drawOn(SDL_Surface *screen)
{
    int value = OptionAgent::agent()->getAsInt(m_param);
    SDL_Color gray = {0x00, 0x00, 0x00, 129};
#if DANDAN
    Uint32 green = SDL_MapRGB(screen->format, 0x00, 0xff, 0x00);
#else
    SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};
#endif

    SDL_Rect rect;
    rect.x = m_shift.getX();
    rect.y = m_shift.getY();
    rect.w = getW();
    rect.h = getH();
#if DANDAN
    SurfaceTool::alphaFill(screen, &rect, gray);
#else
    SDL_FillRect(screen, &rect, gray);
#endif

    rect.x = m_shift.getX();
    rect.y = m_shift.getY();
    rect.w = value2slide(value);
    rect.h = getH();
    SDL_FillRect(screen, &rect, green);
}
//-----------------------------------------------------------------
void
FSlider::own_mouseButton(const MouseStroke &stroke)
{
    if (stroke.isLeft()) {
        //TODO: play click
        V2 inside = stroke.getLoc().minus(m_shift);
        int value = slide2value(inside.getX());
        OptionAgent::agent()->setPersistent(m_param, value);
    }
}

