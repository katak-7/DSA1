#pragma once
#include <unordered_map>
#include <list>
#include <string>
#include <vector>

using namespace std;

class LRUCache {
private:
    int capacity;
    list<pair<int, string>> cacheList; // {key, value} - most recent at front
    unordered_map<int, list<pair<int, string>>::iterator> cacheMap; // key -> iterator

    int hitCount;
    int missCount;

public:
    LRUCache(int cap);
    string get(int key);
    void put(int key, const string& value);
    vector<pair<int, string>> getCurrentState();
    void clear();
    int getHitCount() const { return hitCount; }
    int getMissCount() const { return missCount; }
    int getSize() const { return cacheList.size(); }
    int getCapacity() const { return capacity; }
};