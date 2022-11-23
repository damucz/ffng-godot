#ifndef HEADER_EFFECTMIRROR_H
#define HEADER_EFFECTMIRROR_H

#include "ViewEffect.h"
#include "core/rid.h"

/**
 * Mirror reflect left side.
 */
class EffectMirror : public ViewEffect {
    private:
        static const int MIRROR_BORDER = 3;
        RID m_material;
    public:
        static const char *NAME;
        EffectMirror();
        virtual const char* getName() const { return NAME; }
        virtual void blit(SDL_Surface *screen, SDL_Surface *surface,
                int x, int y);
};

#endif
