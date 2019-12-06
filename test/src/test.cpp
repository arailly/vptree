#include <gtest/gtest.h>
#include <random>
#include <arailib.hpp>
#include <vptree.hpp>

using namespace std;
using namespace arailib;
using namespace vptree;

TEST(vptree, partition) {
    const auto p0 = Point(0, {0});
    const auto p1 = Point(1, {1});
    const auto p2 = Point(2, {2});
    const auto p3 = Point(3, {3});
    const auto p4 = Point(4, {4});
    auto series = Series{p0, p1, p2, p3, p4};

    auto vpt = VPTree();
    vpt.set_nodes(series);

    const auto ref_nodes = vpt.make_ref_nodes(vpt.nodes);
    const auto [inner_nodes, outer_nodes] = vpt.partition_nodes(vpt.nodes[2], 1, ref_nodes);

    ASSERT_EQ(inner_nodes.size(), 2);
    ASSERT_EQ(outer_nodes.size(), 2);
}

TEST(vptree, build) {
    auto series = Series();
    int nrows = 3, ncols = 3;
    size_t id = 0;
    for (int i = 0; i < nrows; i++) {
        float x = i;
        for (int j = 0; j < ncols; j++) {
            float y = j;
            series.push_back(Point(id, {x, y}));
            id++;
        }
    }

    auto vpt = VPTree();
    vpt.build(series);

    ASSERT_EQ(vpt.root->point.id, 6);
    ASSERT_EQ(vpt.root->inner->point.id, 3);
    ASSERT_EQ(vpt.root->inner->inner->point.id, 4);
    ASSERT_EQ(vpt.root->inner->outer->point.id, 7);
}

TEST(vptree, search) {
    const auto query = Point(0, {1.5, 1.5});
    float range = 1;
    int nrows = 4, ncols = 4;
    size_t id = 0;
    auto series = Series();
    for (int i = 0; i < nrows; i++) {
        float x = i;
        for (int j = 0; j < ncols; j++) {
            float y = j;
            series.push_back(Point(id, {x, y}));
            id++;
        }
    }

    auto vpt = VPTree();
    vpt.build(series);

    const auto result = vpt.search(query, range);
    ASSERT_EQ(result.series.size(), 4);
}