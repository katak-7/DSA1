#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <chrono>

using namespace std;

enum class CacheBackend {
    HASH_TABLE,      // unordered_map - O(1)
    BINARY_TREE,     // map - O(log n)
    LINEAR_SEARCH    // vector - O(n)
};

class LRUCache {
private:
    int capacity;
    CacheBackend backend;


    // Data structures for different backends
    list<pair<int, string>> cacheList;

    // Backend 1: Hash Table (unordered_map)
    unordered_map<int, list<pair<int, string>>::iterator> hashMap;

    // Backend 2: Binary Search Tree (map)
    map<int, list<pair<int, string>>::iterator> treeMap;

    // Backend 3: Linear Search (vector)
    vector<pair<int, list<pair<int, string>>::iterator>> vectorMap;

    int hitCount;
    int missCount;
    long long totalAccessTime; // in microseconds
    int operationCount;

public:
    LRUCache(int cap, CacheBackend backend = CacheBackend::HASH_TABLE);
    string get(int key);
    void put(int key, const string& value);
    vector<pair<int, string>> getCurrentState();
    void clear();
    void setBackend(CacheBackend newBackend);

    // Stats getters
    CacheBackend getBackend() const { return backend; }
    int getHitCount() const { return hitCount; }
    int getMissCount() const { return missCount; }
    int getSize() const { return cacheList.size(); }
    int getCapacity() const { return capacity; }
    double getAverageAccessTime() const {
        return operationCount > 0 ? (double)totalAccessTime / operationCount : 0;
    }
    string getBackendName() const;
};