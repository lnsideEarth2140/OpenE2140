//
// Created by Ion Agorria on 12/07/19
//
#ifndef OPENE2140_PLAYER_H
#define OPENE2140_PLAYER_H

#include "engine/simulation/pathfinder/path_handler.h"
#include "engine/graphics/color.h"
#include "engine/core/macros.h"

class Faction;
class Simulation;

/**
 * Contains Player related data
 */
class Player {
public:
    /**
     * ID for the player
     */
    const player_id_t id = 0;

    /**
     * Mask of this player based on id
     */
    const player_mask_t mask = 0;

    /**
     * Energy generated in previous update
     * To be incremented by entities
     */
    entity_energy_t energyGeneration = 0;

    /**
     * Energy pool available considering the energy generation
     * Used by entities
     */
    entity_energy_t energyPool = 0;

    /**
     * Main color for this player
     */
    ColorRGBA color = Color::WHITE;

    /**
     * Extra colors
     */
    std::vector<ColorRGBA> extraColors;

    /**
     * Player own path handler
     */
    std::unique_ptr<PathHandler> pathHandler;

    /**
     * Simulation which player belongs
     */
    Simulation* simulation = nullptr;

    /**
     * Player faction
     */
    Faction* faction = nullptr;

    /**
     * Player enemies mask, those which are enemies are marked as 1 in their bit position (determined at mask)
     */
    player_mask_t enemies = 0;

    /**
     * Name for this player
     */
    std::string name = "";

    /**
     * Player money amount
     */
    money_t money = 0;

    /**
     * Constructor
     */
    explicit Player(player_id_t id);

    /**
     * Destructor
     */
    virtual ~Player() = default;

    /**
     * Macros
     */
    NON_COPYABLE(Player)
    TYPE_NAME(Player)

    /**
     * Returns if this player considers the other as enemy
     *
     * @param other player which to check with this player
     * @return true if enemy
     */
     bool isEnemy(const Player* other) const;

    /**
     * Adds player as this player enemy
     */
    void addEnemy(const Player* other);

    /**
     * Removes player as this player enemy
     */
    void removeEnemy(const Player* other);

    /**
     * Handle the player energy
     */
    void update();
};

#endif //OPENE2140_PLAYER_H
