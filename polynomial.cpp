#include <vector>
#include <utility>

using namespace std;

template<typename T>
class Polynomial {
private:
    std::vector<T> coeffs_ = {0};

    void Shrink(){
        while (coeffs_.size() > 1 && coeffs_.back() == 0) {
            coeffs_.pop_back();
        }
    }
    struct IndexProxy{
        Polynomial<T>& ref_to_polynom;
        size_t degree = 0;

        IndexProxy(Polynomial& poly, size_t degree) : ref_to_polynom(poly), degree(degree) {}

        operator T() const{
            return as_const(ref_to_polynom)[degree];
        }

        IndexProxy& operator= (T value){
            if(degree > ref_to_polynom.Degree()){
                ref_to_polynom.coeffs_.resize(degree + 1);
            }
            ref_to_polynom.coeffs_[degree] = value;
            ref_to_polynom.Shrink();
            return *this;
        }
    };
public:
    Polynomial() = default;
    Polynomial(vector<T> coeffs) : coeffs_(std::move(coeffs)) {
        Shrink();
    }

    template<typename Iterator>
    Polynomial(Iterator first, Iterator last) : coeffs_(first, last) {
        Shrink();
    }

    bool operator ==(const Polynomial& other) const {
        return coeffs_ == other.coeffs_;
    }

    bool operator !=(const Polynomial& other) const {
        return !operator==(other);
    }

    int Degree() const {
        return coeffs_.size() - 1;
    }

    Polynomial& operator +=(const Polynomial& r) {
        if (r.coeffs_.size() > coeffs_.size()) {
            coeffs_.resize(r.coeffs_.size());
        }
        for (size_t i = 0; i != r.coeffs_.size(); ++i) {
            coeffs_[i] += r.coeffs_[i];
        }
        Shrink();
        return *this;
    }

    Polynomial& operator -=(const Polynomial& r) {
        if (r.coeffs_.size() > coeffs_.size()) {
            coeffs_.resize(r.coeffs_.size());
        }
        for (size_t i = 0; i != r.coeffs_.size(); ++i) {
            coeffs_[i] -= r.coeffs_[i];
        }
        Shrink();
        return *this;
    }

    T operator [](size_t degree) const {
        return degree < coeffs_.size() ? coeffs_[degree] : 0;
    }

    IndexProxy operator [](size_t degree) {
        return IndexProxy(*this, degree);
    }

    T operator ()(const T& x) const {
        T res = 0;
        for (auto it = coeffs_.rbegin(); it != coeffs_.rend(); ++it) {
            res *= x;
            res += *it;
        }
        return res;
    }

    using const_iterator = typename std::vector<T>::const_iterator;

    const_iterator begin() const {
        return coeffs_.cbegin();
    }

    const_iterator end() const {
        return coeffs_.cend();
    }
};
