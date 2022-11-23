#ifndef HEADER_RESCOLORPACK_H
#define HEADER_RESCOLORPACK_H

#include "ResourcePack.h"
#include "FColor.h"

/**
 * Pack of color aliases.
 */
class ResColorPack : public ResourcePack<FColor*> {
    public:
        virtual const char *getName() const { return "color_pack"; }
        virtual void unloadRes(FColor *res) { delete res; }
};

#endif

