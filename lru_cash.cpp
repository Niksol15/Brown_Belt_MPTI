#include "Common.h"
#include <list>
#include <unordered_map>
#include <mutex>

using namespace std;

class LruCache : public ICache {
public:
    LruCache(
            shared_ptr<IBooksUnpacker> books_unpacker,
            const Settings& settings
    ): unpacker(move(books_unpacker)), free_memory(settings.max_memory) {
    }

    BookPtr GetBook(const string& book_name) override {
        if(lock_guard guard(m); name_to_iterator.count(book_name)){
            auto it = name_to_iterator[book_name];
            cash.push_front(*it);
            name_to_iterator[book_name] = cash.begin();
            cash.erase(it);
            return cash.front();
        } else {
            unique_ptr<IBook> book = unpacker -> UnpackBook(book_name);
            ClearExcessMemory(book -> GetContent().size());
            if(book -> GetContent().size() <= free_memory){
                cash.push_front(move(book));
                name_to_iterator[cash.front() -> GetName()] = cash.begin();
                free_memory -= cash.front() -> GetContent().size();
                return cash.front();
            } else {
                return move(book);
            }
        }
    }
private:
    shared_ptr<IBooksUnpacker> unpacker;
    //Settings setting;
    size_t free_memory;
    list<BookPtr> cash;
    unordered_map<string, list<BookPtr>::iterator> name_to_iterator;
    mutex m;
    void ClearExcessMemory(size_t needed_memory){
        while(!cash.empty() && free_memory < needed_memory){
            name_to_iterator.erase(cash.back() -> GetName());
            free_memory += cash.back() -> GetContent().size();
            cash.pop_back();
        }
    }
};


unique_ptr<ICache> MakeCache(
        shared_ptr<IBooksUnpacker> books_unpacker,
        const ICache::Settings& settings
) {
    return make_unique<LruCache>(move(books_unpacker), settings);
}
