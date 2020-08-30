#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class ValueType: public Expression{
public:
    explicit ValueType(int x): value(x){}

    int Evaluate() const override{
        return value;
    }

    string ToString() const override{
        return to_string(value);
    }
private:
    int value;
};

class SumType: public Expression{
public:
    SumType(ExpressionPtr _lhs, ExpressionPtr _rhs): lhs(move(_lhs)), rhs(move(_rhs)){}

    int Evaluate() const override{
        return lhs -> Evaluate() + rhs -> Evaluate();
    }

    string ToString() const override{
        return "(" + lhs -> ToString() + ")+(" + rhs -> ToString() + ")";
    }
private:
    ExpressionPtr lhs;
    ExpressionPtr rhs;
};

class ProductType: public Expression{
public:
    ProductType(ExpressionPtr _lhs, ExpressionPtr _rhs): lhs(move(_lhs)), rhs(move(_rhs)){}

    int Evaluate() const override{
        return lhs -> Evaluate() * rhs -> Evaluate();
    }

    string ToString() const override{
        return "(" + lhs -> ToString() + ")*(" + rhs -> ToString() + ")";
    }
private:
    ExpressionPtr lhs;
    ExpressionPtr rhs;
};

string Print(const Expression* e) {
    if (!e) {
        return "Null expression provided";
    }
    stringstream output;
    output << e->ToString() << " = " << e->Evaluate();
    return output.str();
}

ExpressionPtr Value(int value){
    return make_unique<ValueType>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right){
    return make_unique<SumType>(move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right){
    return make_unique<ProductType>(move(left), move(right));
}
