#include <cstddef>
#include <utility>

using namespace std;


template <typename T>
class UniquePtr {
private:
    T* pointer;
public:
    UniquePtr():  pointer(nullptr){}
    explicit UniquePtr(T * ptr): pointer(move(ptr)){}
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr(UniquePtr&& other): pointer(other.Release()){}
    UniquePtr& operator = (const UniquePtr&) = delete;
    UniquePtr& operator = (nullptr_t){
        delete pointer;
        pointer = nullptr;
        return *this;
    }
    UniquePtr& operator = (UniquePtr&& other){
        if(this != &other){
            delete pointer;
            pointer = other.Release();
        }
        return *this;
    }
    ~UniquePtr(){
        delete pointer;
    }

    T& operator * () const{
        return *pointer;
    }

    T * operator -> () const{
        return pointer;
    }

    T * Release(){
        T* temp = pointer;
        pointer = nullptr;
        return temp;
    }

    void Reset(T * ptr){
        if(ptr != pointer){
            delete pointer;
            pointer = ptr;
        }
    }

    void Swap(UniquePtr& other){
        swap(pointer, other.pointer);
    }

    [[nodiscard]] T * Get() const{
        return pointer;
    }
};
