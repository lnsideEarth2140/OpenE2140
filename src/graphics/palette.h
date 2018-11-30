//
// Created by Ion Agorria on 29/04/18
//
#ifndef OPENE2140_PALETTE_H
#define OPENE2140_PALETTE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <SDL_opengl.h>
#include "core/config.h"
#include "core/errorpossible.h"
#include "ipalette.h"

/**
 * Palette implementation which uses array
 */
class Palette : public IPalette {
protected:
    /**
     * Internal palette colors
     */
    std::vector<ColorRGBA> colors;

    /**
     * Texture reference containing this palette data
     */
    GLuint texture = 0;

    /**
     * Checks if image is correct
     *
     * @return true if OK
     */
    bool check();

public:
    /**
     * Constructor for empty palette
     */
    Palette(unsigned int size, bool extra);

    /**
     * Palette destructor
     */
    ~Palette();

    /**
     * Disable copy/move
     */
    NON_COPYABLE_NOR_MOVABLE(Palette)

    /**
     * @return if palette is valid
     */
    operator bool();

    unsigned long length() const override;

    bool getColorVirtual(unsigned int index, ColorRGBA& color) override;

    bool setColorVirtual(unsigned int index, ColorRGBA& color) override;

    /**
     * Binds the texture for use
     */
    bool bindTexture();
};

#endif //OPENE2140_PALETTE_H