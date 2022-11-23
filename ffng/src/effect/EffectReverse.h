#ifndef HEADER_EFFECTREVERSE_H
#define HEADER_EFFECTREVERSE_H

#include "ViewEffect.h"

/**
 * Blit with reversed left and right side.
 */
class EffectReverse : public ViewEffect {
    private:
        RID m_material;
    public:
        static const char *NAME;
        EffectReverse();
        virtual const char* getName() const { return NAME; }
        virtual void blit(SDL_Surface *screen, SDL_Surface *surface,
                int x, int y);
};

#endif
