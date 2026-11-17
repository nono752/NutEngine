#include <gtest/gtest.h>
#include "ECS/Registry.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EcsTypes.hpp"
#include "ECS/SystemMng.hpp"
#include "ECS/ISystem.hpp"
#include "ECS/View.hpp"

struct Position { int x = 0; };
struct Velocity { int dx = 1; };
struct Hp { int hp = 0; };

struct MoveSystem : public nut::ISystem
{
    void update(nut::Registry& registry, float dt) override
    {
        nut::View<Velocity, Position> view = registry.view<Velocity, Position>();
        for(auto [id, vel, pos] : view.components()) 
        {
            if (registry.getComponent<Hp>(id).hp != 0) continue;
            pos.x += vel.dx;
        }
    }
};
struct DamageSystem : public nut::ISystem
{
    int damage = 1;

    void update(nut::Registry& registry, float dt) override
    {
        nut::View<Hp> view = registry.view<Hp>();
        for(auto [id, health] : view.components()) 
        {
            health.hp -= damage;
        }
    }
};

class SystemMngTest : public ::testing::Test 
{
    protected:
        nut::Registry registry;
        nut::SystemMng sysm;
        nut::Entity e1;

        SystemMngTest()
        {
            e1 = registry.createEntity();
            e1.addComponent<Position> ();
            e1.addComponent<Velocity> ();
            e1.addComponent<Hp> ();
        }
};

TEST_F(SystemMngTest, LayersUpdatedSequentialy)
{
    EXPECT_EQ(sysm.getLayerCount(), 1);

    nut::SystemLayerId lastId = sysm.createLayer();
    ASSERT_EQ(sysm.getLayerCount(), 2);
    EXPECT_EQ(lastId, 1);

    auto damageSys = nut::makeSystem<DamageSystem>();
    sysm.addSystem<DamageSystem>(damageSys);
    MoveSystem* moveSys = sysm.addSystem<MoveSystem>(lastId);
    ASSERT_TRUE(damageSys);
    ASSERT_TRUE(moveSys);

    sysm.update(registry, 0);
    EXPECT_EQ(e1.getComponent<Hp>().hp, -1);
    EXPECT_EQ(e1.getComponent<Position>().x, 0);
}

TEST_F(SystemMngTest, LayersEnablingMecanic)
{
    sysm.createLayer();
    sysm.addSystem<DamageSystem>(0);
    sysm.addSystem<MoveSystem>(1);

    sysm.setLayerEnabled(0, false);
    ASSERT_FALSE(sysm.isLayerEnabled(0));

    sysm.update(registry, 0);
    EXPECT_EQ(e1.getComponent<Hp>().hp, 0);
    EXPECT_EQ(e1.getComponent<Position>().x, 1);

    sysm.setLayerEnabled(0, true);
    ASSERT_TRUE(sysm.isLayerEnabled(0));

    sysm.update(registry, 0);
    EXPECT_EQ(e1.getComponent<Hp>().hp, -1);
    EXPECT_EQ(e1.getComponent<Position>().x, 1);
}

TEST_F(SystemMngTest, SystemEnablingMecanic)
{
    DamageSystem* ds = sysm.addSystem<DamageSystem>(0);
    sysm.addSystem<MoveSystem>(0);

    ds->setEnabled(false);
    ASSERT_FALSE(ds->isEnabled());

    sysm.update(registry, 0);
    EXPECT_EQ(e1.getComponent<Hp>().hp, 0);
    EXPECT_EQ(e1.getComponent<Position>().x, 1);

    ds->setEnabled(true);
    ASSERT_TRUE(ds->isEnabled());

    sysm.update(registry, 0);
    EXPECT_EQ(e1.getComponent<Hp>().hp, -1);
    EXPECT_EQ(e1.getComponent<Position>().x, 1);
}

TEST_F(SystemMngTest, MultipleSystemMngWithSharedSystem)
{
    nut::SystemMng sysm2;
    nut::Registry registry2;
    nut::Entity e2 = registry2.createEntity();
    e2.addComponent<Position> ();
    e2.addComponent<Velocity> ();
    e2.addComponent<Hp> ();

    auto ms = nut::makeSystem<MoveSystem>();
    sysm.addSystem<MoveSystem>(ms);
    sysm2.addSystem<MoveSystem>(ms);

    sysm.update(registry, 0);
    sysm2.update(registry2, 0);
    EXPECT_EQ(e1.getComponent<Position>().x, 1);
    EXPECT_EQ(e2.getComponent<Position>().x, 1);

    ms->setEnabled(false);
    sysm.update(registry, 0);
    sysm2.update(registry2, 0);
    EXPECT_EQ(e1.getComponent<Position>().x, 1);
    EXPECT_EQ(e2.getComponent<Position>().x, 1);

    ms->setEnabled(true);
    sysm2.setLayerEnabled(0, false);
    sysm.update(registry, 0);
    sysm2.update(registry2, 0);
    EXPECT_EQ(e1.getComponent<Position>().x, 2);
    EXPECT_EQ(e2.getComponent<Position>().x, 1);
}

TEST_F(SystemMngTest, checkAssertForInvalidLayerId)
{
    EXPECT_DEATH(sysm.setLayerEnabled(10, false), "Error::setLayerEnabled: invalid layer id.");
    EXPECT_DEATH(sysm.isLayerEnabled(10), "Error::isLayerEnabled: invalid layer id.");

    auto ms = nut::makeSystem<MoveSystem>();
    EXPECT_DEATH(sysm.addSystem<MoveSystem>(ms, 10), "Error::addSystem: invalid layer id.");
    ms = nullptr;
    EXPECT_DEATH(sysm.addSystem<MoveSystem>(ms, 0), "Error::addSystem: system cannot be nullptr.");

    EXPECT_DEATH(sysm.addSystem<MoveSystem>(10), "Error::addSystem: invalid layer id.");

    EXPECT_DEATH(sysm.addSystem(10, [](nut::Registry&, float) {}), "Error::addSystem: invalid layer id.");
}

TEST_F(SystemMngTest, AddSystemWithLambda)
{
    int lambdaCalls = 0;
    nut::ISystem* lSys = sysm.addSystem([&lambdaCalls](nut::Registry&, float) {
        ++lambdaCalls;
    });
    EXPECT_NE(lSys, nullptr);

    sysm.update(registry, 0);
    EXPECT_EQ(lambdaCalls, 1);
}