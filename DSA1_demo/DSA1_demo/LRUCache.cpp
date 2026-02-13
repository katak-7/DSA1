#include "LRUCache.h"

LRUCache::LRUCache(int cap) : capacity(cap), hitCount(0), missCount(0) {}

string LRUCache::get(int key) {
    auto it = cacheMap.find(key);

    if (it == cacheMap.end()) {
        // Cache miss
        missCount++;
        return "MISS";
    }

    // Cache hit - move to front (most recently used)
    hitCount++;
    cacheList.splice(cacheList.begin(), cacheList, it->second);
    return it->second->second;
}

void LRUCache::put(int key, const string& value) {
    auto it = cacheMap.find(key);

    if (it != cacheMap.end()) {
        // Key exists - update and move to front
        cacheList.splice(cacheList.begin(), cacheList, it->second);
        it->second->second = value;
        return;
    }

    // New key
    if (cacheList.size() == capacity) {
        // Cache full - evict least recently used (back of list)
        int oldKey = cacheList.back().first;
        cacheMap.erase(oldKey);
        cacheList.pop_back();
    }

    // Add new item to front
    cacheList.push_front({ key, value });
    cacheMap[key] = cacheList.begin();
}

vector<pair<int, string>> LRUCache::getCurrentState() {
    vector<pair<int, string>> state;
    for (const auto& item : cacheList) {
        state.push_back(item);
    }
    return state;
}

void LRUCache::clear() {
    cacheList.clear();
    cacheMap.clear();
    hitCount = 0;
    missCount = 0;
}