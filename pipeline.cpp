#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

struct Email {
    Email() = default;
    Email(string from_, string to_, string body_):
        from(move(from_)), to(move(to_)), body(move(body_)){}
    string from;
    string to;
    string body;
};


class Worker {
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run() {
        throw logic_error("Unimplemented");
    }

protected:
    void PassOn(unique_ptr<Email> email) const{
        if(next_worker){
            next_worker -> Process(move(email));
        }
    }

    unique_ptr<Worker> next_worker;

public:
    void SetNext(unique_ptr<Worker> next){
        next_worker = move(next);
    }
};


class Reader : public Worker {
public:
    explicit Reader(istream& is): messages(is){
    }

    void Process(unique_ptr<Email> email) override {
        throw logic_error("Unimplemented");
    }
    
    void Run() override{
        string from, to, body;
        while (getline(messages, from) && getline(messages, to) &&
            getline(messages, body)){
            PassOn(make_unique<Email>(from, to, body));
        }

    }
private:
    istream& messages;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

    Filter(Function func): predicate(move(func)){}

    void Process(unique_ptr<Email> email) override{
        if(predicate(*email)){
            PassOn(move(email));
        }
    }
private:
    Function predicate;
};


class Copier : public Worker {
public:
    Copier(string address): to_address(move(address)){}

    void Process(unique_ptr<Email> email) override{
        if(email->to != to_address){
            Email copy_email{email -> from, to_address, email -> body};
            PassOn(move(email));
            PassOn(make_unique<Email>(move(copy_email)));
        } else {
            return PassOn(move(email));
        }
    }

private:
    string to_address;
};


class Sender : public Worker {
public:
    Sender(ostream& os_): os(os_){}
    void Process(unique_ptr<Email> email) override{
        os << email -> from << "\n" << email -> to << "\n" << email -> body << "\n";
        PassOn(move(email));
    }
private:
    ostream& os;
};


class PipelineBuilder {
public:
    explicit PipelineBuilder(istream& in){
        workers.push_back(make_unique<Reader>(in));
    }

    PipelineBuilder& FilterBy(Filter::Function filter){
        workers.push_back(make_unique<Filter>(filter));
        return *this;
    }

    PipelineBuilder& CopyTo(string recipient){
        workers.push_back(make_unique<Copier>(recipient));
        return *this;
    }

    PipelineBuilder& Send(ostream& out){
        workers.push_back(make_unique<Sender>(out));
        return *this;
    }

    unique_ptr<Worker> Build(){
        for (size_t i = workers.size() - 1; i > 0; --i) {
            workers[i - 1]->SetNext(move(workers[i]));
        }
        return move(workers[0]);
    }

private:
    vector<unique_ptr<Worker>> workers;
};
