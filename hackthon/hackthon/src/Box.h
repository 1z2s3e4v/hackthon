#ifndef BOX_H
#define BOX_H

#include "Include.h"

class Box
{
public:
    Box() {};
    Box(string name, size_t id, size_t type_id, size_t width, size_t height, size_t deepth, size_t weight): _name{name}, _id{id}, _type_id{type_id}, _width{width}, _height{height}, _deepth{deepth}, _weight{weight}{}
    Box& operator=(const Box& rhs) {
        _name = rhs._name;
        _id = rhs._id;
        _width = rhs._width;
        _weight = rhs._weight;
        _height = rhs._height;
        _deepth = rhs._deepth;
        _orient = rhs._orient;
        _corner = rhs._corner;
        _x = rhs._x;
        _y = rhs._y;
        _z = rhs._z;
        return *this;
    }

    string _name;
    size_t _id, _type_id, _container_id;;
    size_t _width, _height, _deepth, _weight;
    size_t _x, _y, _z;
    size_t _orient;
    double _center; // ????????????????????????????????????????????
    vector<vector<size_t>> _corner;

    static bool compById(const Box* a, const Box* b)
    {
        return (a->_width + a->_height + a->_deepth) > (b->_width + b->_height + b->_deepth);
    }

    size_t getWidth(size_t orient){ 
        switch(orient) { 
            case 0: case 1: 
                return _width; 
                break; 
            case 4: case 5: 
                return _height; 
                break; 
            case 2: case 3: 
                return _deepth; 
                break; 
            default: 
                cout << "Error in orient width" << endl;
                return -1;
                break;
        } 
    }
    size_t getHeight(size_t orient){ 
        switch(orient) { 
            case 2: case 5: 
                return _width; 
                break; 
            case 0: case 3: 
                return _height; 
                break; 
            case 1: case 4: 
                return _deepth; 
                break; 
            default: 
                cout << "Error in orient height" << endl;
                return -1;
                break;
        } 
    }
    size_t getDeepth(size_t orient){ 
        switch(orient) { 
            case 3: case 4: 
                return _width; 
                break; 
            case 1: case 2: 
                return _height; 
                break; 
            case 0: case 5: 
                return _deepth; 
                break; 
            default: 
                cout << "Error in orient deepth" << endl;
                return -1;
                break;
        } 
    }

    size_t getVolume() {
        return _width * _height * _deepth;
    }
};

#endif