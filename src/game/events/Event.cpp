#include "Event.hpp"

namespace game {
namespace events {

Event::Event(const std::string& type)
    : type_(type)
    , timestamp_(std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch()).count()) {
}

} // namespace events
} // namespace game 