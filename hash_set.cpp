#include <forward_list>
#include <iterator>

using namespace std;

template <typename Type, typename Hasher>
class HashSet {
public:
    using BucketList = forward_list<Type>;

    explicit HashSet(size_t num_buckets, const Hasher& hasher = {}):
        SIZE(num_buckets), hash_table(SIZE), _hasher(hasher){
    }

    void Add(const Type& value){
        if(find_by_value(value) == hash_table[get_index(value)].end()){
            hash_table[get_index(value)].push_front(value);
        }
    }
    bool Has(const Type& value) const{
        return  find_by_value(value) !=
                hash_table[get_index(value)].end();
    }
    void Erase(const Type& value){
        auto it = find_by_value(value);
        size_t index = get_index(value) ;
        if (it != hash_table[index].end()){
            hash_table[index].erase_after(it);
        }
    }
    const BucketList& GetBucket(const Type& value) const{
        return hash_table[_hasher(value) % SIZE];
    }
private:
    auto find_by_value(const Type& value) const {
        auto it = hash_table[get_index(value)].before_begin();
        while(it != hash_table[get_index(value)].end()){
            if(next(it) != hash_table[get_index(value)].end() && *next(it) == value){
                return it;
            }
            ++it;
        }
        return it;
    }
    inline size_t get_index(const Type& value) const{
        return _hasher(value) % SIZE;
    }
    const size_t SIZE;
    vector<BucketList> hash_table;
    Hasher _hasher;
};
