#include "KeyboardEvent.hpp"

namespace game {
namespace events {

KeyboardEvent::KeyboardEvent(const std::string& key, const std::string& keyText, bool isPressed)
    : Event("keyboard")
    , key_(key)
    , keyText_(keyText)
    , isPressed_(isPressed) {
}

} // namespace events
} // namespace game 