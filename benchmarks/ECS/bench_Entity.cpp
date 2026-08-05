#include <benchmark/benchmark.h>
#include <vector>
#include <array>
#include <random>
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"
#include "../BenchUtils.hpp"

struct HeavyComponent { std::array<int, 16> data; };

static void BM_EntityDestroy(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities = nut::bench::setupEntities(registry, state);
        
        state.ResumeTiming();

        for (nut::Entity& entity : entities) 
            entity.destroy();
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_EntityAddComponentEmplace(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities = nut::bench::setupEntities(registry, state);

        state.ResumeTiming();

        for (nut::Entity& entity : entities) 
            entity.addComponent<HeavyComponent>(std::array<int, 16>{0});
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_EntityAddComponentInsert(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::Registry registry;
        std::vector<nut::Entity> entities = nut::bench::setupEntities(registry, state);

        HeavyComponent component{std::array<int, 16>{0}};

        state.ResumeTiming();

        // Utilisation du wrapper
        for (nut::Entity& entity : entities) 
            entity.addComponent<HeavyComponent>(component);
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_EntityGetComponent(benchmark::State& state) 
{
    nut::Registry registry;
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));

    std::vector<nut::Entity> entities;
    entities.reserve(numEntities);

    for (size_t i = 0; i < numEntities; ++i) 
    {
        nut::Entity e = registry.createEntity();
        e.addComponent<HeavyComponent>(std::array<int, 16>{0});
        entities.push_back(e);
    }
    nut::bench::shuffleVector<nut::Entity>(entities);

    for (auto _ : state)
        for (nut::Entity& e : entities)
            benchmark::DoNotOptimize(e.getComponent<HeavyComponent>());

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_EntityHasComponent(benchmark::State& state) 
{
    nut::Registry registry;
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::Entity> entities;
    entities.reserve(numEntities);

    for (size_t i = 0; i < numEntities; ++i) 
    {
        nut::Entity e = registry.createEntity();
        if (i % 2 == 0) 
            e.addComponent<HeavyComponent>(std::array<int, 16>{0});
        entities.push_back(e);
    }
    nut::bench::shuffleVector<nut::Entity>(entities);

    for (auto _ : state) 
        for (nut::Entity& e : entities) 
            benchmark::DoNotOptimize(e.hasComponent<HeavyComponent>());

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_EntityDestroy)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_EntityAddComponentEmplace)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_EntityAddComponentInsert)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_EntityGetComponent)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_EntityHasComponent)->Arg(100000)->Arg(1000000);