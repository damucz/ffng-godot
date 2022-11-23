#ifndef HEADER_SHAPE_BUILDER_H
#define HEADER_SHAPE_BUILDER_H

class Shape;

#include "Cube.h"

#include "GodotSDL.h"

/**
 * Creator of images from shapes.
 * Useable for testing of new levels.
 */
class ShapeBuilder {
    private:
        /**
        * ShapeBuilder is not meant to be instantiated.
         */
        ShapeBuilder() {}
        static void prepareColor(SDL_Color *color, const FShape *shape,
                Cube::eWeight weight);
        static Uint32 calcShapeHash(const FShape *shape);
    public:
        static SDL_Surface *createImage(const FShape *shape, Cube::eWeight weight);
};

#endif
