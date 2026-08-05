#include <benchmark/benchmark.h>
#include <array>
#include <vector>
#include <random>
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"
#include "../BenchUtils.hpp"

struct HeavyComponent { std::array<int, 16> data; };

static void BM_RegistryCreateEntityFresh(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming(); 
        nut::Registry registry;
        state.ResumeTiming();

        for (int i = 0; i < state.range(0); ++i)
            benchmark::DoNotOptimize(registry.createEntity());
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_RegistryCreateEntityRecycled(benchmark::State& state) 
{
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::EntityId> shuffledIds = nut::bench::generateShuffledIds(numEntities); // for cache miss

    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities = nut::bench::setupEntities(registry, state);

        for (nut::EntityId id : shuffledIds)
            registry.destroyEntity(id);

        state.ResumeTiming();

        // take ids in the freeIds list
        for (int i = 0; i < state.range(0); ++i)
            benchmark::DoNotOptimize(registry.createEntity());
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_RegistryDestroyEntity(benchmark::State& state) 
{   
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::EntityId> shuffledIds = nut::bench::generateShuffledIds(numEntities); // for cache miss

    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities = nut::bench::setupEntities(registry, state);

        state.ResumeTiming();

        for (nut::EntityId id : shuffledIds)
            registry.destroyEntity(id);
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_RegistryAddComponentInsert(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities = nut::bench::setupEntities(registry, state);
        std::vector<nut::EntityId> entityIds = nut::bench::getIdsFromEntities(entities);

        registry.reserve<HeavyComponent>(state.range(0)); 
        HeavyComponent compToCopy{std::array<int, 16>{0}}; 

        state.ResumeTiming();

        for (int i = 0; i < state.range(0); ++i) 
            registry.addComponent<HeavyComponent>(entityIds[i], compToCopy); // insert version
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_RegistryAddComponentEmplace(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities = nut::bench::setupEntities(registry, state);
        std::vector<nut::EntityId> entityIds = nut::bench::getIdsFromEntities(entities);
        
        registry.reserve<HeavyComponent>(state.range(0));

        state.ResumeTiming();

        for (int i = 0; i < state.range(0); ++i) 
            registry.addComponent<HeavyComponent>(entityIds[i], std::array<int, 16>{0}); // emplace version
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_RegistryGetComponent(benchmark::State& state) 
{
    nut::Registry registry;
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::EntityId> shuffledIds = nut::bench::generateShuffledIds(numEntities); // shuffle the ids for cache miss

    for (size_t i = 0; i < numEntities; ++i) 
    {
        nut::Entity e = registry.createEntity();
        registry.addComponent<HeavyComponent>(e.getId(), std::array<int, 16>{0});
    }

    for (auto _ : state)
        for (nut::EntityId id : shuffledIds) 
            benchmark::DoNotOptimize(registry.getComponent<HeavyComponent>(id));

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_RegistryHasComponent(benchmark::State& state) 
{
    nut::Registry registry;
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));

    // fill one out of two
    for (size_t i = 0; i < numEntities; ++i) 
    {
        nut::Entity e = registry.createEntity();
        if (i % 2 == 0) 
            registry.addComponent<HeavyComponent>(e.getId(), std::array<int, 16>{0});
    }

    // shuffle ids for cache miss
    std::vector<nut::EntityId> shuffledIds = nut::bench::generateShuffledIds(numEntities);

    for (auto _ : state)
        for (nut::EntityId id : shuffledIds)
            benchmark::DoNotOptimize(registry.hasComponent<HeavyComponent>(id));

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_RegistryRemoveComponent(benchmark::State& state) {
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities;
        entities.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) 
        {
            entities.push_back(registry.createEntity());
            registry.addComponent<HeavyComponent>(entities[i].getId(), std::array<int, 16>{0});
        }
        std::vector<nut::EntityId> entityIds = nut::bench::getIdsFromEntities(entities);

        state.ResumeTiming();

        for (int i = 0; i < state.range(0); ++i)
            registry.removeComponent<HeavyComponent>(entityIds[i]);
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_RegistryCreateEntityFresh)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_RegistryDestroyEntity)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_RegistryCreateEntityRecycled)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_RegistryAddComponentInsert)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_RegistryAddComponentEmplace)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_RegistryGetComponent)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_RegistryHasComponent)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_RegistryRemoveComponent)->Arg(100000)->Arg(1000000);