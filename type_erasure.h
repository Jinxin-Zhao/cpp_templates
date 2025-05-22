#ifndef TYPE_REASURE_H
#define TYPE_REASURE_H

#include <vector>
#include <iostream>
#include <string>
// type erasure in C++
// an example of type erasure in c++ standard library:

// datatype can be set when using qsort in C
// void qsort(void * base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
/*
    int less_int(const void * a, const void * b){
        return *(int *)a - *(int *)b;
    }
    int arr[8] = {1,2,3,4,5,6,7,8};
    qsort(arr, sizeof(arr)/sizeof(arr[0]), sizeof(int), less_int);
*/

// "interface + implementation" to realize type erasure

class Context{
public:
    Context() {
        // Initialize context
    }
};

class Shape {
public:
    virtual void Draw(const Context & ctx) const = 0;
};

class Circle : public Shape {
public:
    Circle(int radius) : m_radius(radius) {}
    void Draw(const Context & ctx) const override {
        // draw circle
    }
    int m_radius;
};

class Rectangle : public Shape {
public:
    Rectangle(int width, int height) : m_width(width), m_height(height) {}
    void Draw(const Context & ctx) const override {
        // draw rectangle
    }
    int m_width;
    int m_height;
};

void DrawShape(const std::vector<Shape*> & shapes) {
    Context ctx;
    for (const auto & shape : shapes) {
        shape->Draw(ctx);
    }
}
///////////////////////////////////////////////////////////////


// Draw() has no idea about the implementation of Circle and Rectangle
// it only knows that they are derived from Shape
// this is the essence of type erasure
// the caller of draw() does not need to know the details of the shapes
// it only needs to know that they are derived from Shape

// if there is a new shape, it needs to derive from Shape and implement Draw()
// how to avoid the situation ? using template !

// outer ploymorphic function based on template
class CRectangle {
public:
        CRectangle(int width, int height) : m_width(width), m_height(height) {}
        int m_width;
        int m_height;
};

class CCircle {
    public:
        CCircle(int radius) : m_radius(radius) {}
        int m_radius;
};

void DrawShape(const Context& ctx, Circle circle);
void DrawShape(const Context& ctx, Rectangle rect);

template <typename T>
void DrawSingleShape(const T & shape) {
    Context ctx;
    DrawShape(ctx, shape);
}

// however, this is not totally type erasure
// std::vector<???> shapes;
// void DrawShape(const std::vector<???> & shapes) {
//     for (const auto & shape : shapes) {
//         DrawSingleShape(shape);
//     }
// }
///////////////////////////////////////////////////////////////

// totally type erasure reference to Klaus lglberger
class DShape {
private:
    struct ShapeConcept {
        virtual ~ShapeConcept() = default;
        virtual void Draw(const Context & ctx) const = 0;
    };

    template <typename T>
    struct ShapeModel : ShapeConcept {
        ShapeModel(T shape) : m_shape(std::move(shape)) {}
        void Draw(const Context & ctx) const override {
            DrawSingleShape(ctx, m_shape);
        }
        T m_shape;
    };

    std::unique_ptr<ShapeConcept> m_shape;

public:
    template <typename T>
    DShape(const T & shape) : m_shape(std::make_unique<ShapeModel<T>>(std::move(shape))) {}
    
    void Draw(const Context & ctx) const {
        m_shape->Draw(ctx);
    }
};

#endif // TYPE_REASURE_H