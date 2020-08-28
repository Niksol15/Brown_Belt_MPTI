#include <future>
#include <mutex>
#include <unordered_map>
#include <vector>
using namespace std;

template <typename K, typename V, typename Hash = std::hash<K>>
class ConcurrentMap {
public:
    using MapType = unordered_map<K, V, Hash>;

    struct WriteAccess {
        lock_guard<mutex> guard;
        V& ref_to_value;
    };

    struct ReadAccess {
        lock_guard<mutex> guard;
        const V& ref_to_value;
    };

    explicit ConcurrentMap(size_t bucket_count): hash_maps(bucket_count){
    }

    WriteAccess operator[](const K& key){
        auto& curr_map = hash_maps[get_map_index(key)];
        return WriteAccess{lock_guard(curr_map.bucket_mutex), curr_map.hash_map[key]};
    }
    ReadAccess At(const K& key) const{
        auto& curr_map = hash_maps[get_map_index(key)];
        return ReadAccess{lock_guard(curr_map.bucket_mutex), curr_map.hash_map.at(key)};
    }

    bool Has(const K& key) const{
        return hash_maps[get_map_index(key)].hash_map.count(key);
    }

    MapType BuildOrdinaryMap() const{
        MapType result;
        for(const auto& map: hash_maps){
            for(const auto& [key, value]: map.hash_map){
                result[key] = At(key).ref_to_value;
            }
        }
        return result;
    }

private:
    struct Bucket{
        MapType hash_map;
        mutable mutex bucket_mutex;
    };
    Hash hasher;
    vector<Bucket> hash_maps;
    inline size_t get_map_index(const K& key) const{
        return hasher(key) % hash_maps.size();
    }
};
