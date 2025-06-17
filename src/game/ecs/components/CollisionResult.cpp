/**
 * CollisionResult component for pure component-based collision tracking.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/components/collision_result.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/CollisionResult.java
 * 
 * Implementation of CollisionResult component methods following the exact
 * same patterns established in the Python and Java reference implementations.
 */

#include "CollisionResult.hpp"
#include <sstream>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace components {

// CollisionData implementation
CollisionResult::CollisionData::CollisionData(Entity entityA, Entity entityB, const Vector2& collisionPoint, const Vector2& collisionNormal)
    : entityA(entityA), entityB(entityB), collisionPoint(collisionPoint), collisionNormal(collisionNormal),
      timestamp(std::chrono::steady_clock::now()) {
    // otherEntity and owner will be set by CollisionResult::addCollision()
}

std::string CollisionResult::CollisionData::toString() const {
    std::ostringstream oss;
    oss << "CollisionData(entity_a=" << entityA.getId() 
        << ", entity_b=" << entityB.getId()
        << ", point=(" << collisionPoint.x << "," << collisionPoint.y << ")"
        << ", normal=(" << collisionNormal.x << "," << collisionNormal.y << "))";
    return oss.str();
}

// CollisionResult implementation
CollisionResult::CollisionResult(Entity entity)
    : Component(entity), processed(false), frameCount(0), enabled(true) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                "[CollisionResult] Created component for entity %llu", entity.getId());
}

void CollisionResult::addCollision(Entity entityA, Entity entityB, const Vector2& collisionPoint, const Vector2& collisionNormal) {
    CollisionData collisionData(entityA, entityB, collisionPoint, collisionNormal);
    
    // Set the other entity reference for convenience (matches Python/Java logic)
    collisionData.otherEntity = (entityB == getEntity()) ? entityA : entityB;
    collisionData.owner = getEntity();
    
    collisions.push_back(std::move(collisionData));
    processed = false;
    
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                 "[CollisionResult] Added collision for entity %llu: %s", 
                 getEntity().getId(), collisionData.toString().c_str());
}

const std::vector<CollisionResult::CollisionData>& CollisionResult::getCollisions() const {
    return collisions;
}

std::vector<CollisionResult::CollisionData> CollisionResult::getCollisionsCopy() const {
    return collisions;  // Returns a copy
}

bool CollisionResult::hasCollisions() const {
    return !collisions.empty() && !processed;
}

void CollisionResult::markProcessed() {
    processed = true;
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                 "[CollisionResult] Marked %zu collisions as processed for entity %llu", 
                 collisions.size(), getEntity().getId());
}

void CollisionResult::clearCollisions() {
    size_t numCollisions = collisions.size();
    collisions.clear();
    processed = false;
    frameCount++;
    
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                 "[CollisionResult] Cleared %zu collisions for entity %llu, frame %d", 
                 numCollisions, getEntity().getId(), frameCount);
}

const CollisionResult::CollisionData* CollisionResult::getCollisionWith(Entity otherEntity) const {
    for (const auto& collision : collisions) {
        if (collision.entityA == otherEntity || collision.entityB == otherEntity) {
            return &collision;
        }
    }
    return nullptr;
}

std::string CollisionResult::toString() const {
    std::ostringstream oss;
    oss << "CollisionResult(entity=" << getEntity().getId()
        << ", collisions=" << collisions.size()
        << ", processed=" << (processed ? "true" : "false")
        << ", frame=" << frameCount << ")";
    return oss.str();
}

} // namespace components
} // namespace ecs
} // namespace game 