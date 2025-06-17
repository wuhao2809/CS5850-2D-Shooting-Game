#pragma once

#include "../Component.hpp"
#include "../Entity.hpp"
#include <string>

namespace game::ecs::components {
    
    /**
     * Target Component - Duck Shooter Game
     * 
     * Component that marks an entity as a target in the duck shooter game.
     * Contains scoring information and target type classification.
     * 
     * Based on: Python/Java Target components
     */
    class Target : public game::ecs::Component {
    public:
        /**
         * Constructor with duck shooter game defaults
         * 
         * @param entity The entity this component belongs to
         * @param pointValue Points awarded when this target is hit (from JSON config)
         * @param targetType Type of target ("regular", "boss")
         * @param isHit Whether this target has been hit (default: false)
         */
        Target(const game::ecs::Entity& entity, 
               int pointValue, 
               const std::string& targetType = "regular", 
               bool isHit = false);
        
        /**
         * Copy constructor and assignment operator
         */
        Target(const Target& other) = default;
        Target& operator=(const Target& other) = default;
        
        /**
         * Move constructor and assignment operator
         */
        Target(Target&& other) noexcept = default;
        Target& operator=(Target&& other) noexcept = default;
        
        /**
         * Destructor
         */
        ~Target() override = default;
        
        // Public member variables for direct access (ECS pattern)
        int pointValue;          ///< Points awarded for hitting this target
        std::string targetType;  ///< Type of target ("regular", "boss")
        bool isHit;              ///< Whether this target has been hit
        
        /**
         * Mark this target as hit (prevents duplicate scoring)
         */
        void markAsHit();
        
        /**
         * Get the point value of this target
         * 
         * @return The points awarded when hit
         */
        int getPointValue() const;
        
        /**
         * Get the type of this target
         * 
         * @return The target type
         */
        const std::string& getTargetType() const;
        
        /**
         * Check if this target has been hit
         * 
         * @return true if target has been hit
         */
        bool isHitTarget() const;
        
        /**
         * String representation for debugging
         * 
         * @return String describing target state
         */
        std::string toString() const;
        
        // Note: Point values are now loaded from GameData.json configuration
        // Regular ducks: typically 10 points (configurable)
        // Boss ducks: typically 50 points (configurable)
    };
    
} // namespace game::ecs::components 