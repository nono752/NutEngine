#include <gtest/gtest.h>
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EcsTypes.hpp"

struct Position { float x, y; };
struct Velocity { float dx, dy; };

class RegistryTest : public ::testing::Test 
{
    protected:
        nut::Registry registry;
};

TEST_F(RegistryTest, CreateEntityYieldsValidEntity) 
{
    nut::Entity entity = registry.createEntity();

    EXPECT_TRUE(registry.isValid(entity));
    EXPECT_TRUE(registry.isValid(entity.getId()));
}

TEST_F(RegistryTest, DestroyEntityInvalidatesIt) 
{
    nut::Entity entity = registry.createEntity();
    EXPECT_TRUE(registry.isValid(entity));

    registry.destroyEntity(entity);
    
    EXPECT_FALSE(registry.isValid(entity));
}

TEST_F(RegistryTest, EntitiesAreUnique) 
{
    nut::Entity e1 = registry.createEntity();
    nut::Entity e2 = registry.createEntity();
    nut::Entity e3 = registry.createEntity();

    EXPECT_NE(e1, e2); // overloaded !=operator check id and version
    EXPECT_NE(e2, e3);
    EXPECT_NE(e1, e3);
}

TEST_F(RegistryTest, DestroyedEntitiesAreRecycledAndUnique) 
{
    nut::Entity e1 = registry.createEntity();
    nut::Entity e2 = registry.createEntity();
    nut::Entity e3 = registry.createEntity();

    registry.destroyEntity(e2);
    EXPECT_FALSE(registry.isValid(e2));

    nut::Entity e4 = registry.createEntity();

    // check uniqueness of a recycled entity
    EXPECT_TRUE(registry.isValid(e4));
    EXPECT_NE(e4, e1);
    EXPECT_NE(e4, e2);
    EXPECT_NE(e4, e3);

    // check if recycled and version updated correctly
    EXPECT_EQ(e4.getId(), e2.getId()) << "Id not recycled";
    EXPECT_NE(e4.getVersion(), e2.getVersion()) << "Version not updated";
    EXPECT_EQ(e4.getVersion(), e2.getVersion() + 2) << "Version not updated correctly (alive version should be even)";
}

TEST_F(RegistryTest, ClearAllDestroysEveryEntity) 
{
    nut::Entity e1 = registry.createEntity();
    nut::Entity e2 = registry.createEntity();

    registry.clearAll();

    EXPECT_FALSE(registry.isValid(e1));
    EXPECT_FALSE(registry.isValid(e2));
}

TEST_F(RegistryTest, ComponentTypesHaveUniqueIds) 
{
    nut::ComponentId posId = nut::Registry::getComponentId<Position>();
    nut::ComponentId velId = nut::Registry::getComponentId<Velocity>();

    // check uniqueness
    EXPECT_NE(posId, velId);

    // Check that the id is generated once only per Component.
    EXPECT_EQ(posId, nut::Registry::getComponentId<Position>());
}

TEST_F(RegistryTest, AddAndRemoveComponent) 
{
    nut::Entity entity = registry.createEntity();
    nut::EntityId entityId = entity.getId();

    registry.addComponent<Position>(entityId, Position{10.5f, 20.0f}); // with insert
    registry.addComponent<Velocity>(entityId, 1.0f, 2.0f); // with emplace
    ASSERT_TRUE(registry.hasComponent<Position>(entityId));
    ASSERT_TRUE(registry.hasComponent<Velocity>(entityId));

    // check if value correctly created
    Position& pos = registry.getComponent<Position>(entityId);
    Velocity& vel = registry.getComponent<Velocity>(entityId);
    EXPECT_FLOAT_EQ(pos.x, 10.5f);
    EXPECT_FLOAT_EQ(pos.y, 20.0f);
    EXPECT_FLOAT_EQ(vel.dx, 1.0f);
    EXPECT_FLOAT_EQ(vel.dy, 2.0f);

    // check remove
    registry.removeComponent<Position>(entityId);
    EXPECT_FALSE(registry.hasComponent<Position>(entityId));
}

TEST_F(RegistryTest, DestroyingEntityRemovesItsComponents) 
{
    nut::Entity entity = registry.createEntity();
    nut::EntityId entityId = entity.getId();

    registry.addComponent<Velocity>(entityId, Velocity{1.0f, 1.0f});
    registry.destroyEntity(entityId);

    EXPECT_FALSE(registry.hasComponent<Velocity>(entityId));
}