#include "Expirable.hpp"
#include <sstream>

namespace game::ecs::components {
    
    Expirable::Expirable(const game::ecs::Entity& entity)
        : game::ecs::Component(entity)
        , isExpiredFlag(false)
    {
    }
    
    void Expirable::markExpired() {
        isExpiredFlag = true;
    }
    
    bool Expirable::isExpired() const {
        return isExpiredFlag;
    }
    
    std::string Expirable::toString() const {
        std::ostringstream oss;
        oss << "Expirable(entity=" << entity_.getId()
            << ", expired=" << (isExpiredFlag ? "true" : "false") << ")";
        return oss.str();
    }
    
} // namespace game::ecs::components 