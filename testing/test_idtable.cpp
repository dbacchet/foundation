#include <iostream>
#include <thread>

#include "gtest/gtest.h"

#include "common/idtable.h"

namespace {
}


TEST(IDTable, Creation) {
    IDTable<int64_t> idtable;
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
    ASSERT_EQ(idtable.size(),0);
}

TEST(IDTable, Add) {
    IDTable<int64_t> idtable;
    idtable.add();
    idtable.add();
    ASSERT_EQ(idtable.size(),2);
}

TEST(IDTable, Get) {
    IDTable<int64_t> idtable;
    auto id1 = idtable.add(1);
    auto id2 = idtable.add(2);
    auto id3 = idtable.add(3);
    ASSERT_EQ(idtable.size(),3);
    ASSERT_EQ(idtable.get(id1),1);
    ASSERT_EQ(idtable.get(id2),2);
    ASSERT_EQ(idtable.get(id3),3);
}

TEST(IDTable, Has) {
    IDTable<int64_t> idtable;
    auto id1 = idtable.add(1);
    auto id2 = idtable.add(2);
    auto id3 = idtable.add(3);
    ASSERT_TRUE(idtable.has(id1));
    ASSERT_TRUE(idtable.has(id2));
    ASSERT_TRUE(idtable.has(id3));
    ASSERT_FALSE(idtable.has(ID{101,102}));
}

TEST(IDTable, Remove) {
    IDTable<int64_t> idtable;
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
    ASSERT_EQ(idtable._ids.size(),3);
    ASSERT_EQ(idtable._objects.size(),3);
    auto id4 = idtable.add(4);
    ASSERT_EQ(idtable._ids.size(),3);
    ASSERT_EQ(idtable._objects.size(),3);
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

