//
// Created by Ion Agorria on 1/11/18
//
#ifndef OPENE2140_SIMULATION_H
#define OPENE2140_SIMULATION_H

#include "engine/core/types.h"
#include "engine/core/macros.h"
#include "entity.h"
#include "simulation_parameters.h"
#include "engine/assets/asset_manager.h"
#include "engine/io/log.h"

struct EntityPrototype;
class Faction;
class Player;
class Engine;
class World;
class Renderer;
class AssetLevel;
class EntityStore;

/**
 * Contains everything inside the running game
 */
class Simulation: public IErrorPossible {
private:
    /**
     * Log for object
     */
    log_ptr log;

    /**
     * Simulation setup parameters
     */
    std::unique_ptr<SimulationParameters> parameters;

    /**
     * Engine object
     */
    std::shared_ptr<Engine> engine;

    /**
     * Asset level used for this simulation
     */
    AssetLevel* assetLevel;

    /**
     * Entities contained by this simulation
     */
    std::unique_ptr<EntityStore> entityStore;

    /**
     * World for this simulation
     */
    std::unique_ptr<World> world;

    /**
     * Factions for this simulation
     */
    std::vector<std::unique_ptr<Faction>> factions;

    /**
     * Players for this simulation
     */
    std::vector<std::unique_ptr<Player>> players;

public:
    /**
     * Flag for enabling debugging entities
     */
    bool debugEntities = false;

    /**
     * World tile size
     */
    int tileSize = 0;

    /**
     * World tile size half
     */
    int tileSizeHalf = 0;

    /**
     * Constructor
     */
    Simulation(std::shared_ptr<Engine> engine, std::unique_ptr<SimulationParameters> parameters);

    /**
     * Destructor
     */
    ~Simulation() override;

    /**
     * Disable copy
     */
    NON_COPYABLE(Simulation)

    /**
     * Called when simulation should load the world
     */
    virtual void loadWorld();

    /**
     * Called when simulation should load players
     */
    void loadPlayers();

    /**
     * Called when simulation should load entities
     */
    void loadEntities();

    /**
     * Called when simulation is being updated
     */
    virtual void update();

    /**
     * Called when simulation is being closed
     */
    virtual void close();

    /**
     * Draws this simulation
     *
     * @param rectangle the visible rectangle to draw
     * @param visibleEntities stores the drawn entities
     */
    virtual void draw(const Rectangle& rectangle, std::vector<std::shared_ptr<Entity>>& visibleEntities);

    /**
     * @return entities store in simulation
     */
    EntityStore* getEntitiesStore() const;

    /**
     * @return World instance in simulation
     */
    World* getWorld() const;

    /**
     * Creates a new entity and adds to simulation
     */
    std::shared_ptr<Entity> createEntity(const EntityPrototype& entityPrototype);

    /**
     * Creates a new entity and optionally adds to simulation
     */
    std::shared_ptr<Entity> createEntity(const entity_type_t& entityType, bool addToSimulation = false);

    /**
     * Creates a new entity and optionally adds to simulation
     */
    std::shared_ptr<Entity> createEntity(entity_kind_t entityKind, const std::string& entityCode, bool addToSimulation = false);

    /**
     * Adds entity to simulation
     */
    void addEntity(const std::shared_ptr<Entity>& entity);

    /**
     * Removes entity from simulation
     */
    void removeEntity(const std::shared_ptr<Entity>& entity);

    /**
     * @return current renderer
     */
    Renderer* getRenderer() const;

    /**
     * Adds a new faction to simulation
     *
     * @param faction to add
     */
    void addFaction(std::unique_ptr<Faction> faction);

    /**
     * @return factions in simulation
     */
    std::vector<std::unique_ptr<Faction>>& getFactions();

    /**
     * Obtain the faction from ID
     *
     * @param id of faction
     * @return faction or null if none found
     */
    Faction* getFaction(faction_id_t id) const;

    /**
     * Obtain the faction from code
     *
     * @param code of faction
     * @return faction or null if none found
     */
    Faction* getFaction(const std::string& code) const;

    /**
     * Adds a new player to simulation
     *
     * @param player to add
     */
    void addPlayer(std::unique_ptr<Player> player);

    /**
     * @return players in simulation
     */
    std::vector<std::unique_ptr<Player>>& getPlayers();

    /**
     * Obtain the player from ID
     *
     * @param id of player
     * @return player or null if none found
     */
    Player* getPlayer(player_id_t id) const;

    /*
     * World helpers
     */

    /**
     * Translates world vector to tile vector
     *
     * @param vector to convert
     * @param result to write result
     * @return tile if valid
     */
    void toTileVector(const Vector2& vector, Vector2& result) const;

    /**
     * Translates tile vector to world vector
     *
     * @param vector to convert
     * @param result to write result
     * @param center should the position be centered to tile?
     * @return tile if valid
     */
    void toWorldVector(const Vector2& vector, Vector2& result, bool center) const;

    /**
     * Translates world rectangle to tile rectangle
     *
     * @param rectangle to convert
     * @param result to write result
     * @return tile if valid
     */
    void toTileRectangle(const Rectangle& rectangle, Rectangle& result) const;

    /**
     * Translates tile rectangle to world rectangle
     *
     * @param rectangle to convert
     * @param result to write result
     * @param center should the position be centered to tile?
     * @return tile if valid
     */
    void toWorldRectangle(const Rectangle& rectangle, Rectangle& result, bool center) const;

    /**
     * Calculates the angle between the origin and destination in world coordinates
     *
     * @param origin world vector
     * @param destination world vector
     * @return angle
     */
    number_t angleWorldVectors(const Vector2& origin, const Vector2& destination) const;

    /**
     * Calculates the angle between the origin and destination in tile coordinates
     *
     * @param origin tile vector
     * @param destination tile vector
     * @return angle
     */
    number_t angleTileVectors(const Vector2& origin, const Vector2& destination) const;

    /*
     * AssetManager proxy
     */

    /**
     * Gets the loaded image from an asset
     *
     * @return image
     */
    Image* getImage(const asset_path_t& path) const;
};

#endif //OPENE2140_SIMULATION_H
