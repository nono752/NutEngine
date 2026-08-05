#include <benchmark/benchmark.h>
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"
#include "ECS/View.hpp"
#include "../BenchUtils.hpp"

struct Position { float x, y; };
struct Velocity { float dx, dy; };

void setupSingleComponentAligned(nut::Registry& registry, benchmark::State& state)
{
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::EntityId> entityIds = nut::bench::setupEntityIds(registry, state);

    registry.reserve<Position>(numEntities);
    for (nut::EntityId id : entityIds)
        registry.addComponent<Position>(id, {0.0f, 0.0f});

    // Position datas aligned with ids
}
void setupSingleComponentFragmented(nut::Registry& registry, benchmark::State& state)
{
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::EntityId> entityIds = nut::bench::setupEntityIds(registry, state);

    registry.reserve<Position>(numEntities);

    // position added not sequentialy
    nut::bench::shuffleVector(entityIds);
    for (nut::EntityId id : entityIds)
        registry.addComponent<Position>(id, {0.0f, 0.0f});
    
    // Position datas are not aligned with ids
}

void setupMultiComponentAligned(nut::Registry& registry, benchmark::State& state)
{
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::EntityId> entityIds = nut::bench::setupEntityIds(registry, state);

    registry.reserve<Position>(numEntities);
    for (nut::EntityId id : entityIds)
        registry.addComponent<Position>(id, {0.0f, 0.0f});
    
    registry.reserve<Velocity>(numEntities);
    for (nut::EntityId i = 0; i < numEntities; ++i) 
        if (i % 2 == 0) // sparsity
            registry.addComponent<Velocity>(entityIds[i], {1.0f, 1.0f});
    
    // Position/Velocity aligned by ids
}
void setupMultiComponentFragmented(nut::Registry& registry, benchmark::State& state)
{
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));

    std::vector<nut::EntityId> entityIds;
    entityIds.reserve(numEntities);
    for (nut::EntityId i = 0; i < numEntities; ++i) 
        entityIds.push_back(registry.createEntity().getId());

    registry.reserve<Position>(numEntities);

    // Position datas added not sequentialy
    nut::bench::shuffleVector(entityIds);
    for (nut::EntityId id : entityIds)
        registry.addComponent<Position>(id, {0.0f, 0.0f});


    registry.reserve<Velocity>(numEntities);

    // Velocity datas added with sparsity and not sequentialy
    nut::bench::shuffleVector(entityIds);
    for (nut::EntityId i = 0; i < numEntities; ++i) 
        if (i % 2 == 0) // sparsity
            registry.addComponent<Velocity>(entityIds[i], {1.0f, 1.0f});

    // the Velocity/Position datas are not aligned -> cache miss
}

template <void (*SetupFunc)(nut::Registry&, benchmark::State&)>
static void BM_ViewSingleComponentWithIdIterator(benchmark::State& state) 
{
    nut::Registry registry;
    SetupFunc(registry, state);

    for (auto _ : state) 
    {
        auto view = registry.view<Position>();
        for (nut::EntityId id : view) 
        {
            Position& pos = registry.getComponent<Position>(id);
            pos.x += 1.0f;
            benchmark::DoNotOptimize(pos);
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

template <void (*SetupFunc)(nut::Registry&, benchmark::State&)>
static void BM_ViewSingleComponentWithEach(benchmark::State& state) 
{
    nut::Registry registry;
    SetupFunc(registry, state);

    for (auto _ : state) 
    {
        auto view = registry.view<Position>();
        
        view.each([](nut::EntityId id, Position& pos) {
            pos.x += 1.0f;
            benchmark::DoNotOptimize(pos);
        });
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

template <void (*SetupFunc)(nut::Registry&, benchmark::State&)>
static void BM_ViewMultiComponentWithIdIterator(benchmark::State& state) 
{
    nut::Registry registry;
    SetupFunc(registry, state);

    for (auto _ : state) 
    {
        auto view = registry.view<Position, Velocity>();
        for (nut::EntityId id : view)
        {
            auto& pos = registry.getComponent<Position>(id);
            auto& vel = registry.getComponent<Velocity>(id);
            pos.x += vel.dx;
            benchmark::DoNotOptimize(pos);
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

template <void (*SetupFunc)(nut::Registry&, benchmark::State&)>
static void BM_ViewMultiComponentWithComponentIterator(benchmark::State& state) 
{
    nut::Registry registry;
    SetupFunc(registry, state);

    for (auto _ : state) 
    {
        auto view = registry.view<Position, Velocity>();
        for (auto [id, pos, vel] : view.components()) 
        {
            pos.x += vel.dx;
            benchmark::DoNotOptimize(pos);
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

template <void (*SetupFunc)(nut::Registry&, benchmark::State&)>
static void BM_ViewMultiComponentWithEntityIterator(benchmark::State& state) 
{
    nut::Registry registry;
    SetupFunc(registry, state);

    for (auto _ : state) 
    {
        auto view = registry.view<Position, Velocity>();
        for (nut::Entity e : view.entities()) 
        {
            e.getComponent<Position>().x += e.getComponent<Velocity>().dx;
            benchmark::DoNotOptimize(e.getComponent<Position>());
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

template <void (*SetupFunc)(nut::Registry&, benchmark::State&)>
static void BM_ViewMultiComponentWithEach(benchmark::State& state) {
    nut::Registry registry;
    SetupFunc(registry, state);

    for (auto _ : state) 
    {
        auto view = registry.view<Position, Velocity>();
        
        view.each([](nut::EntityId id, Position& pos, Velocity& vel) {
            pos.x += vel.dx;
            benchmark::DoNotOptimize(pos);
        });
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK_TEMPLATE(BM_ViewSingleComponentWithIdIterator, setupSingleComponentAligned)->Arg(10000)->Arg(100000);
BENCHMARK_TEMPLATE(BM_ViewSingleComponentWithIdIterator, setupSingleComponentFragmented)->Arg(10000)->Arg(100000);

BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithIdIterator, setupMultiComponentAligned)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithIdIterator, setupMultiComponentFragmented)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithComponentIterator, setupMultiComponentAligned)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithComponentIterator, setupMultiComponentFragmented)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithEntityIterator, setupMultiComponentAligned)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithEntityIterator, setupMultiComponentFragmented)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithEach, setupMultiComponentAligned)->Arg(10000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_ViewMultiComponentWithEach, setupMultiComponentFragmented)->Arg(10000)->Arg(100000)->Arg(1000000);