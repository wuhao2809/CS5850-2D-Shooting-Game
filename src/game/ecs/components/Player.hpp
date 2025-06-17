#pragma once

#include "../Component.hpp"
#include "../Entity.hpp"

// Forward declaration
class Timer;

namespace game::ecs::components {
    
    /**
     * Player Component - Duck Shooter Game
     * 
     * Manages player-specific shooting mechanics including fire rate and timing.
     * Simplified to match Python/Java implementations.
     * 
     * Based on: Python/Java Player components
     */
    class Player : public game::ecs::Component {
    public:
        /**
         * Creates a Player component with Timer dependency.
         * @param entity The entity this component belongs to
         * @param timer Timer instance for hardware-independent timing (required)
         * @param fireRate Time between shots in seconds
         * @throws std::invalid_argument if timer is null
         */
        Player(const game::ecs::Entity& entity, 
               const Timer* timer,
               float fireRate = 0.3f);
        
        /**
         * Copy constructor and assignment operator
         */
        Player(const Player& other) = default;
        Player& operator=(const Player& other) = default;
        
        /**
         * Move constructor and assignment operator
         */
        Player(Player&& other) noexcept = default;
        Player& operator=(Player&& other) noexcept = default;
        
        /**
         * Destructor
         */
        ~Player() override = default;
        
        // Public member variables for direct access (ECS pattern)
        float fireRate;      ///< Time between shots in seconds
        double lastFired;    ///< Timestamp of last shot in game time (seconds)
        
        /**
         * Check if the player can fire based on fire rate and current time
         * 
         * @return true if enough time has passed since last shot
         */
        bool canFire() const;
        
        /**
         * Record that the player has fired a shot (updates timestamp)
         */
        void fire();
        
        /**
         * Get the fire rate
         * 
         * @return Fire rate in seconds between shots
         */
        float getFireRate() const;
        
        /**
         * Get the timestamp of the last shot fired
         * 
         * @return Last fired timestamp in game time (seconds)
         */
        double getLastFired() const;
        
        /**
         * String representation for debugging
         * 
         * @return String describing player state
         */
        std::string toString() const;

    private:
        const Timer* timer_;  ///< Timer reference for hardware-independent timing
    };
    
} // namespace game::ecs::components 