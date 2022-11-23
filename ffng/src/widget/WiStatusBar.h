#ifndef HEADER_WISTATUSBAR_H
#define HEADER_WISTATUSBAR_H

class FFont;

#include "IWidget.h"

#include "GodotSDL.h"
#include <string>

/**
 * Statusbar with fixed width.
 */
class WiStatusBar : public IWidget {
    private:
        FFont *m_font;
        SDL_Color m_color;
        int m_w;
        std::string m_label;
    public:
        WiStatusBar(FFont *new_font, const SDL_Color &color, int width=0);
        virtual ~WiStatusBar();
        virtual int getW() const { return m_w; }
        virtual int getH() const;
        void setLabel(const std::string &label) { m_label = label; }

        virtual void drawOn(SDL_Surface *screen);
};

#endif
