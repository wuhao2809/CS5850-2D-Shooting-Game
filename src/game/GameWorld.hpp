#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "Timer.hpp"
#include "ecs/Entity.hpp"
#include "ecs/ComponentManager.hpp"
#include "ecs/SystemManager.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/Sprite.hpp"
#include "ecs/components/Movement.hpp"
#include "ecs/components/Input.hpp"
#include "ecs/components/Images.hpp"
#include "ecs/components/Collision.hpp"
#include "ecs/components/Player.hpp"
#include "ecs/components/ShootingGalleryState.hpp"
#include "ecs/systems/MovementSystem.hpp"
#include "ecs/systems/RenderSystem.hpp"
#include "ecs/systems/EventSystem.hpp"
#include "ecs/systems/UIEventSystem.hpp"
#include "ecs/systems/CollisionSystem.hpp"
#include "ecs/systems/PlayerControlSystem.hpp"
#include "ecs/systems/TargetSpawnSystem.hpp"
#include "ecs/systems/DuckMovementSystem.hpp"
#include "ecs/systems/ProjectileSystem.hpp"
#include "ecs/systems/GameStateSystem.hpp"
#include "ecs/systems/ExpiredEntitiesSystem.hpp"
#include "events/EventManager.hpp"
#include <SDL3/SDL.h>
#include <fstream>
#include <unordered_map>
#include <locale>

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

    // Debug methods
    std::unordered_map<std::string, ecs::components::Collision*> getCollisionComponents() const;

    // World dimension getters
    int getWorldWidth() const { return worldWidth; }
    int getWorldHeight() const { return worldHeight; }

private:
    GameWorld();  // Constructor declared here, defined in cpp
    ~GameWorld(); // Destructor declared here, defined in cpp

    std::string assetsDir;
    int worldWidth;
    int worldHeight;
    SDL_Renderer* renderer;
    std::unique_ptr<Timer> gameTimer;  // Timer instance for hardware-independent timing
    std::vector<ecs::Entity> entities;  // Maintains insertion order
    std::map<std::string, size_t> entityIndices;  // For quick lookups by ID
    ecs::ComponentManager& componentManager;
    events::EventManager& eventManager;
    
    // Core systems
    ecs::systems::UIEventSystem* uiEventSystem;
    ecs::systems::MovementSystem* movementSystem;
    ecs::systems::RenderSystem* renderSystem;
    ecs::systems::EventSystem* eventSystem;
    ecs::systems::CollisionSystem* collisionSystem;
    ecs::systems::PlayerControlSystem* playerControlSystem;
    
    // Game-specific systems  
    ecs::systems::TargetSpawnSystem* targetSpawnSystem;
    ecs::systems::DuckMovementSystem* duckMovementSystem;
    ecs::systems::ProjectileSystem* projectileSystem;
    ecs::systems::GameStateSystem* gameStateSystem;
    ecs::systems::ExpiredEntitiesSystem* expiredEntitiesSystem;
    
    void logSystemStates();

    void createEntityFromJson(const nlohmann::json& data);
};

} // namespace game 