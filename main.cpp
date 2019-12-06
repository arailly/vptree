#include <iostream>
#include <arailib.hpp>
#include <vptree.hpp>

using namespace std;
using namespace arailib;
using namespace vptree;

void save_results(const string& save_path, const vector<SearchResult>& results) {
    ofstream ofs(save_path);
    string line = "time,n_result\n";
    ofs << line;

    for (const auto& result : results) {
        line = to_string(result.time) + "," +
               to_string(result.series.size());
        ofs << line << endl;
    }
}

int main() {
    const auto config = read_config();
    const string data_path = config["data_path"];
    const string query_path = config["query_path"];
    const string save_path = config["save_path"];
    const unsigned n = config["n"];
    const float range = config["range"];

    const auto queries = read_csv(query_path);

    auto vpt = VPTree();
    vpt.build(data_path, n);
    cout << "complete: build" << endl;

    vector<SearchResult> results;
    for (const auto& query : queries) {
        const auto result = vpt.search(query, range);
        results.push_back(result);
    }

    save_results(save_path, results);
}