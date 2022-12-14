#include <iostream>
#include <string>
#include <cassert>
#include <sstream>
using namespace std;
class vec
{
public:
    float data[3];

    float& operator[](size_t i)
    {
        return data[i];
    }

    const float& operator[](size_t i) const
    {
        return data[i];
    }
};

class object
{
public:
    std::string name = "";
};

class sphere : public object
{
public:
    vec center;
    double radius;
};

class point : public object
{
public:
    vec location;
};

void set_names(sphere* objects, size_t number_objects, const std::string& base_name)
{
    for(size_t i = 0; i < number_objects; i++)
    {
        objects[i].name = base_name + to_string(i);
    }
}

void set_names(point* objects, size_t number_objects, const std::string& base_name)
{
    for(size_t i = 0; i < number_objects; i++)
    {
        objects[i].name = base_name + to_string(i);
    }
}

int main()
{
    sphere * balls = new sphere[10];
    point * points = new point[10];

    vec origin;
    for(size_t i = 0; i < 3; i++)
        origin[i] = 0;
    
    for(size_t i = 0; i < 10; i++)
    {
        balls[i].center = origin;
        balls[i].radius = 0.1 * (i + 1);
        points[i].location = origin;
        points[i].location[0] = 0.2 * i;
    }
    set_names(balls, 10, "ball-");
    set_names(points, 10, "point-");

    for(size_t i = 0; i < 10; i++)
    {
        std::cout << balls[i].name << " " << points[i].name << std::endl;
    }

    delete [] balls;
    delete [] points;
    
    return 0;
}

