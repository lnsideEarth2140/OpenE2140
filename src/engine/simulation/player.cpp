//
// Created by Ion Agorria on 3/08/19
//

#include "engine/core/macros.h"
#include "engine/io/log.h"
#include "faction.h"
#include "player.h"

Player::Player(player_id_t id):
    id(id), mask(BIT_MASK(id)) {
    pathHandler = std::make_unique<PathHandler>(this);
}

bool Player::isEnemy(const Player* other) const {
    if (!other) return false;
    return BIT_STATE(enemies, other->mask);
}

void Player::addEnemy(const Player* other) {
    BIT_ON(enemies, other->mask);
}

void Player::removeEnemy(const Player* other) {
    BIT_OFF(enemies, other->mask);
}

void Player::update() {
    energyPool = energyGeneration;
    energyGeneration = 0;
    pathHandler->update();
}