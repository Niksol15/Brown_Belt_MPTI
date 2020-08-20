#include <tuple>
#include <functional>

using namespace std;

using CoordType = int;

struct Point3D {
    CoordType x;
    CoordType y;
    CoordType z;

    bool operator==(const Point3D& other) const {
        return  tie(x, y, z) == tie(other.x, other.y, other.z);
    }
};

struct Hasher {
    size_t operator() (const Point3D& point) const{
        size_t param = 373;
        return _hash(point.x) * param * param + _hash(point.y) * param + _hash(point.z);
    }

    hash<CoordType> _hash;
};
