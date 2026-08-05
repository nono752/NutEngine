#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"

namespace nut
{
    ComponentId Registry::getNextComponentId() 
    {
        static EntityId nextId = 0;
        return nextId++;
    }

    Entity Registry::createEntity()
    {
        EntityId newId; 

        if (!freeIds.empty()) 
        {
            newId = freeIds.back(); // utilise vecteur plutot que queue pour question d'optimisation.
            freeIds.pop_back();
            entityVersions[newId]++; // incrémente de sorte à ce que une version vivante est paire
        } 
        else 
        {
            newId = idCounter++;
            entityVersions.push_back(0);
        }

        return Entity(newId, entityVersions[newId], this);
    }

    void Registry::destroyEntity(Entity entity)
    {
        if (!isValid(entity)) 
            return; 
        
        destroyEntity(entity.getId());
    }
    void Registry::destroyEntity(EntityId id)
    {
        if (!isValid(id))
            return;

        for (auto& sset : components)
            if (sset)
                sset->erase(id);

        // increment version to odd number is equivalent to set dead
        entityVersions[id]++;

        freeIds.push_back(id);
    }

    // the entity correspond to the current entity in the registry at the same id (if they still have the same version)
    bool Registry::isValid(Entity entity) const
    {
        // entityVersions.size() give the biggest distributed id
        // the registry keep the actual version of an entity
        return entity.getId() < entityVersions.size() && entity.getVersion() == entityVersions[entity.getId()];
    }

    // the id is associated with an alive entity independently of the version
    bool Registry::isValid(EntityId id) const 
    {
        if (id >= entityVersions.size()) 
            return false;

        // entityVersions[id] % 2 == 0  =>  the id handle an active entity
        return (entityVersions[id] & 1) == 0; 
    }

    void Registry::clearAll()
    {
        for (auto& sset : components)
            if (sset)
                sset->clear();
        
        freeIds.clear();
    
        for (EntityId i = 0; i < entityVersions.size(); ++i) 
        {
            if ((entityVersions[i] & 1) == 0) // if version even
                entityVersions[i]++; // make odd i.e. dead

            freeIds.push_back(i);
        }
    }

    void Registry::reserveAll(size_t capacity)
    {
        for (auto& pool : components)
            pool->reserve(capacity); // reserve is virtual here
    }
}