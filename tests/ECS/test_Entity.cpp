#include <gtest/gtest.h>
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EcsTypes.hpp"

struct Health { int hp; };

class EntityTest : public ::testing::Test 
{
    protected:
        nut::Registry registry;
};

TEST_F(EntityTest, EqualityOperatorsWorkCorrectly) 
{
    nut::Entity e1 = registry.createEntity();
    nut::Entity e2 = registry.createEntity();
    
    nut::Entity e1_copy = e1; 

    EXPECT_EQ(e1, e1_copy);
    EXPECT_NE(e1, e2);
}

TEST_F(EntityTest, CopiedEntitySharesSameRegistryAndState) 
{
    nut::Entity e1 = registry.createEntity();
    e1.addComponent<Health>(Health{100});

    nut::Entity e2 = e1;
    EXPECT_EQ(e1, e2);
    EXPECT_EQ(e1.getId(), e2.getId());
    EXPECT_EQ(e1.getVersion(), e2.getVersion());
    
    ASSERT_TRUE(e2.hasComponent<Health>());
    EXPECT_EQ(e2.getComponent<Health>().hp, 100);

    e2.getComponent<Health>().hp = 50;
    EXPECT_EQ(e1.getComponent<Health>().hp, 50);
}

TEST_F(EntityTest, ComponentMethodsDelegateToRegistry) 
{
    nut::Entity entity = registry.createEntity();
    nut::EntityId id = entity.getId();

    entity.addComponent<Health>(Health{50});
    ASSERT_TRUE(entity.hasComponent<Health>());
    EXPECT_EQ(entity.getComponent<Health>().hp, 50);
    ASSERT_TRUE(registry.hasComponent<Health>(id));
    EXPECT_EQ(registry.getComponent<Health>(id).hp, 50);

    entity.removeComponent<Health>();
    ASSERT_FALSE(entity.hasComponent<Health>());
    ASSERT_FALSE(registry.hasComponent<Health>(id));
}

TEST_F(EntityTest, DestroyMethodDelegatesToRegistry) 
{
    nut::Entity entity = registry.createEntity();
    nut::EntityId id = entity.getId();

    EXPECT_TRUE(entity);

    entity.destroy();
    EXPECT_FALSE(entity);
    EXPECT_FALSE(registry.isValid(id));
}