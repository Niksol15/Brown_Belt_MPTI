#include <map>
#include <string>
#include <unordered_map>
#include <utility>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

class Database {
public:
    bool Put(Record record){
        if(!data_base.count(record.id)){
            auto& db_record = data_base[record.id];
            db_record.record = move(record);
            db_record.timestamp_range_it = timestamp_range.emplace(db_record.record.timestamp, &db_record.record);
            db_record.karma_range_it = karma_range.emplace(db_record.record.karma, &db_record.record);
            db_record.user_id_range_it = user_id_range.emplace(db_record.record.user, &db_record.record);
            return true;
        }
        return false;
    }
    const Record* GetById(const string& id) const{
        if(data_base.count(id)){
            return &data_base.at(id).record;
        }
        return nullptr;
    }
    bool Erase(const string& id){
        if(data_base.count(id)){
            const auto& db_record = data_base[id];
            timestamp_range.erase(db_record.timestamp_range_it);
            karma_range.erase(db_record.karma_range_it);
            user_id_range.erase(db_record.user_id_range_it);
            data_base.erase(id);
            return true;
        }
        return false;
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const{
        auto range_end = timestamp_range.upper_bound(high);
        for(auto it = timestamp_range.lower_bound(low);
            it != range_end && callback(*(it->second)); ++it);
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const{
        auto range_end = karma_range.upper_bound(high);
        for(auto it = karma_range.lower_bound(low);
            it != range_end && callback(*(it->second)); ++it);
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const{
        auto finded = user_id_range.equal_range(user);
        for(auto it = finded.first; it != finded.second && callback(*(it->second)); ++it);
    }

private:
    template<typename T>
    using RecordRange   = multimap<T, const Record*>;
    struct DBRecord{
        Record record;
        RecordRange<int>::iterator timestamp_range_it;
        RecordRange<int>::iterator karma_range_it;
        RecordRange<string>::iterator user_id_range_it;
    };
    unordered_map<string, DBRecord> data_base;
    RecordRange<int> timestamp_range;
    RecordRange<int> karma_range;
    RecordRange<string> user_id_range;
};
