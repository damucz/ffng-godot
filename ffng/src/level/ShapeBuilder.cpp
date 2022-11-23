/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "ShapeBuilder.h"

#include "FShape.h"
#include "View.h"
#if DANDAN
#include "SurfaceTool.h"
#endif

//-----------------------------------------------------------------
/**
 * Prepare color values
 * based on the given shape and weight.
 */
    void
ShapeBuilder::prepareColor(SDL_Color *color, const FShape *shape,
        Cube::eWeight weight)
{
    if (NULL == color) {
        return;
    }

    color->r = 0;
    color->g = 0;
    color->b = 0;
    color->unused = 255;

    switch (weight) {
        case Cube::LIGHT:
            color->g = ShapeBuilder::calcShapeHash(shape) % 255;
            color->r = 255 - color->g;
            break;
        case Cube::HEAVY:
            color->b = 50 + ShapeBuilder::calcShapeHash(shape) % (255 - 50);
            break;
        default:
            color->r = 128;
            color->g = 128;
            color->b = 128;
            break;
    }
}
//-----------------------------------------------------------------
/**
 * Calc an almost unique hash of the shape.
 */
    Uint32
ShapeBuilder::calcShapeHash(const FShape *shape)
{
    Uint32 hash = 0;
    FShape::const_iterator end = shape->marksEnd();
    for (FShape::const_iterator i = shape->marksBegin(); i != end; ++i) {
        hash = 31 * hash + i->getX();
        hash = 31 * hash + i->getY();
    }
    return hash;
}
//-----------------------------------------------------------------
/**
 * Create new image for the given shape.
 */
    SDL_Surface *
ShapeBuilder::createImage(const FShape *shape, Cube::eWeight weight)
{
#if DANDAN
    static const SDL_Color TRANSPARENT = {255, 0, 255, 255};

    SDL_Surface *surface = SurfaceTool::createTransparent(
            shape->getW() * View::SCALE, shape->getH() * View::SCALE,
            TRANSPARENT);
#else
    SDL_Surface* surface = SDL_CreateRGBSurface(0, shape->getW() * View::SCALE, shape->getH() * View::SCALE, 32, 0, 0, 0, 0);
#endif

    SDL_Rect rect;
    rect.w = View::SCALE;
    rect.h = View::SCALE;

    SDL_Color color;
    prepareColor(&color, shape, weight);

    FShape::const_iterator end = shape->marksEnd();
    for (FShape::const_iterator i = shape->marksBegin(); i != end; ++i) {
        rect.x = i->getX() * View::SCALE;
        rect.y = i->getY() * View::SCALE;
#if DANDAN
        SurfaceTool::alphaFill(surface, &rect, color);
#else
        SDL_FillRect(surface, &rect, color);
#endif
    }

    return surface;
}

