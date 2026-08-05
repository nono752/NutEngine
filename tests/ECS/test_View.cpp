#include <gtest/gtest.h>
#include <array>
#include <string>
#include "ECS/Registry.hpp"
#include "ECS/View.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EcsTypes.hpp"

struct Health { int hp; };
struct Damage { int amount; };

class ViewTest : public ::testing::Test 
{
    protected:
        nut::Registry registry;
        int iterationCount = 0;

        void testIteration(std::array<nut::EntityId, 2> ids, const std::string& where)
        {
            EXPECT_EQ(iterationCount, 2) << "Failed in: " << where;;
            EXPECT_EQ(registry.getComponent<Health>(ids[0]).hp, 180) << "Failed in: " << where;;
            EXPECT_EQ(registry.getComponent<Health>(ids[1]).hp, 90) << "Failed in: " << where;;
        }
        void setIteration(std::array<nut::EntityId, 2> ids)
        {
            iterationCount = 0;
            registry.getComponent<Health>(ids[0]).hp = 200;
            registry.getComponent<Health>(ids[1]).hp = 100;
        }
};

TEST_F(ViewTest, ViewIteratesOnlyMatchingEntities) 
{
    nut::Entity e0 = registry.createEntity();
    nut::EntityId id0 = e0.getId();
    registry.addComponent<Health>(id0, Health{200}); 
    registry.addComponent<Damage>(id0, Damage{20});

    nut::Entity e1 = registry.createEntity();
    nut::EntityId id1 = e1.getId();
    registry.addComponent<Health>(id1, Health{100}); 
    registry.addComponent<Damage>(id1, Damage{10});

    nut::Entity e2 = registry.createEntity();
    nut::EntityId id2 = e2.getId();
    registry.addComponent<Health>(id2, Health{50});

    nut::Entity e3 = registry.createEntity();
    nut::EntityId id3 = e3.getId();
    registry.addComponent<Damage>(id3, Damage{20});

    auto view = registry.view<Health, Damage>();
    EXPECT_TRUE(view.contains(id0));
    EXPECT_TRUE(view.contains(id1));
    EXPECT_FALSE(view.contains(id2));
    EXPECT_FALSE(view.contains(id3));

    // check IdIterator
    setIteration({id0, id1});
    for(nut::EntityId entityId : view) 
    { 
        iterationCount++;
        registry.getComponent<Health>(entityId).hp -= registry.getComponent<Damage>(entityId).amount;
    }
    testIteration({id0, id1}, "IdIterator");

    // check EntityIterator
    setIteration({id0, id1});
    for(nut::Entity entity : view.entities()) 
    { 
        iterationCount++;
        entity.getComponent<Health>().hp -= entity.getComponent<Damage>().amount;
    }
    testIteration({id0, id1}, "EntityIterator");

    // check ComponentIterator
    setIteration({id0, id1});
    for(auto [id, health, damage] : view.components()) 
    { 
        iterationCount++;
        health.hp -= damage.amount;
    }
    testIteration({id0, id1}, "ComponentIterator");

    // check each with ids
    setIteration({id0, id1});
    view.each([&](nut::EntityId id, Health& h, Damage& d) {
        iterationCount++;
        h.hp -= d.amount;
    });
    testIteration({id0, id1}, "iterating function each() with ids");

    // check each without ids
    setIteration({id0, id1});
    view.each([&](Health& h, Damage& d) {
        iterationCount++;
        h.hp -= d.amount;
    });
    testIteration({id0, id1}, "iterating function each() without ids");
}