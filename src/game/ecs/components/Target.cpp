#include "Target.hpp"
#include <sstream>

namespace game::ecs::components {
    
    Target::Target(const game::ecs::Entity& entity, 
                   int pointValue, 
                   const std::string& targetType, 
                   bool isHit)
        : game::ecs::Component(entity)
        , pointValue(pointValue)
        , targetType(targetType)
        , isHit(isHit)
    {
    }
    
    void Target::markAsHit() {
        isHit = true;
    }
    
    int Target::getPointValue() const {
        return pointValue;
    }
    
    const std::string& Target::getTargetType() const {
        return targetType;
    }
    
    bool Target::isHitTarget() const {
        return isHit;
    }
    
    std::string Target::toString() const {
        std::string hitStatus = isHit ? "HIT" : "ACTIVE";
        std::ostringstream oss;
        oss << "Target(entity=" << entity_.getId()
            << ", points=" << pointValue
            << ", type=" << targetType
            << ", status=" << hitStatus << ")";
        return oss.str();
    }
    
} // namespace game::ecs::components 