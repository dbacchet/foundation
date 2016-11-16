#include <iostream>
#include <thread>

#include "gtest/gtest.h"

#include "common/foundation_types.h"


TEST(ID, Creation) {
    ID id;
    ASSERT_EQ(id.index,UINT32_MAX);
    ASSERT_EQ(id.internal_id,UINT32_MAX);
    ASSERT_EQ(id.next_free_idx,UINT32_MAX);
    ID id2(100,200);
    ASSERT_EQ(id2.index,100);
    ASSERT_EQ(id2.internal_id,200);
}

TEST(ID, Valid) {
    ID id;
    ID id2(100,200);
    ASSERT_FALSE(valid(id));
    ASSERT_TRUE(valid(id2));
}

