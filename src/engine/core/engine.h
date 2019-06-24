//
// Created by Ion Agorria on 1/11/18
//
#ifndef OPENE2140_ENGINE_H
#define OPENE2140_ENGINE_H

#include "engine/core/common.h"
#include "engine/simulation/simulation_parameters.h"
#include "engine/core/error_possible.h"
#include "engine/io/log.h"
#include "engine/math/vector2.h"

//Forward declarations
class AssetManager;
class EntityManager;
class Window;
class EventHandler;
class Renderer;
class Simulation;
class Timer;
class GUIMenu;

/**
 * Contains the central game code that calls and coordinates the subsystems
 */
class Engine: public std::enable_shared_from_this<Engine>, public IErrorPossible {
protected:
    /**
     * Log for game
     */
    log_ptr log;

    /**
     * Camera where player is looking
     */
    Vector2 camera;

    /**
     * Event handler to receive window events
     */
    std::unique_ptr<EventHandler> eventHandler;

    /**
     * Main window
     */
    std::unique_ptr<Window> window;

    /**
     * Renderer which handles the drawing of game
     */
    std::unique_ptr<Renderer> renderer;

    /**
     * Manager for asset fetching
     */
    std::unique_ptr<AssetManager> assetManager;

    /**
     * Manager for entity management
     */
    std::unique_ptr<EntityManager> entityManager;

    /**
     * Stores the simulation state
     */
    std::unique_ptr<Simulation> simulation;

    /**
     * Stores the engine timer
     */
    std::unique_ptr<Timer> timer;

    /**
     * Current active menu if any
     */
    std::unique_ptr<GUIMenu> menu;

    /**
     * Called when engine is requested to close
     */
    virtual void close();

    /**
     * Initializes engine
     */
    virtual void run();

    /**
     * Updates engine data
     */
    void update();

    /**
     * Draws engine data
     */
    void draw();

    /**
     * Called from engine to setup EventHandler
     */
    virtual void setupEventHandler();

    /**
     * Called from engine to setup Window
     */
    virtual void setupWindow();

    /**
     * Called from engine to setup Renderer
     */
    virtual void setupRenderer();

    /**
     * Called from engine to setup AssetManager
     */
    virtual void setupAssetManager();

    /**
     * Called from engine to setup EntityManager
     */
    virtual void setupEntityManager();

    /**
     * Called from engine to setup Simulation
     *
     * @param parameters
     */
    virtual void setupSimulation(std::unique_ptr<SimulationParameters>& parameters);
public:
    /**
     * Main engine entry point, does the basic initializations
     *
     * @param argc number of args
     * @param argv args array
     * @return program exit code
     */
    static int main(int argc, char** argv, std::shared_ptr<Engine> engine);

    /**
     * Constructor
     */
    Engine();

    /**
     * Destructor
     */
    ~Engine();

    /**
     * Disable copy/move
     */
    NON_COPYABLE_NOR_MOVABLE(Engine)

    /**
     * Get shared pointer to this engine
     *
     * @return shared pointer
     */
    template<typename T>
    std::shared_ptr<T> this_shared_ptr() {
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }

    /**
     * @return event handler
     */
    EventHandler* getEventHandler();

    /**
     * @return main Window
     */
    Window* getWindow();

    /**
     * @return Renderer
     */
    Renderer* getRenderer();

    /**
     * @return AssetManager
     */
    AssetManager* getAssetManager();

    /**
     * @return Simulation
     */
    Simulation* getSimulation();

    /**
     * @return camera
     */
    Vector2& getCamera();

    /**
     * Updates the engine camera
     *
     * @param camera to set
     */
    void updateCamera(const Vector2& camera);

    /**
     * @return key code for provided bind
     */
    input_key_code_t getKeyBind(const std::string& name);
};

#endif //OPENE2140_ENGINE_H
