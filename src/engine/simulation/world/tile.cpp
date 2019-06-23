//
// Created by Ion Agorria on 20/05/18
//
#include "tile.h"

Tile::Tile(tile_index_t index, Vector2& position): index(index), position(position) {
}

void Tile::setPrototype(TilePrototype prototype) {
    this->tilesetIndex = prototype.tilesetIndex;
    this->tileFlags = prototype.tileFlags;
    this->ore = prototype.ore;
}

