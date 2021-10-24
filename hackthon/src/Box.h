#ifndef BOX_H
#define BOX_H

#include "Include.h"

class Box
{
public:
    Box(string name, size_t id, double width, double height, double deepth, double weight): _name{name}, _id{id}, _width{width}, _height{height}, _deepth{deepth}, _weight{weight}{}

    string _name;
    size_t _id;
    double _width, _height, _deepth, _weight;
    double _x, _y, _z;
    size_t _orient;
    double _center; // ????????????????????????????????????????????
    vector<vector<size_t>> _corner;

    static bool compById(const Box* a, const Box* b)
    {
        return (a->_width + a->_height + a->_deepth) > (b->_width + b->_height + b->_deepth);
    }

    double getWidth(size_t orient){ 
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
    double getHeight(size_t orient){ 
        switch(orient) { 
            case 3: case 4: 
                return _width; 
                break; 
            case 0: case 2: 
                return _height; 
                break; 
            case 1: case 5: 
                return _deepth; 
                break; 
            default: 
                cout << "Error in orient height" << endl;
                return -1;
                break;
        } 
    }
    double getDeepth(size_t orient){ 
        switch(orient) { 
            case 2: case 5: 
                return _width; 
                break; 
            case 1: case 3: 
                return _height; 
                break; 
            case 0: case 4: 
                return _deepth; 
                break; 
            default: 
                cout << "Error in orient deepth" << endl;
                return -1;
                break;
        } 
    }
};

#endif