# VPTree
## Overview
It is implementation of range search on Vantage-Point Tree.

The main algorithm is written `include/vptree.hpp` (header only), so you can use it easily.

Reference: https://fribbels.github.io/vptree/writeup

## Example
```
#include <iostream>
#include <arailib.hpp>
#include <vptree.hpp>

using namespace std;
using namespace arailib;
using namespace vptree;

int main() {
    const string data_path = "path/to/data.csv";
    const string query_path = "path/to/query.csv";
    const unsigned n = 1000; // data size
    const unsigned n_query = 1; // query size
    const float range = 10; // search range (distance)
    const string& distance = "euclidean"; // distance name; we support "euclidean", "manhattan", and "angular".

    const auto queries = read_csv(query_path, n_query); // read query file

    auto vpt = VPTree(distance); // init vptree index
    vpt.build(data_path, n); // build index
    
    vector<SearchResult> results(queries.size());
    for (const auto& query : queries) {
        results[query.id] = vpt.range_search(query, range);
    }
}
```

## Input File Format
If you want to create index with this three vectors, `(0, 1), (2, 4), (3, 3)`, you must describe data.csv like following format:
```
0,1
2,4
3,3
```

Query format is same as data format.
