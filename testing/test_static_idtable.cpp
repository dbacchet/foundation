#include <iostream>
#include <thread>

#include "gtest/gtest.h"

#include "common/static_idtable.h"

namespace {
}

TEST(StaticIDTable, Creation) {
    StaticIDTable<int64_t,16> idtable;
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
    ASSERT_EQ(idtable.size(),0);
}

TEST(StaticIDTable, Add) {
    StaticIDTable<int64_t,16> idtable;
    idtable.add();
    idtable.add();
    ASSERT_EQ(idtable.size(),2);
}

TEST(StaticIDTable, Get) {
    StaticIDTable<int64_t,16> idtable;
    auto id1 = idtable.add(1);
    auto id2 = idtable.add(2);
    auto id3 = idtable.add(3);
    ASSERT_EQ(idtable.size(),3);
    ASSERT_EQ(idtable.get(id1),1);
    ASSERT_EQ(idtable.get(id2),2);
    ASSERT_EQ(idtable.get(id3),3);
}

TEST(StaticIDTable, Has) {
    StaticIDTable<int64_t,16> idtable;
    auto id1 = idtable.add(1);
    auto id2 = idtable.add(2);
    auto id3 = idtable.add(3);
    ASSERT_TRUE(idtable.has(id1));
    ASSERT_TRUE(idtable.has(id2));
    ASSERT_TRUE(idtable.has(id3));
    ASSERT_FALSE(idtable.has(ID{101,102}));
}

TEST(StaticIDTable, Remove) {
    StaticIDTable<int64_t,16> idtable;
    auto id1 = idtable.add(1);
    auto id2 = idtable.add(2);
    auto id3 = idtable.add(3);
    ASSERT_EQ(idtable.size(),3);
    bool success = idtable.remove(id2);
    ASSERT_TRUE(success);
    ASSERT_EQ(idtable.size(),2);
    success = idtable.remove(id2); // try to remove an element alredy removed
    ASSERT_FALSE(success);
    ASSERT_EQ(idtable.size(),2);
    ASSERT_TRUE(idtable.has(id1));
    ASSERT_FALSE(idtable.has(id2));
    ASSERT_TRUE(idtable.has(id3));
    // check that the slots in the _ids and _objects are reused
    auto id4 = idtable.add(4);
    // check the internal oredering
    ASSERT_EQ(idtable._objects[0],1);
    ASSERT_EQ(idtable._objects[1],3);
    ASSERT_EQ(idtable._objects[2],4);
    ASSERT_EQ(idtable._ids[0].index,0);
    ASSERT_EQ(idtable._ids[1].index,2);
    ASSERT_EQ(idtable._ids[2].index,1);
    ASSERT_TRUE(idtable.has(id1));
    ASSERT_FALSE(idtable.has(id2));
    ASSERT_TRUE(idtable.has(id3));
    ASSERT_TRUE(idtable.has(id4));
}

