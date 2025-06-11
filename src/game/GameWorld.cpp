#include "GameWorld.hpp"
#include "GameColor.hpp"
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

namespace game {

GameWorld::GameWorld() 
    : worldWidth(800), 
      worldHeight(600), 
      renderer(nullptr), 
      componentManager(ecs::ComponentManager::getInstance()),
      eventManager(events::EventManager::getInstance()) {
}

void GameWorld::setAssetsDirectory(const std::string& directory) {
    assetsDir = directory;
    SDL_Log("Set assets directory to: %s", directory.c_str());
}

void GameWorld::setRenderer(SDL_Renderer* renderer) {
    SDL_Log("[GameWorld] Setting renderer: %p", renderer);
    this->renderer = renderer;
}

bool GameWorld::initialize() {
    try {
        SDL_Log("[GameWorld] Initializing with renderer: %p", renderer);
        // Get system manager instance
        auto& systemManager = ecs::SystemManager::getInstance();

        // Add systems
        movementSystem = systemManager.addSystem<ecs::systems::MovementSystem>(worldWidth, worldHeight);
        renderSystem = systemManager.addSystem<ecs::systems::RenderSystem>();
        eventSystem = systemManager.addSystem<ecs::systems::EventSystem>(&eventManager);  

        // Load game data from JSON
        loadFromJson(assetsDir + "/GameData.json");
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to initialize GameWorld: " << e.what() << std::endl;
        return false;
    }
}

void GameWorld::loadFromJson(const std::string& filePath) {
    SDL_Log("Loading game data from: %s", filePath.c_str());
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    // Read file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // Parse JSON
    auto json = nlohmann::json::parse(content);
    
    // Load world dimensions from JSON
    if (json.contains("world")) {
        worldWidth = json["world"]["width"].get<int>();
        worldHeight = json["world"]["height"].get<int>();
        SDL_Log("Loaded world dimensions: %dx%d", worldWidth, worldHeight);
    } else {
        SDL_Log("No world dimensions found in JSON, using defaults: %dx%d", worldWidth, worldHeight);
    }

    SDL_Log("Found %zu entities in JSON", json["entities"].size());

    // Process each entity in the JSON
    for (const auto& entityData : json["entities"]) {
        createEntityFromJson(entityData);
    }
    SDL_Log("Finished loading %zu entities", entities.size());

    // Log system states after loading
    logSystemStates();
}

void GameWorld::createEntityFromJson(const nlohmann::json& data) {
    // Get system manager instance
    auto& systemManager = ecs::SystemManager::getInstance();

    // Create entity
    auto entity = ecs::Entity::create();
    std::string entityId = data["id"].get<std::string>();
    SDL_Log("Creating entity with ID: %s", entityId.c_str());
    
    // Add to vector and store index
    entities.push_back(entity);
    entityIndices[entityId] = entities.size() - 1;
    SDL_Log("Added entity to entities vector");

    // Notify systems about the new entity BEFORE adding components
    systemManager.onEntityCreated(entity);

    // Log the components data
    SDL_Log("Entity %s components data: %s", entityId.c_str(), data["components"].dump().c_str());

    // Add components based on JSON data
    if (data.contains("components") && data["components"].contains("transform")) {
        const auto& t = data["components"]["transform"];
        SDL_Log("Adding Transform component to entity %s with data: %s", entityId.c_str(), t.dump().c_str());
        componentManager.addComponent<ecs::components::Transform>(
            entity,
            ecs::Vector2(t["position"]["x"].get<float>(), t["position"]["y"].get<float>()),
            t["rotation"].get<float>()
        );
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Transform)));
        SDL_Log("Transform component added");
    }

    if (data.contains("components") && data["components"].contains("sprite")) {
        const auto& s = data["components"]["sprite"];
        SDL_Log("Adding Sprite component to entity %s with data: %s", entityId.c_str(), s.dump().c_str());
        componentManager.addComponent<ecs::components::Sprite>(
            entity,
            s["width"].get<float>(),
            s["height"].get<float>(),
            GameColor(
                s["color"]["r"].get<int>(),
                s["color"]["g"].get<int>(),
                s["color"]["b"].get<int>()
            )
        );
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Sprite)));
        SDL_Log("Sprite component added");
    }

    if (data.contains("components") && data["components"].contains("movement")) {
        const auto& m = data["components"]["movement"];
        SDL_Log("Adding Movement component to entity %s with data: %s", entityId.c_str(), m.dump().c_str());
        const auto& velocityData = m["velocity"];
        const auto& accelerationData = m["acceleration"];
        componentManager.addComponent<ecs::components::Movement>(
            entity,
            ecs::Vector2(velocityData["x"].get<float>(), velocityData["y"].get<float>()),
            ecs::Vector2(accelerationData["x"].get<float>(), accelerationData["y"].get<float>())
        );
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Movement)));
        SDL_Log("Movement component added");
    }

    if (data.contains("components") && data["components"].contains("input")) {
        const auto& i = data["components"]["input"];
        SDL_Log("Adding Input component to entity %s with data: %s", entityId.c_str(), i.dump().c_str());
        
        // Create Input component with default values
        componentManager.addComponent<ecs::components::Input>(entity);
        auto* input = componentManager.getComponent<ecs::components::Input>(entity);
        
        // Override with JSON values if present
        if (i.contains("enabled")) {
            input->setEnabled(i["enabled"].get<bool>());
        }
        if (i.contains("moveSpeed")) {
            input->setMoveSpeed(i["moveSpeed"].get<float>());
        }
        if (i.contains("keys")) {
            const auto& keys = i["keys"];
            if (keys.contains("up")) input->setKey("up", keys["up"].get<std::string>());
            if (keys.contains("down")) input->setKey("down", keys["down"].get<std::string>());
            if (keys.contains("left")) input->setKey("left", keys["left"].get<std::string>());
            if (keys.contains("right")) input->setKey("right", keys["right"].get<std::string>());
        }
        
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Input)));
        SDL_Log("Input component added");
    }
    
    SDL_Log("Finished creating entity %s", entityId.c_str());
}

void GameWorld::update(float deltaTime) {
    // Update event manager first
    eventManager.update();
    // Then update all systems
    auto& systemManager = ecs::SystemManager::getInstance();
    systemManager.update(deltaTime);
}

void GameWorld::render() {
    if (!renderer) {
        SDL_Log("Cannot render: renderer not set");
        return;
    }
    
    // Log current state
    SDL_Log("Rendering world with %zu entities", entities.size());
    for (const auto& entity : entities) {
        auto* transform = componentManager.getComponent<ecs::components::Transform>(entity);
        auto* sprite = componentManager.getComponent<ecs::components::Sprite>(entity);
        if (transform && sprite) {
            SDL_Log("Entity: pos=(%.1f, %.1f), size=(%.1fx%.1f), visible=%d",
                transform->getPosition().x,
                transform->getPosition().y,
                sprite->getWidth(),
                sprite->getHeight(),
                sprite->isVisible()
            );
        }
    }
    
    // Update render system
    auto& systemManager = ecs::SystemManager::getInstance();
    systemManager.update(0.0f);  // Use 0.0f for rendering
}

void GameWorld::clear() {
    entities.clear();
    entityIndices.clear();
    componentManager.reset();
}

size_t GameWorld::getEntityCount() const {
    return entities.size();
}

const std::vector<ecs::Entity>& GameWorld::getEntities() const {
    return entities;
}

ecs::Entity GameWorld::getEntityById(const std::string& id) const {
    auto it = entityIndices.find(id);
    if (it != entityIndices.end()) {
        return entities[it->second];
    }
    throw std::runtime_error("Entity not found: " + id);
}

// Helper to log all system entity counts
void GameWorld::logSystemStates() {
    auto& systemManager = ecs::SystemManager::getInstance();
    SDL_Log("[GameWorld] Logging all system entity counts after file load:");
    // We need access to the systems vector, so add a public method in SystemManager if not present
    // For now, we assume a method: getSystems()
    // If not present, you will need to add it in SystemManager.hpp/cpp
    // Example:
    // const std::vector<std::unique_ptr<System>>& getSystems() const { return systems_; }
    for (const auto& systemPtr : systemManager.getSystems()) {
        size_t entityCount = systemPtr->getEntities().size();
        SDL_Log("[GameWorld] System %s has %zu entities", typeid(*systemPtr).name(), entityCount);
    }
}

} // namespace game 