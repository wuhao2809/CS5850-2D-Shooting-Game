#include "Player.hpp"
#include "../../Timer.hpp"  // Add Timer include
#include <sstream>
#include <stdexcept>
#include <SDL3/SDL.h>

namespace game::ecs::components {
    
    Player::Player(const game::ecs::Entity& entity, 
                   const Timer* timer,
                   float fireRate)
        : game::ecs::Component(entity)
        , fireRate(fireRate)
        , lastFired(0.0)
        , timer_(timer)
    {
        if (timer_ == nullptr) {
            throw std::invalid_argument("Player component requires a Timer instance for hardware-independent timing");
        }
        
        // Initialize with current game time to prevent immediate firing
        lastFired = timer_->getClock();
        
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "Player initialized with fireRate=%.3f using hardware-independent timing", 
                   fireRate);
    }
    
    bool Player::canFire() const {
        double currentTime = timer_->getClock();  // ✅ Game time
        return (currentTime - lastFired) >= fireRate;
    }
    
    void Player::fire() {
        lastFired = timer_->getClock();  // ✅ Game time
    }
    
    float Player::getFireRate() const {
        return fireRate;
    }
    
    double Player::getLastFired() const {
        return lastFired;
    }
    
    std::string Player::toString() const {
        std::ostringstream oss;
        oss << "Player(entity=" << entity_.getId() 
            << ", fireRate=" << fireRate 
            << ", lastFired=" << lastFired << ")";
        return oss.str();
    }
    
} // namespace game::ecs::components 