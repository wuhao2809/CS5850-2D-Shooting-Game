#include "GameWorld.hpp"
#include "GameColor.hpp"
#include "ecs/systems/UIEventSystem.hpp"
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <iostream>
#include "resources/ResourceManager.hpp"
#include "Timer.hpp"

using json = nlohmann::json;

namespace game {

GameWorld::GameWorld() 
    : worldWidth(800), 
      worldHeight(600), 
      renderer(nullptr), 
      componentManager(ecs::ComponentManager::getInstance()),
      eventManager(events::EventManager::getInstance()) {
    
    // Initialize locale for Windows
#ifdef _WIN32
    try {
        std::locale::global(std::locale::classic());
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Initialized classic locale");
    } catch (const std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Failed to initialize locale: %s", e.what());
    }
#endif
}

GameWorld::~GameWorld() {
    // Cleanup locale for Windows
#ifdef _WIN32
    try {
        std::locale::global(std::locale::classic());
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Cleaned up locale");
    } catch (const std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Failed to cleanup locale: %s", e.what());
    }
#endif
}

void GameWorld::setAssetsDirectory(const std::string& directory) {
    assetsDir = directory;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Set assets directory to: %s", directory.c_str());
    
    // Set the assets directory for the ResourceManager
    auto& resourceManager = resources::ResourceManager::getInstance();
    resourceManager.setAssetsDirectory(directory);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Set ResourceManager assets directory to: %s", directory.c_str());
}

void GameWorld::setRenderer(SDL_Renderer* renderer) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Setting renderer: %p", renderer);
    this->renderer = renderer;
    
    // Also update the RenderSystem if it exists
    if (renderSystem) {
        renderSystem->setRenderer(renderer);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Updated RenderSystem renderer to: %p", renderer);
    }
}

bool GameWorld::initialize() {
    try {
        // Create Timer instance for hardware-independent timing
        gameTimer = std::make_unique<Timer>(60);  // 60 FPS default
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "[GameWorld] Created Timer instance for hardware-independent timing");
        
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Initializing with renderer: %p", renderer);
        // Get system manager instance
        auto& systemManager = ecs::SystemManager::getInstance();

        // Add core systems in PURE ECS SPECIFICATION ORDER (matches Python/Java implementation)
        // 0. UIEventSystem - Input processing (first to bridge events to components)
        uiEventSystem = systemManager.addSystem<ecs::systems::UIEventSystem>(eventManager);
        
        // 1. PlayerControlSystem - Input handling and shooting
        playerControlSystem = systemManager.addSystem<ecs::systems::PlayerControlSystem>(eventManager, worldWidth, worldHeight);
        
        // 2. GameStateSystem - State and timer management
        gameStateSystem = systemManager.addSystem<ecs::systems::GameStateSystem>();
        
        // 3. TargetSpawnSystem - Duck spawning with weighted templates
        targetSpawnSystem = systemManager.addSystem<ecs::systems::TargetSpawnSystem>(worldWidth, worldHeight);
        
        // 4. DuckMovementSystem - Duck-specific movement patterns
        duckMovementSystem = systemManager.addSystem<ecs::systems::DuckMovementSystem>(worldWidth, worldHeight);
        
        // 5. MovementSystem - General movement (projectiles)
        movementSystem = systemManager.addSystem<ecs::systems::MovementSystem>();
        
        // 6. ProjectileSystem - Projectile lifecycle management (pure ECS, no events)
        projectileSystem = systemManager.addSystem<ecs::systems::ProjectileSystem>();
        
        // 7. CollisionSystem - Collision detection (pure ECS, no events)
        collisionSystem = systemManager.addSystem<ecs::systems::CollisionSystem>();
        
        // 8. ExpiredEntitiesSystem - Entity cleanup (CRITICAL: Must come after all other systems)
        expiredEntitiesSystem = systemManager.addSystem<ecs::systems::ExpiredEntitiesSystem>();
        expiredEntitiesSystem->setSystemManager(&systemManager);
        
        // 9. RenderSystem - Visual rendering (last)
        renderSystem = systemManager.addSystem<ecs::systems::RenderSystem>(renderer);
        
        // Add EventSystem (not part of specification order but needed for events)
        eventSystem = systemManager.addSystem<ecs::systems::EventSystem>(&eventManager);
        
        // Subscribe PlayerControlSystem to keyboard events
        eventManager.subscribe("keyboard", playerControlSystem);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] PlayerControlSystem subscribed to keyboard events");

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Added all systems in pure ECS order: UIEvent (input bridge), PlayerControl, GameState, TargetSpawn, DuckMovement, Movement, Projectile, Collision, ExpiredEntities, Render, Event");

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
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading game data from: %s", filePath.c_str());
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
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loaded world dimensions: %dx%d", worldWidth, worldHeight);
        
        // Set world size for systems that need it (but MovementSystem no longer needs it)
        if (movementSystem) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] MovementSystem initialized without world bounds");
        }
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "No world dimensions found in JSON, using defaults: %dx%d", worldWidth, worldHeight);
    }

    // Load templates for TargetSpawnSystem (matches Python/Java)
    if (json.contains("templates") && targetSpawnSystem) {
        const auto& templatesJson = json["templates"];
        std::unordered_map<std::string, nlohmann::json> templates;
        
        for (auto it = templatesJson.begin(); it != templatesJson.end(); ++it) {
            templates[it.key()] = it.value();
        }
        
        targetSpawnSystem->setTemplates(templates);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Loaded %zu templates for TargetSpawnSystem", templates.size());
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] No templates found in JSON or TargetSpawnSystem not initialized");
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Found %zu entities in JSON", json["entities"].size());

    // Process each entity in the JSON
    for (const auto& entityData : json["entities"]) {
        createEntityFromJson(entityData);
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Finished loading %zu entities", entities.size());

    // Log system states after loading
    logSystemStates();
}

void GameWorld::createEntityFromJson(const nlohmann::json& data) {
    // Get system manager instance
    auto& systemManager = ecs::SystemManager::getInstance();

    // Create entity
    auto entity = ecs::Entity::create();
    std::string entityId = data["id"].get<std::string>();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Creating entity with ID: %s", entityId.c_str());
    
    // Add to vector and store index
    entities.push_back(entity);
    entityIndices[entityId] = entities.size() - 1;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Added entity to entities vector");

    // Notify systems about the new entity BEFORE adding components
    systemManager.onEntityCreated(entity);

    // Log the components data
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Entity %s components data: %s", entityId.c_str(), data["components"].dump().c_str());

    // Add components based on JSON data
    if (data.contains("components") && data["components"].contains("transform")) {
        const auto& t = data["components"]["transform"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding Transform component to entity %s with data: %s", entityId.c_str(), t.dump().c_str());
        componentManager.addComponent<ecs::components::Transform>(
            entity,
            ecs::Vector2(t["position"]["x"].get<float>(), t["position"]["y"].get<float>()),
            t["rotation"].get<float>()
        );
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Transform)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Transform component added");
    }

    if (data.contains("components") && data["components"].contains("sprite")) {
        const auto& s = data["components"]["sprite"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding Sprite component to entity %s with data: %s", entityId.c_str(), s.dump().c_str());
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
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Sprite component added");
    }

    if (data.contains("components") && data["components"].contains("movement")) {
        const auto& m = data["components"]["movement"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding Movement component to entity %s with data: %s", entityId.c_str(), m.dump().c_str());
        const auto& velocityData = m["velocity"];
        const auto& accelerationData = m["acceleration"];
        componentManager.addComponent<ecs::components::Movement>(
            entity,
            ecs::Vector2(velocityData["x"].get<float>(), velocityData["y"].get<float>()),
            ecs::Vector2(accelerationData["x"].get<float>(), accelerationData["y"].get<float>())
        );
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Movement)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Movement component added");
    }

    if (data.contains("components") && data["components"].contains("input")) {
        const auto& i = data["components"]["input"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding Input component to entity %s with data: %s", entityId.c_str(), i.dump().c_str());
        
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
            if (keys.contains("fire")) input->setKey("fire", keys["fire"].get<std::string>());
        }
        
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Input)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Input component added");
    }

    if (data.contains("components") && data["components"].contains("images")) {
        const auto& img = data["components"]["images"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding Images component to entity %s with data: %s", entityId.c_str(), img.dump().c_str());
        
        // Create Images component
        componentManager.addComponent<ecs::components::Images>(entity);
        auto* images = componentManager.getComponent<ecs::components::Images>(entity);
        
        // Add image names
        if (img.contains("imageNames")) {
            for (const auto& imageName : img["imageNames"]) {
                images->addImage(imageName.get<std::string>());
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Added image: %s", imageName.get<std::string>().c_str());
            }
        }
        
        // Set active image if specified
        if (img.contains("activeImage")) {
            images->setCurrentImage(img["activeImage"].get<size_t>());
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Set current image to index: %zu", img["activeImage"].get<size_t>());
        }
        
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Images)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Images component added");
    }

    if (data.contains("components") && data["components"].contains("player")) {
        const auto& p = data["components"]["player"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding Player component to entity %s with data: %s", entityId.c_str(), p.dump().c_str());
        
        // Create Player component with Timer dependency
        float fireRate = p.contains("fireRate") ? p["fireRate"].get<float>() : 0.3f;
        componentManager.addComponent<ecs::components::Player>(entity, gameTimer.get(), fireRate);
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Player)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Player component added with fireRate: %f using hardware-independent timing", fireRate);
    }

    // Create ShootingGalleryState component (matches Python/Java pattern)
    if (data.contains("components") && data["components"].contains("shootingGalleryState")) {
        const auto& sgs = data["components"]["shootingGalleryState"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding ShootingGalleryState component to entity %s with data: %s", entityId.c_str(), sgs.dump().c_str());
        
        // Create ShootingGalleryState singleton instance with Timer dependency
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] About to create ShootingGalleryState instance");
        ecs::components::ShootingGalleryState::createInstance(entity, gameTimer.get());
        auto& gameState = ecs::components::ShootingGalleryState::getInstance();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] ShootingGalleryState instance created successfully");
        
        // Configure from JSON data by setting member variables directly
        if (sgs.contains("gameDuration")) {
            gameState.timeRemaining = sgs["gameDuration"].get<float>();
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Set timeRemaining to %.2f from JSON", gameState.timeRemaining);
        }
        
        // Start the game (matches Python behavior)
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] About to call startGame()");
        gameState.startGame();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] startGame() called successfully");
        
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ShootingGalleryState component created and initialized");
    }

    // Add Collision component only if explicitly defined in JSON
    if (data.contains("components") && data["components"].contains("collision")) {
        const auto& c = data["components"]["collision"];
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Adding Collision component to entity %s with data: %s", entityId.c_str(), c.dump().c_str());
        componentManager.addComponent<ecs::components::Collision>(entity);
        systemManager.onComponentAdded(entity, std::type_index(typeid(ecs::components::Collision)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Collision component added");
    }
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Finished creating entity %s", entityId.c_str());
}

void GameWorld::update(float deltaTime) {
    // Update event manager first
    eventManager.update();
    // Then update all systems
    auto& systemManager = ecs::SystemManager::getInstance();
    const auto& systems = systemManager.getSystems();
    for (const auto& systemPtr : systems) {
        size_t entityCount = systemPtr->getEntities().size();
        const std::type_info& type = typeid(systemPtr);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] System %s has %zu entities", type.name(), entityCount);
        systemPtr->update(deltaTime);
    }
}

void GameWorld::render() {
    if (!renderer) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Cannot render: renderer not set");
        return;
    }
    
    // Log current state
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Rendering world with %zu entities", entities.size());
    for (const auto& entity : entities) {
        auto* transform = componentManager.getComponent<ecs::components::Transform>(entity);
        auto* sprite = componentManager.getComponent<ecs::components::Sprite>(entity);
        if (transform && sprite) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Entity: pos=(%.1f, %.1f), size=(%.1fx%.1f), visible=%d",
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
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] Logging all system entity counts after file load:");
    for (const auto& systemPtr : systemManager.getSystems()) {
        size_t entityCount = systemPtr->getEntities().size();
        const std::type_info& type = typeid(systemPtr);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameWorld] System %s has %zu entities", type.name(), entityCount);
    }
}

std::unordered_map<std::string, ecs::components::Collision*> GameWorld::getCollisionComponents() const {
    std::unordered_map<std::string, ecs::components::Collision*> collisionMap;
    
    for (size_t i = 0; i < entities.size(); ++i) {
        const auto& entity = entities[i];
        auto* collision = componentManager.getComponent<ecs::components::Collision>(entity);
        if (collision) {
            // Create a unique ID for the entity (use index as string for now)
            std::string entityId = "entity_" + std::to_string(i);
            collisionMap[entityId] = collision;
        }
    }
    
    return collisionMap;
}

} // namespace game 