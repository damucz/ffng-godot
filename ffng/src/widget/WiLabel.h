#ifndef HEADER_WILABEL_H
#define HEADER_WILABEL_H

class FFont;

#include "WiPicture.h"

#include "GodotSDL.h"
#include <string>

/**
 * Constant text label.
 */
class WiLabel : public WiPicture {
    public:
        WiLabel(const std::string &text, const FFont &font,
                const SDL_Color &color);
};

#endif
