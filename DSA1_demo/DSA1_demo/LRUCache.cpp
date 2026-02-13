#include "LRUCache.h"

LRUCache::LRUCache(int cap, CacheBackend b)
    : capacity(cap), backend(b), hitCount(0), missCount(0),
    totalAccessTime(0), operationCount(0) {
}

string LRUCache::get(int key) {
    auto start = chrono::high_resolution_clock::now();

    list<pair<int, string>>::iterator listIt;
    bool found = false;

    switch (backend) {
    case CacheBackend::HASH_TABLE: {
        auto it = hashMap.find(key);
        if (it != hashMap.end()) {
            found = true;
            listIt = it->second;
        }
        break;
    }

    case CacheBackend::BINARY_TREE: {
        auto it = treeMap.find(key);
        if (it != treeMap.end()) {
            found = true;
            listIt = it->second;
        }
        break;
    }

    case CacheBackend::LINEAR_SEARCH: {
        for (auto& pair : vectorMap) {
            if (pair.first == key) {
                found = true;
                listIt = pair.second;
                break;
            }
        }
        break;
    }
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    totalAccessTime += duration.count();
    operationCount++;

    if (!found) {
        missCount++;
        return "MISS";
    }

    // Cache hit - move to front
    hitCount++;
    cacheList.splice(cacheList.begin(), cacheList, listIt);
    return listIt->second;
}

void LRUCache::put(int key, const string& value) {
    auto start = chrono::high_resolution_clock::now();

    list<pair<int, string>>::iterator listIt;
    bool found = false;

    // Check if key exists in current backend
    switch (backend) {
    case CacheBackend::HASH_TABLE: {
        auto it = hashMap.find(key);
        if (it != hashMap.end()) {
            found = true;
            listIt = it->second;
        }
        break;
    }

    case CacheBackend::BINARY_TREE: {
        auto it = treeMap.find(key);
        if (it != treeMap.end()) {
            found = true;
            listIt = it->second;
        }
        break;
    }

    case CacheBackend::LINEAR_SEARCH: {
        for (auto& pair : vectorMap) {
            if (pair.first == key) {
                found = true;
                listIt = pair.second;
                break;
            }
        }
        break;
    }
    }

    if (found) {
        // Update existing
        cacheList.splice(cacheList.begin(), cacheList, listIt);
        listIt->second = value;

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        totalAccessTime += duration.count();
        operationCount++;
        return;
    }

    // New key - check capacity
    if (cacheList.size() == capacity) {
        // Evict LRU
        int oldKey = cacheList.back().first;

        switch (backend) {
        case CacheBackend::HASH_TABLE:
            hashMap.erase(oldKey);
            break;
        case CacheBackend::BINARY_TREE:
            treeMap.erase(oldKey);
            break;
        case CacheBackend::LINEAR_SEARCH:
            vectorMap.erase(
                remove_if(vectorMap.begin(), vectorMap.end(),
                    [oldKey](const auto& p) { return p.first == oldKey; }),
                vectorMap.end()
            );
            break;
        }

        cacheList.pop_back();
    }

    // Add new item
    cacheList.push_front({ key, value });
    listIt = cacheList.begin();

    switch (backend) {
    case CacheBackend::HASH_TABLE:
        hashMap[key] = listIt;
        break;
    case CacheBackend::BINARY_TREE:
        treeMap[key] = listIt;
        break;
    case CacheBackend::LINEAR_SEARCH:
        vectorMap.push_back({ key, listIt });
        break;
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    totalAccessTime += duration.count();
    operationCount++;
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
    hashMap.clear();
    treeMap.clear();
    vectorMap.clear();
    hitCount = 0;
    missCount = 0;
    totalAccessTime = 0;
    operationCount = 0;
}

void LRUCache::setBackend(CacheBackend newBackend) {
    // Rebuild index with new backend
    backend = newBackend;

    hashMap.clear();
    treeMap.clear();
    vectorMap.clear();

    for (auto it = cacheList.begin(); it != cacheList.end(); ++it) {
        int key = it->first;

        switch (backend) {
        case CacheBackend::HASH_TABLE:
            hashMap[key] = it;
            break;
        case CacheBackend::BINARY_TREE:
            treeMap[key] = it;
            break;
        case CacheBackend::LINEAR_SEARCH:
            vectorMap.push_back({ key, it });
            break;
        }
    }
}

string LRUCache::getBackendName() const {
    switch (backend) {
    case CacheBackend::HASH_TABLE: return "Hash Table (unordered_map)";
    case CacheBackend::BINARY_TREE: return "Binary Search Tree (map)";
    case CacheBackend::LINEAR_SEARCH: return "Linear Search (vector)";
    default: return "Unknown";
    }
}