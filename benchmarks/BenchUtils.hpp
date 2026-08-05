#ifndef BENCH_UTILS_H
#define BENCH_UTILS_H

#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <cstddef>
#include "ECS/EcsTypes.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"

namespace nut::bench
{
    inline std::vector<EntityId> generateShuffledIds(std::size_t count, unsigned int seed = 42) 
    {
        std::vector<EntityId> ids(count);
        std::iota(ids.begin(), ids.end(), 0);

        std::mt19937 rng(seed);
        std::shuffle(ids.begin(), ids.end(), rng);

        return ids;
    }

    template <typename T>
    inline void shuffleVector(std::vector<T>& vec, unsigned int seed = 42) 
    {
        std::mt19937 rng(seed);
        std::shuffle(vec.begin(), vec.end(), rng);
    }

    inline std::vector<EntityId> generateRandomIdsInRange(std::size_t count, EntityId min_id, EntityId max_id, unsigned int seed = 42) 
    {
        std::vector<EntityId> ids(count);
        std::mt19937 rng(seed);
        std::uniform_int_distribution<EntityId> dist(min_id, max_id);

        for (EntityId& id : ids) 
            id = dist(rng);

        return ids;
    }

    inline std::vector<Entity> setupEntities(Registry& registry, benchmark::State& state)
    {
        std::vector<Entity> entities;
        entities.reserve(state.range(0));

        for (int i = 0; i < state.range(0); ++i)
            entities.push_back(registry.createEntity());

        return entities;
    }

    inline std::vector<EntityId> setupEntityIds(Registry& registry, benchmark::State& state)
    {
        std::vector<EntityId> entityIds;
        entityIds.reserve(state.range(0));

        for (int i = 0; i < state.range(0); ++i)
            entityIds.push_back(registry.createEntity().getId());

        return entityIds;
    }
    
    inline std::vector<EntityId> getIdsFromEntities(std::vector<Entity> entities)
    {
        std::vector<EntityId> ids;
        ids.reserve(entities.size());

        for (Entity& e : entities)
            ids.push_back(e.getId());
        
        return ids;
    }
}


#endif