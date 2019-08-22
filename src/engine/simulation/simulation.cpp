//
// Created by Ion Agorria on 1/11/18
//

#include "engine/core/utils.h"
#include "engine/core/engine.h"
#include "engine/graphics/renderer.h"
#include "faction.h"
#include "player.h"
#include "entities/entity.h"
#include "components/player_component.h"
#include "entities/entity_manager.h"
#include "world/world.h"
#include "engine/assets/asset.h"
#include "engine/assets/asset_level.h"
#include "simulation.h"

Simulation::Simulation(std::shared_ptr<Engine> engine, std::unique_ptr<SimulationParameters> parameters):
        parameters(std::move(parameters)), engine(std::move(engine)) {
    log = Log::get("Simulation");
    debugEntities = Utils::isFlag(FLAG_DEBUG_ALL);
    if (!this->parameters || this->parameters->world.empty()) {
        error = "Parameters not set";
        return;
    }
    //Load asset
    assetLevel = this->engine->getAssetManager()->getAsset<AssetLevel>(this->parameters->world);
    if (!assetLevel) {
        error = "World asset not found";
        return;
    }
}

void Simulation::loadWorld() {
    log->debug("Name: '" + assetLevel->name() + "'");

    //Load tileset
    std::unordered_map<unsigned int, Image*> tilesetImages;
    size_t tilesetSize = assetLevel->tilesetSize();
    for (size_t i = 0; i < tilesetSize; ++i) {
        asset_path_t path = assetLevel->tilePath(i);
        if (!path.empty()) {
            tilesetImages[i] = getImage(path);
        }
    }
    error = assetLevel->getError();
    if (hasError()) {
        return;
    }

    //Create world
    world = std::make_unique<World>(assetLevel, tilesetImages);
    error = world->getError();
    if (hasError()) {
        return;
    }
}

void Simulation::loadPlayers() {
    //Load players from parameters
    for (auto& parametersPlayer : parameters->players) {
        if (parametersPlayer) {
            addPlayer(std::move(parametersPlayer));
        }
    }

    //Load players from level (overwrites existing players or creates news)
    if (parameters->loadLevelContent) {
        std::vector<PlayerPrototype> levelPlayers;
        assetLevel->players(levelPlayers);
        error = assetLevel->getError();
        if (hasError()) {
            return;
        }
        for (PlayerPrototype& playerPrototype : levelPlayers) {
            Faction* faction = nullptr;
            if (playerPrototype.faction) {
                faction = getFaction(playerPrototype.faction);
            }

            Player* player = getPlayer(playerPrototype.id);
            if (!player) {
                std::unique_ptr playerPtr = std::make_unique<Player>(playerPrototype.id);
                player = playerPtr.get();
                addPlayer(std::move(playerPtr));
            }
            player->enemies = playerPrototype.enemies;
            player->money = playerPrototype.money;
            player->faction = faction;
        }
    }
}

void Simulation::loadEntities() {
    //Load entities from level
    if (parameters->loadLevelContent) {
        std::vector<EntityPrototype> levelEntities;
        assetLevel->entities(levelEntities);
        error = assetLevel->getError();
        if (hasError()) {
            return;
        }
        for (EntityPrototype& entityPrototype : levelEntities) {
            if (!entityPrototype.exists) {
                //TODO these should be stored for later use
                continue;
            }
            createEntity(entityPrototype);
        }
    }
}

Simulation::~Simulation() {
    close();
}

void Simulation::close() {
    log->debug("Closing");
    std::vector<std::shared_ptr<Entity>> toRemove(entities);
    for (std::shared_ptr<Entity>& entity : toRemove) {
        entity->removedFromSimulation();
    }
    entities.clear();
    if (world) {
        world.reset();
    }
}

void Simulation::update() {
    world->update();
    for (std::shared_ptr<Entity>& entity : entities) {
        entity->update();
    }
}

void Simulation::draw(const Rectangle& viewport) {
    //Obtain a slightly bigger rectangle
    int tileSize = world->tileSize;
    Rectangle rectangle(
        viewport.x - tileSize,
        viewport.y - tileSize,
        viewport.w + tileSize * 2,
        viewport.h + tileSize * 2
    );

    //Draw world
    Renderer* renderer = getRenderer();
    world->draw(renderer, rectangle);

    //Draw entities
    for (std::shared_ptr<Entity>& entity : entities) {
        const Rectangle& bounds = entity->getBounds();
        if (rectangle.isOverlap(bounds)) {
            entity->draw();
            if (debugEntities) {
                renderer->drawRectangle(bounds, 2, Color::DEBUG_ENTITIES);
            }
        }
    }
}

const std::vector<std::shared_ptr<Entity>>& Simulation::getEntities() const {
    return entities;
}

World* Simulation::getWorld() const {
    return world.get();
}

entity_id_t Simulation::nextEntityID() {
    lastEntityID++;
    return lastEntityID;
}

std::shared_ptr<Entity> Simulation::createEntity(const EntityPrototype& entityPrototype) {
    std::shared_ptr<Entity> entityPtr = engine->getEntityManager()->makeEntity(entityPrototype.type);
    if (entityPtr) {
        Entity* entity = entityPtr.get();
        //Basic stuff
        Vector2 position;
        world->toWorldVector(entityPrototype.position, position);
        entity->setPosition(position);
        entity->setDirection(entityPrototype.direction);
        entity->setDisable(entityPrototype.disabled);
        if (entityPrototype.player) {
            //Obtain player and component
            PlayerComponent* component = GET_COMPONENT(entity, PlayerComponent);
            Player* player = getPlayer(entityPrototype.player);
            if (player && component) {
                component->setPlayer(player);
            }
        }
        addEntity(entityPtr);
    }
    return entityPtr;
}

std::shared_ptr<Entity> Simulation::createEntity(const entity_type_t& entityType) {
    std::shared_ptr<Entity> entity = engine->getEntityManager()->makeEntity(entityType);
    if (entity) {
        addEntity(entity);
    }
    return entity;
}

void Simulation::addEntity(const std::shared_ptr<Entity>& entity) {
    if (entity->isActive()) {
        log->warn("Attempted to add already active entity {0} to simulation", entity->getID());
    }
    entities.emplace_back(entity);
    entity->addedToSimulation(this);
}

void Simulation::removeEntity(const std::shared_ptr<Entity>& entity) {
    if (!entity->isActive()) {
        log->warn("Attempted to remove non active entity {0} from simulation", entity->getID());
    }
    Utils::eraseElementFromVector(entities, entity);
    entity->removedFromSimulation();
}

Renderer* Simulation::getRenderer() const {
    return engine->getRenderer();
}

void Simulation::addFaction(std::unique_ptr<Faction> faction) {
    faction_id_t id = faction->id;
    //Allocate new empty if not enough are present
    if (factions.size() < static_cast<faction_id_t>(id + 1)) {
        factions.resize(id + 1);
    }
    factions[id].swap(faction);
}

std::vector<std::unique_ptr<Faction>>& Simulation::getFactions() {
    return factions;
}

Faction* Simulation::getFaction(faction_id_t id) const {
    return id < factions.size() ? factions[id].get() : nullptr;
}

Faction* Simulation::getFaction(const std::string& code) const {
    for (const std::unique_ptr<Faction>& faction : factions) {
        if (faction && faction->code == code) {
            return faction.get();
        }
    }
    return nullptr;
}

void Simulation::addPlayer(std::unique_ptr<Player> player) {
    player_id_t id = player->id;
    //Allocate new empty if not enough are present
    if (players.size() < static_cast<player_id_t>(id + 1)) {
        players.resize(id + 1);
    }
    players[id].swap(player);
}

std::vector<std::unique_ptr<Player>>& Simulation::getPlayers() {
    return players;
}

Player* Simulation::getPlayer(player_id_t id) const {
    return id < players.size() ? players[id].get() : nullptr;
}

Image* Simulation::getImage(const asset_path_t& path) const {
    return engine->getAssetManager()->getImage(path).get();
}
