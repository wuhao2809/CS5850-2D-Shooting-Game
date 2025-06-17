#include "Projectile.hpp"
#include <sstream>

namespace game::ecs::components {
    
    Projectile::Projectile(const game::ecs::Entity& entity, 
                           float speed, 
                           float maxRange, 
                           float traveledDistance)
        : game::ecs::Component(entity)
        , speed(speed)
        , maxRange(maxRange)
        , traveledDistance(traveledDistance)
    {
    }
    
    void Projectile::addTraveledDistance(float distance) {
        traveledDistance += distance;
    }
    
    bool Projectile::shouldExpire() const {
        return traveledDistance >= maxRange;
    }
    
    float Projectile::getSpeed() const {
        return speed;
    }
    
    float Projectile::getMaxRange() const {
        return maxRange;
    }
    
    float Projectile::getTraveledDistance() const {
        return traveledDistance;
    }
    
    std::string Projectile::toString() const {
        std::ostringstream oss;
        oss << "Projectile(entity=" << entity_.getId()
            << ", speed=" << speed
            << ", range=" << maxRange
            << ", traveled=" << traveledDistance << ")";
        return oss.str();
    }
    
} // namespace game::ecs::components 