#include <benchmark/benchmark.h>
#include "ECS/internal/SparseSet.hpp"
#include "ECS/EcsTypes.hpp"
#include "../BenchUtils.hpp"
#include <array>
#include <random>

struct HeavyComponent { std::array<int, 16> data; };

static void BM_SparseSetInsert(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::internal::SparseSet<HeavyComponent> sparseSet;
        sparseSet.reserve(state.range(0));
        state.ResumeTiming();

        for (nut::EntityId i = 0; i < state.range(0); ++i)
            sparseSet.insert(i, HeavyComponent{{0, 1, 2, 3}});
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_SparseSetEmplace(benchmark::State& state) 
{
    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::internal::SparseSet<HeavyComponent> sparseSet;
        sparseSet.reserve(state.range(0));
        state.ResumeTiming();

        for (nut::EntityId i = 0; i < state.range(0); ++i)
            sparseSet.emplace(i, std::array<int, 16>{0, 1, 2, 3});
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_SparseSetGet(benchmark::State& state) 
{
    nut::internal::SparseSet<HeavyComponent> sparseSet;
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    sparseSet.reserve(numEntities);

    for (nut::EntityId i = 0; i < numEntities; ++i)
        sparseSet.emplace(i, std::array<int, 16>{0, 1, 2, 3});

    // shuffledIds for cache miss
    std::vector<nut::EntityId> shuffledIds = nut::bench::generateShuffledIds(numEntities);

    for (auto _ : state) 
        for (nut::EntityId id : shuffledIds)
            benchmark::DoNotOptimize(sparseSet.get(id));

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_SparseSetContains(benchmark::State& state) 
{
    nut::internal::SparseSet<int> sparseSet;
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    sparseSet.reserve(numEntities);

    for (nut::EntityId i = 0; i < numEntities; ++i)
        sparseSet.insert(i, 42);

    // for cache miss
    std::vector<nut::EntityId> shuffledIds = nut::bench::generateShuffledIds(numEntities);

    for (auto _ : state) 
        for (nut::EntityId id : shuffledIds)
            benchmark::DoNotOptimize(sparseSet.contains(id));

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_SparseSetErase(benchmark::State& state) 
{
    nut::EntityId numEntities = static_cast<nut::EntityId>(state.range(0));
    std::vector<nut::EntityId> shuffledIds = nut::bench::generateShuffledIds(numEntities); // for cache miss

    for (auto _ : state) 
    {
        state.PauseTiming();
        nut::internal::SparseSet<HeavyComponent> sparseSet;
        sparseSet.reserve(state.range(0));
        for (nut::EntityId i = 0; i < state.range(0); ++i)
            sparseSet.emplace(i, std::array<int, 16>{0, 1, 2, 3});
        
        state.ResumeTiming();

        for (nut::EntityId id : shuffledIds)
            sparseSet.erase(id);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_SparseSetInsert)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_SparseSetEmplace)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_SparseSetGet)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_SparseSetContains)->Arg(100000)->Arg(1000000);
BENCHMARK(BM_SparseSetErase)->Arg(100000)->Arg(1000000);