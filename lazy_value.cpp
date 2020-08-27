#include <functional>
#include <optional>
#include <utility>

using namespace std;

template <typename T>
class LazyValue {
public:
    explicit LazyValue(function<T()> init): constructor(move(init)){}

    bool HasValue() const{
        return optional_value.has_value();
    }
    const T& Get() const{
        if(!optional_value.has_value()){
            optional_value = constructor();
        }
        return optional_value.value();
    }

private:
    function<T()> constructor;
    mutable optional<T> optional_value = nullopt;
};
