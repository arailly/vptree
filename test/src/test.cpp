#include <gtest/gtest.h>
#include <random>
#include <arailib.hpp>
#include <vptree.hpp>

using namespace std;
using namespace arailib;
using namespace vptree;

TEST(vptree, partition) {
    const auto p0 = Data<>(0, {0});
    const auto p1 = Data<>(1, {1});
    const auto p2 = Data<>(2, {2});
    const auto p3 = Data<>(3, {3});
    const auto p4 = Data<>(4, {4});
    auto series = Series<>{p0, p1, p2, p3, p4};

    auto vpt = VPTree();
    vpt.set_nodes(series);

    const auto ref_nodes = vpt.make_ref_nodes(vpt.nodes);
    const auto partitioned = vpt.partition_nodes(vpt.nodes[2], 1, ref_nodes);
    const auto inner_nodes = partitioned.first;
    const auto outer_nodes = partitioned.second;

    ASSERT_EQ(inner_nodes.size(), 2);
    ASSERT_EQ(outer_nodes.size(), 2);
}

TEST(vptree, search) {
    const auto query = Data<>(0, {1.5, 1.5});
    float range = 1;
    int nrows = 4, ncols = 4;
    size_t id = 0;
    auto series = Series<>();
    for (int i = 0; i < nrows; i++) {
        float x = i;
        for (int j = 0; j < ncols; j++) {
            float y = j;
            series.push_back(Data<>(id, {x, y}));
            id++;
        }
    }

    auto vpt = VPTree();
    vpt.build(series);

    const auto result = vpt.range_search(query, range);
    ASSERT_EQ(result.series.size(), 4);
}

TEST(vptree, angular_search) {
    const auto query = Data<>(0, {1.5, 1.5});
    float range = 0.01;
    int nrows = 4, ncols = 4;
    size_t id = 0;

    mt19937 engine(42);
    normal_distribution<float> distribution(0, 1);

    auto series = Series<>();
    for (int i = 0; i < nrows; i++) {
        float x = i;// + distribution(engine);
        for (int j = 0; j < ncols; j++) {
            float y = j;// + distribution(engine);
            series.push_back(Data<>(id, {x, y}));
            id++;
        }
    }

    auto vpt = VPTree("angular");
    vpt.build(series);

    const auto result = vpt.range_search(query, range);
    ASSERT_EQ(result.series.size(), 3);
}