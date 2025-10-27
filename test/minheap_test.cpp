#include <gtest/gtest.h>
#include "../src/MinHeap.h"
#include "../src/MinHeap.tpp"

TEST(MinHeapTest, CreateEmptyHeap)
{
    MinHeap<int> heap(10);
    EXPECT_TRUE(heap.isEmpty());
    EXPECT_FALSE(heap.isFull());
}

TEST(MinHeapTest, InsertElement)
{
    MinHeap<int> heap(10);
    heap.insert(5);
    EXPECT_FALSE(heap.isEmpty());
    EXPECT_EQ(heap.getMin(), 5);
}

TEST(MinHeapTest, InsertMultiple)
{
    MinHeap<int> heap(10);
    heap.insert(5);
    heap.insert(3);
    heap.insert(7);

    EXPECT_EQ(heap.getMin(), 3);
}

TEST(MinHeapTest, ExtractMin)
{
    MinHeap<int> heap(10);
    heap.insert(5);
    heap.insert(3);
    heap.insert(7);

    EXPECT_EQ(heap.extractMin(), 3);
    EXPECT_EQ(heap.extractMin(), 5);
    EXPECT_EQ(heap.extractMin(), 7);
}

TEST(MinHeapTest, HeapProperty)
{
    MinHeap<int> heap(10);
    heap.insert(5);
    heap.insert(4);
    heap.insert(3);
    heap.insert(2);
    heap.insert(1);

    EXPECT_EQ(heap.extractMin(), 1);
    EXPECT_EQ(heap.extractMin(), 2);
    EXPECT_EQ(heap.extractMin(), 3);
}

TEST(MinHeapTest, HeapFull)
{
    MinHeap<int> heap(3);
    heap.insert(1);
    heap.insert(2);
    heap.insert(3);

    EXPECT_TRUE(heap.isFull());
}
