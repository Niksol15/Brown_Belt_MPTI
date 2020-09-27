#pragma once

namespace RAII{
    template<typename Provider>
    class Booking{
    public:
        using BookingId = typename Provider::BookingId;
        Booking() = default;
        Booking(Provider* new_provider, BookingId new_id):
            provider(new_provider){}
        Booking(Booking<Provider> &&other) : provider(other.provider) {
            other.provider = nullptr;
        }
        Booking<Provider>& operator=(Booking<Provider>&& other) {
            delete provider;
            provider = other.provider;
            other.provider = nullptr;
            return *this;
        }
        Booking(const Booking<Provider>&) = delete;
        Booking<Provider>& operator=(const Booking<Provider>&) = delete;
        ~Booking(){
            if (provider != nullptr) {
                provider->CancelOrComplete(*this);
            }
        }
    private:
        Provider* provider;
    };
}
