#include "ECS/Entity.hpp"

// Included at the end of Registry.hpp to solve a circular dependencie.

namespace nut
{
    template<typename T> decltype(auto) 
    Entity::addComponent(const T& component)
    {
        assert(owner != nullptr && owner->isValid(id));
        return owner->addComponent<T>(id, component);
    }
    template<typename T, typename... Args> decltype(auto) 
    Entity::addComponent(Args&&... args)
    {
        assert(owner != nullptr && owner->isValid(id));
        return owner->addComponent<T>(id, std::forward<Args>(args)...);
    }
    template<typename T> T& 
    Entity::getComponent()
    {
        assert(owner != nullptr && owner->isValid(id));
        return owner->getComponent<T>(id);
    }
    template<typename T> bool 
    Entity::hasComponent()
    {
        assert(owner != nullptr && owner->isValid(id));
        return owner->hasComponent<T>(id);
    }
    template<typename T> void 
    Entity::removeComponent()
    {
        assert(owner != nullptr && owner->isValid(id));
        owner->removeComponent<T>(id);
    }
}