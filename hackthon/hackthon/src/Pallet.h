#ifndef PALLET_H
#define PALLET_H

class Pallet
{
public:
    Pallet(double width, double height, double deepth, double weight): _width{width}, _height{height}, _deepth{deepth}, _weight{weight}{}

    double _width, _height, _deepth, _weight;
};

#endif