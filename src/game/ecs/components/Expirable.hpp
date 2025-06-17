#pragma once

#include "../Component.hpp"
#include "../Entity.hpp"
#include <string>

namespace game::ecs::components {
    
    /**
     * Expirable Component - Duck Shooter Game
     * 
     * Component that marks entities as expirable and tracks their expiration state.
     * Used by various systems to mark entities for removal, and by ExpiredEntitiesSystem to clean them up.
     * 
     * Based on: Python/Java Expirable components
     */
    class Expirable : public game::ecs::Component {
    public:
        /**
         * Constructor
         * 
         * @param entity The entity this component belongs to
         */
        Expirable(const game::ecs::Entity& entity);
        
        /**
         * Copy constructor and assignment operator
         */
        Expirable(const Expirable& other) = default;
        Expirable& operator=(const Expirable& other) = default;
        
        /**
         * Move constructor and assignment operator
         */
        Expirable(Expirable&& other) noexcept = default;
        Expirable& operator=(Expirable&& other) noexcept = default;
        
        /**
         * Destructor
         */
        ~Expirable() override = default;
        
        // Public member variables for direct access (ECS pattern)
        bool isExpiredFlag;   ///< Whether this entity is expired
        
        /**
         * Mark this entity as expired for removal
         */
        void markExpired();
        
        /**
         * Check if this entity is expired
         * 
         * @return true if the entity is expired, false otherwise
         */
        bool isExpired() const;
        
        /**
         * String representation for debugging
         * 
         * @return String describing expirable state
         */
        std::string toString() const;
    };
    
} // namespace game::ecs::components 