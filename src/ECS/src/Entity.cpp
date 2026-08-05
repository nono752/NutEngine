#include "ECS/Entity.hpp"
#include "ECS/Registry.hpp"

namespace nut
{
    Entity::Entity(EntityId id, EntityVersion version, Registry* registry)
    : id(id), version(version), owner(registry)
    {}

    void Entity::destroy()
    {
        if (owner)
            owner->destroyEntity(*this);
    }

    bool Entity::isValid() const
    {
        if (!owner)
            return false;

        return owner->getVersion(id) == version;
    }

    Entity::operator bool() const
    {
        return isValid();
    }

    bool Entity::operator==(const Entity& other) const
    {
        return other.id == id && other.version == version && other.owner == owner;
    }
            
    bool Entity::operator!=(const Entity& other) const
    {
        return !(*this == other);
    }
}