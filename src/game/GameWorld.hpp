#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "ecs/Entity.hpp"
#include "ecs/ComponentManager.hpp"
#include "ecs/SystemManager.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/Sprite.hpp"
#include "ecs/components/Movement.hpp"
#include "ecs/components/Input.hpp"
#include "ecs/systems/MovementSystem.hpp"
#include "ecs/systems/RenderSystem.hpp"
#include "ecs/systems/EventSystem.hpp"
#include "events/EventManager.hpp"
#include <SDL3/SDL.h>
#include <fstream>

namespace game {

class GameWorld {
public:
    static GameWorld& getInstance() {
        static GameWorld instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    GameWorld(const GameWorld&) = delete;
    GameWorld& operator=(const GameWorld&) = delete;

    void setAssetsDirectory(const std::string& directory);
    void setRenderer(SDL_Renderer* renderer);
    SDL_Renderer* getRenderer() const { 
        SDL_Log("[GameWorld] Getting renderer: %p", renderer);
        return renderer; 
    }
    bool initialize();
    void update(float deltaTime);
    void render();
    void clear();
    size_t getEntityCount() const;
    const std::vector<ecs::Entity>& getEntities() const;
    ecs::Entity getEntityById(const std::string& id) const;
    void loadFromJson(const std::string& filePath);

    // World dimension getters
    int getWorldWidth() const { return worldWidth; }
    int getWorldHeight() const { return worldHeight; }

private:
    GameWorld();  // Constructor declared here, defined in cpp
    ~GameWorld() = default;

    std::string assetsDir;
    int worldWidth;
    int worldHeight;
    SDL_Renderer* renderer;
    std::vector<ecs::Entity> entities;  // Maintains insertion order
    std::map<std::string, size_t> entityIndices;  // For quick lookups by ID
    ecs::ComponentManager& componentManager;
    events::EventManager& eventManager;
    ecs::systems::MovementSystem* movementSystem;
    ecs::systems::RenderSystem* renderSystem;
    ecs::systems::EventSystem* eventSystem;
    void logSystemStates();

    void createEntityFromJson(const nlohmann::json& data);
};

} // namespace game 