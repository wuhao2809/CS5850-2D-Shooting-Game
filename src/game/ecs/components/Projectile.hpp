#pragma once

#include "../Component.hpp"
#include "../Entity.hpp"
#include <string>

namespace game::ecs::components {
    
    /**
     * Projectile Component - Duck Shooter Game
     * 
     * Component that marks an entity as a projectile.
     * Tracks projectile behavior including speed, range limits, and distance traveled.
     * 
     * Based on: Python/Java Projectile components
     */
    class Projectile : public game::ecs::Component {
    public:
        /**
         * Constructor with duck shooter game defaults
         * 
         * @param entity The entity this component belongs to
         * @param speed Speed of the projectile in pixels per second (default: 400.0)
         * @param maxRange Maximum distance the projectile can travel before being destroyed (default: 800.0)
         * @param traveledDistance Distance already traveled (default: 0.0)
         */
        Projectile(const game::ecs::Entity& entity, 
                   float speed = 400.0f, 
                   float maxRange = 800.0f, 
                   float traveledDistance = 0.0f);
        
        /**
         * Copy constructor and assignment operator
         */
        Projectile(const Projectile& other) = default;
        Projectile& operator=(const Projectile& other) = default;
        
        /**
         * Move constructor and assignment operator
         */
        Projectile(Projectile&& other) noexcept = default;
        Projectile& operator=(Projectile&& other) noexcept = default;
        
        /**
         * Destructor
         */
        ~Projectile() override = default;
        
        // Public member variables for direct access (ECS pattern)
        float speed;            ///< Speed of the projectile in pixels per second
        float maxRange;         ///< Maximum distance the projectile can travel before being destroyed
        float traveledDistance; ///< Distance already traveled in pixels
        
        /**
         * Add to the traveled distance
         * 
         * @param distance Distance to add to the traveled distance
         */
        void addTraveledDistance(float distance);
        
        /**
         * Check if the projectile should expire based on its range
         * 
         * @return true if the projectile should be destroyed, false otherwise
         */
        bool shouldExpire() const;
        
        /**
         * Get the speed of the projectile
         * 
         * @return The speed in pixels per second
         */
        float getSpeed() const;
        
        /**
         * Get the maximum range of the projectile
         * 
         * @return The maximum range in pixels
         */
        float getMaxRange() const;
        
        /**
         * Get the distance traveled by the projectile
         * 
         * @return The traveled distance in pixels
         */
        float getTraveledDistance() const;
        
        /**
         * String representation for debugging
         * 
         * @return String describing projectile state
         */
        std::string toString() const;
    };
    
} // namespace game::ecs::components 