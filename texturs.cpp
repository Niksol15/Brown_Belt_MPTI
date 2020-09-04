#include "Common.h"

using namespace std;

bool IsPointInSize(Point p, Size s) {
    return p.x >= 0 && p.y >= 0 && p.x < s.width && p.y < s.height;
}

Size ImageSize(const Image& image) {
    auto width = static_cast<int>(image.empty() ? 0 : image[0].size());
    auto height = static_cast<int>(image.size());
    return {width, height};
}

class Shape: public IShape{
public:
    Shape(): position({0, 0}), _size({0,0}){}
    void SetPosition(Point point) override{
        position = point;
    }

    Point GetPosition() const override{
        return position;
    }

    void SetSize(Size size) override{
        _size = size;
    }
    Size GetSize() const override{
        return _size;
    }

    void SetTexture(std::shared_ptr<ITexture> other) override{
        texture = other;
    }
    ITexture* GetTexture() const override{
        return texture.get();
    }

    void Draw(Image& image) const override{
        Point local_point;
        Size image_size = ImageSize(image);
        for(local_point.y = 0; local_point.y < _size.height; ++local_point.y){
            for(local_point.x = 0; local_point.x < _size.width; ++local_point.x){
                Point global_point{position.x + local_point.x, position.y + local_point.y};
                if(IsPointInShape(local_point) &&
                    IsPointInSize(global_point, image_size)){
                    if(texture != nullptr &&
                        IsPointInSize(local_point, texture -> GetSize())){
                        image[global_point.y][global_point.x] =
                                texture -> GetImage()[local_point.y][local_point.x];
                    } else {
                        image[global_point.y][global_point.x] = '.';
                    }
                }
            }
        }
    }
protected:
    Point position;
    Size _size;
    std::shared_ptr<ITexture> texture = nullptr;
    virtual bool IsPointInShape(Point) const = 0;
};

class Ellipse: public Shape{
public:
    Ellipse(): Shape(){}
    std::unique_ptr<IShape> Clone() const override{
        return make_unique<Ellipse>(*this);
    }
private:
    bool IsPointInShape(Point point) const override {
        return IsPointInEllipse(point, _size);
    }
};

class Rectangle: public Shape{
public:
    Rectangle(): Shape(){}
    std::unique_ptr<IShape> Clone() const override{
        return make_unique<Rectangle>(*this);
    }
private:
    bool IsPointInShape(Point point) const override {
        return point.x < _size.width && point.y < _size.height;
    }
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
    switch(shape_type){
        case ShapeType::Ellipse:
            return std::make_unique<Ellipse>();
        case ShapeType::Rectangle:
            return std::make_unique<Rectangle>();
        default:
            return nullptr;
    }
}
