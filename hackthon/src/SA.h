#ifndef SA_H
#define SA_H

#include "Pallet.h"
#include "Box.h"
#include "Include.h"

class SA
{
public:
    SA(vector<size_t> container, vector<Pallet*> palletV, vector<Box*> boxV, double totalVolume): _container{container}, _palletV{palletV}, _boxV{boxV}, _totalVolume{totalVolume}
    {
        _n = boxV.size();
        _curGammaV.resize(3);
        _bestGammaV.resize(3);
        _curOrientV.resize(_n);
        _bestOrientV.resize(_n);
        _isBefore = vector<vector<vector<bool> > >(_n, vector<vector<bool> >(_n, vector<bool>(3)));
    }

    void solve();
    void printBoxInfo();
    void outputBoxResult();
    bool evaluate(vector<Box*>&);
    void updateBoxInfo(vector<string> gammaV, vector<size_t> orientV); // also performs legalization
private:
    // Input
    vector<size_t> _container;
    vector<Pallet*> _palletV;
    vector<Box*> _boxV;
    size_t _totalVolume;
    size_t _n;  // size of objects

    // SA
    // Gamma1, Gamma2, Gamma3
    vector<string> _curGammaV;
    vector<string> _bestGammaV;
    vector<size_t> _curOrientV;
    vector<size_t> _bestOrientV;
    // _isBefore[n][n][3]: a is before b in dimension c
    vector<vector<vector<bool> > > _isBefore;
    double _bestCost;
    double _curTemp;
    double _endTemp;
    double _r;
    size_t _P;

    void init();
    pair<vector<string>, vector<size_t>> pickNeighbor();
    pair<double, double> calCost(vector<string> gammaV, vector<size_t> orientV);
};

#endif