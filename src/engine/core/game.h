//
// Created by Ion Agorria on 1/11/18
//
#ifndef OPENE2140_GAME_H
#define OPENE2140_GAME_H

#include <memory>
#include "src/engine/core/common.h"
#include "io/log.h"

/**
 * Contains the central game code that calls and coordinates the subsystems
 */
class AssetManager;
class Window;
class EventHandler;
class Renderer;
class Simulation;
class Game: public std::enable_shared_from_this<Game> {
    /**
     * Log for game
     */
    log_ptr log;

    /**
     * Manager for asset fetching
     */
    std::unique_ptr<AssetManager> assetManager;

    /**
     * Main window
     */
    std::unique_ptr<Window> window;

    /**
     * Stores the game simulation state
     */
    std::unique_ptr<Simulation> simulation;

    /**
     * Event handler to receive window events
     */
    std::unique_ptr<EventHandler> eventHandler;

    /**
     * Renderer which handles the drawing of game
     */
    std::unique_ptr<Renderer> renderer;
public:
    /**
     * Game constructor
     */
    Game();

    /**
     * Game destructor
     */
    ~Game();

    /**
     * Game close method
     */
    void close();

    /**
     * Disable copy/move
     */
    NON_COPYABLE_NOR_MOVABLE(Game)

    /**
     * Get shared pointer to this game
     *
     * @return shared game pointer
     */
    std::shared_ptr<Game> this_shared_ptr() {
        return shared_from_this();
    }

    /**
     * Initializes game
     *
     * @return true if success
     */
    bool run();

    /**
     * Main loop for game
     */
    void loop();

    /**
     * @return main window
     */
    Window* getWindow();

    /** @return Renderer */
    Renderer* getRenderer();

    /** @return Simulation */
    Simulation* getSimulation();

    /** @return AssetManager */
    AssetManager* getAssetManager();

    void test(int i);
};

#endif //OPENE2140_GAME_H
