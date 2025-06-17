#pragma once

#include "../System.hpp"
#include "../Entity.hpp"
#include "../components/Transform.hpp"
#include "../components/Player.hpp"
#include "../components/Input.hpp"
#include "../components/KeyboardInput.hpp"
#include "../components/Sprite.hpp"
#include "../components/Movement.hpp"
#include "../components/Projectile.hpp"
#include "../components/Collision.hpp"
#include "../components/Expirable.hpp"
#include "../components/ShootingGalleryState.hpp"
#include "../../events/EventManager.hpp"
#include "../../events/EventListener.hpp"
#include "../../events/KeyboardEvent.hpp"
#include <unordered_set>
#include <string>
#include <memory>

namespace game::ecs::systems {
    
    /**
     * PlayerControlSystem enhanced for dual input support and pure component-based input.
     * 
     * C++ Implementation References:
     * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/player_control_system.py
     * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/PlayerControlSystem.java
     * 
     * System that handles player input for movement and shooting with dual input support:
     * 1. Primary: KeyboardInput component (pure component-based, frame-perfect)
     * 2. Fallback: EventListener input (existing event-driven approach)
     * 
     * This provides backward compatibility while enabling pure ECS architecture.
     */
    class PlayerControlSystem : public game::ecs::System, public game::events::EventListener {
    public:
        /**
         * Constructor
         * 
         * @param eventManager Event manager for handling input events (fallback)
         * @param worldWidth Width of the game world for boundary checking
         * @param worldHeight Height of the game world for positioning
         */
        PlayerControlSystem(game::events::EventManager& eventManager, 
                           float worldWidth, 
                           float worldHeight);
        
        /**
         * Destructor
         */
        ~PlayerControlSystem() override = default;
        
        /**
         * Handle keyboard events for tracking key states (fallback input method)
         * 
         * @param event The event to process (from EventListener interface)
         */
        void onEvent(const game::events::Event& event) override;
        
        /**
         * Update player control for all relevant entities
         * Uses dual input approach: KeyboardInput components first, then fallback to events
         * 
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime) override;
        
        /**
         * Entity management
         */
        void onEntityAdded(const Entity& entity) override;
        void onEntityRemoved(const Entity& entity) override;
        
    private:
        /**
         * Handle movement input for the player with dual input support
         * 
         * @param entity The entity to move
         * @param transform Transform component of the entity
         * @param input Input component with key mappings
         * @param keyboardInput Optional KeyboardInput component (preferred)
         * @param deltaTime Time elapsed since last update
         */
        void handleMovement(const Entity& entity, 
                           game::ecs::components::Transform* transform,
                           game::ecs::components::Input* input,
                           game::ecs::components::KeyboardInput* keyboardInput,
                           float deltaTime);
        
        /**
         * Handle shooting input for the player with dual input support
         * 
         * @param entity The player entity
         * @param transform Transform component of the player
         * @param player Player component with shooting mechanics
         * @param input Input component with key mappings
         * @param keyboardInput Optional KeyboardInput component (preferred)
         */
        void handleShooting(const Entity& entity,
                           game::ecs::components::Transform* transform,
                           game::ecs::components::Player* player,
                           game::ecs::components::Input* input,
                           game::ecs::components::KeyboardInput* keyboardInput);
        
        /**
         * Check if a key is pressed using dual input approach
         * 
         * @param key The key to check
         * @param keyboardInput Optional KeyboardInput component (preferred)
         * @return true if the key is currently pressed
         */
        bool isKeyPressed(const std::string& key, 
                         game::ecs::components::KeyboardInput* keyboardInput) const;
        
        /**
         * Create a shoot request component for request-based projectile creation
         * NEW: Request-based shooting for Phase 3 pure ECS architecture
         * 
         * @param entity The player entity making the request
         * @param x X position to create the projectile
         * @param y Y position to create the projectile
         */
        void createShootRequest(const Entity& entity, float x, float y);
        
        /**
         * Create a new projectile at the specified position (preserved for compatibility)
         * 
         * @param x X position to create the projectile
         * @param y Y position to create the projectile
         */
        void createProjectile(float x, float y);
        
        // Event management
        game::events::EventManager& eventManager_;
        
        // World boundaries
        float worldWidth_;
        float worldHeight_;
        
        // Pressed keys tracking for fallback input (matches Python/Java pattern)
        std::unordered_set<std::string> pressedKeys_;
    };
    
} // namespace game::ecs::systems 