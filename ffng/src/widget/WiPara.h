#ifndef HEADER_WIPARA_H
#define HEADER_WIPARA_H

class FFont;

#include "VBox.h"

#include "GodotSDL.h"
#include <string>

/**
 * Multi line paragraph.
 */
class WiPara : public VBox {
    public:
        WiPara(const std::string &text, const FFont &font,
                const SDL_Color &color);
};

#endif
