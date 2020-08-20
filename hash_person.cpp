#include <tuple>
#include <functional>

using namespace std;

struct Address {
    string city, street;
    int building;

    bool operator==(const Address& other) const {
        return tie(city, street, building) == tie(other.city, other.street, other.building);
    }
};

struct Person {
    string name;
    int height;
    double weight;
    Address address;

    bool operator==(const Person& other) const {
        return tie(name, height, weight, address) ==
            tie(other.name, other.height, other.weight, other.address);
    }
};

struct AddressHasher {
    hash<string> str_hash;
    static const size_t param = 173;
    size_t operator() (const Address& address) const{
        return str_hash(address.city) * param * param + str_hash(address.street) * param + address.building;
    }
};

struct PersonHasher {
    AddressHasher address_hasher;
    hash<string> str_hash;
    hash<double>  double_hash;
    static const size_t param = 373;
    size_t operator() (const Person& person) const{
        return str_hash(person.name) * param * param * param + person.height * param * param +
        double_hash(person.weight) * param + address_hasher(person.address);
    }
};
