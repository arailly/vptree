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
        const Point point;
        float r;
        Node* inner;
        Node* outer;
        Node(Point& p) : point(move(p)) {}
    };

    typedef vector<reference_wrapper<Node>> RefNodes;

    struct VPTree {
        vector<Node> nodes;
        Node* root;
        mt19937 engine;

        VPTree(unsigned random_state = 42) : engine(mt19937(random_state)) {}

        RefNodes make_ref_nodes(vector<Node>& process_nodes) const {
            RefNodes result;
            for (auto& pn : process_nodes) result.emplace_back(pn);
            return result;
        }

        void set_nodes(Series& series) {
            for (auto& point : series) nodes.push_back(Node(point));
        }

        void build(Series& series) {
            // set nodes
            set_nodes(series);

            // build
            auto ref_nodes = make_ref_nodes(nodes);
            root = build_level(ref_nodes);
        }

        pair<RefNodes, RefNodes> partition_nodes(const Node& node, float mid_dist,
                                                 const RefNodes& process_nodes) const {
            RefNodes inner_nodes, outer_nodes;
            for (const auto& pn : process_nodes) {
                if (pn.get().point == node.point) continue;
                const auto dist = euclidean_distance(node.point, pn.get().point);
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

            // select middle node
            const auto mid_index = [random_id, &process_nodes]() {
                const auto mi = process_nodes.size() / 2;
                // unless conflict with random_id
                if (mi != random_id) return mi;
                // unless overflow
                else if (mi + 1 < process_nodes.size()) return mi + 1;
                else return mi - 1;
            }();
            const auto mid_node = process_nodes[process_nodes.size() / 2].get();
            const float mid_dist = euclidean_distance(node->point, mid_node.point);
            node->r = mid_dist;

            // divide inner or outer
            auto [inner_nodes, outer_nodes] = partition_nodes(*node, mid_dist, process_nodes);

            node->inner = build_level(inner_nodes);
            node->outer = build_level(outer_nodes);

            return node;
        }

        Series search(const Point& query, const float range) {
            const auto result = search_level(query, range, root);
            return result;
        }

        Series search_level(const Point& query, const float range, const Node* node) {
            Series result;
            const auto dist = euclidean_distance(query, node->point);

            if (dist < range) result.push_back(node->point);

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
