//
// Created by Yusuke Arai on 2019/12/04.
//

#ifndef VPTREE_VPTREE_HPP
#define VPTREE_VPTREE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <arailib.hpp>

using namespace std;
using namespace arailib;

namespace vptree {
    struct Node {
        const Data<> data;
        float r;
        Node* inner;
        Node* outer;
        int n_children;
        Node(Data<>& p) : data(move(p)), inner(nullptr), outer(nullptr),
                          n_children(0) {}
    };

    typedef vector<reference_wrapper<Node>> RefNodes;

    struct SearchResult {
        time_t time = 0;
        Series<> series;
    };

    struct VPTree {
        vector<Node> nodes;
        Node* root;
        const DistanceFunction<> df;
        mt19937 engine;

        VPTree(const string& df = "euclidean",
               const unsigned random_state = 42) :
               df(select_distance(df)), engine(mt19937(random_state)) {}

        RefNodes make_ref_nodes(vector<Node>& process_nodes) const {
            RefNodes result;
            for (auto& pn : process_nodes) result.emplace_back(pn);
            return result;
        }

        void set_nodes(Series<>& series) {
            for (auto& point : series) nodes.push_back(Node(point));
        }

        void build(Series<>& series) {
            // set nodes
            set_nodes(series);

            // build
            auto ref_nodes = make_ref_nodes(nodes);
            root = build_level(ref_nodes);
        }

        void build(const string& data_path, const int n) {
            auto series = load_data(data_path, n);
            build(series);
        }

        pair<RefNodes, RefNodes> partition_nodes(const Node& node, float mid_dist,
                                                 const RefNodes& process_nodes) const {
            RefNodes inner_nodes, outer_nodes;
            for (const auto& pn : process_nodes) {
                if (pn.get().data == node.data) continue;
                const auto dist = df(node.data, pn.get().data);
                if (dist <= mid_dist) inner_nodes.push_back(pn);
                else outer_nodes.push_back(pn);
            }
            return {inner_nodes, outer_nodes};
        }

        Node* build_level(RefNodes& process_nodes) {
            if (process_nodes.size() <= 0) return nullptr;

            // select process node
            uniform_int_distribution<size_t> distribution(0, process_nodes.size() - 1);
            const auto random_id = distribution(engine);
            auto* node = &(process_nodes[random_id].get());

            const auto mid_node = process_nodes[process_nodes.size() / 2].get();
            const float mid_dist = df(node->data, mid_node.data);
            node->r = mid_dist;

            // divide inner or outer
            auto partitioned = partition_nodes(*node, mid_dist, process_nodes);

            // calc n_children
            node->n_children = partitioned.first.size() + partitioned.second.size();

            node->inner = build_level(partitioned.first);
            node->outer = build_level(partitioned.second);

            return node;
        }

        SearchResult range_search(const Data<>& query, const float range) {
            const auto start = get_now();
            auto result = SearchResult();
            result.series = search_level(query, range, root);
            const auto end = get_now();
            result.time = get_duration(start, end);
            return result;
        }

        Series<> search_level(const Data<>& query, const float range, const Node* node) {
            Series<> result;
            const auto dist = df(query, node->data);

            if (dist < range) result.push_back(node->data);

            if (dist - range < node->r && node->inner) {
                const auto inner_result = search_level(query, range, node->inner);
                for (const auto& e : inner_result) result.push_back(e);
            }

            if (dist + range > node->r && node->outer) {
                const auto outer_result = search_level(query, range, node->outer);
                for (const auto& e : outer_result) result.push_back(e);
            }

            return result;
        }
    };
}

#endif //VPTREE_VPTREE_HPP
