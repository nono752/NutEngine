#include <gtest/gtest.h>
#include "ECS/internal/SparseSet.hpp"
#include "ECS/EcsTypes.hpp"

struct Health { int hp; };

namespace nut::internal
{
    class SparseSetTest : public ::testing::Test
    {
        protected:
            SparseSet<Health> sparseSet;
    };
    struct SparseSetTester 
    {
        template <typename T>
        static const std::vector<EntityId>& getDenseId(const SparseSet<T>& set) 
        {
            return set.getDenseId(); 
        }
    };

    TEST_F(SparseSetTest, SizeCorrespondElementsCount) 
    {
        EXPECT_EQ(sparseSet.size(), 0);

        sparseSet.insert(5, Health{100});
        EXPECT_EQ(sparseSet.size(), 1);

        sparseSet.insert(10, Health{200});
        EXPECT_EQ(sparseSet.size(), 2);
    }

    TEST_F(SparseSetTest, EraseMaintainsContiguityAndUpdatesSize) 
    {
        sparseSet.insert(10, Health{10});
        sparseSet.insert(20, Health{20});
        sparseSet.insert(30, Health{30});
        EXPECT_EQ(sparseSet.size(), 3);

        sparseSet.erase(20);
        EXPECT_FALSE(sparseSet.contains(20));
        EXPECT_TRUE(sparseSet.contains(10));
        EXPECT_TRUE(sparseSet.contains(30));

        // denseId and denseData should have same size
        const std::vector<EntityId>& denseId = SparseSetTester::getDenseId(sparseSet);
        EXPECT_EQ(sparseSet.size(), 2);
        EXPECT_EQ(sparseSet.size(), denseId.size());

        // check the swap
        EXPECT_EQ(sparseSet.get(10).hp, 10);
        EXPECT_EQ(sparseSet.get(30).hp, 30);
    }

    TEST_F(SparseSetTest, ClearRemovesAllElements) 
    {
        sparseSet.insert(1, Health{10});
        sparseSet.insert(2, Health{20});
        sparseSet.insert(3, Health{30});
        EXPECT_EQ(sparseSet.size(), 3);

        sparseSet.clear();
        EXPECT_EQ(sparseSet.size(), 0);

        EXPECT_FALSE(sparseSet.contains(1));
        EXPECT_FALSE(sparseSet.contains(2));
        EXPECT_FALSE(sparseSet.contains(3));
    }

    TEST_F(SparseSetTest, SparseVectorResizesDynamicallyAndCorrectly) 
    {
        nut::EntityId bigId = 10000;
        nut::EntityId half = bigId/2;

        sparseSet.insert(1, Health{100});
        sparseSet.insert(bigId, Health{200});

        EXPECT_TRUE(sparseSet.contains(1));
        EXPECT_FALSE(sparseSet.contains(half));
        EXPECT_TRUE(sparseSet.contains(bigId));

        sparseSet.reserve(bigId * 2);
        EXPECT_TRUE(sparseSet.contains(1));
        EXPECT_FALSE(sparseSet.contains(half));
        EXPECT_TRUE(sparseSet.contains(bigId));
        EXPECT_FALSE(sparseSet.contains(bigId + half));
    }
}