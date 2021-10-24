#include "SA.h"
#include "Graph.h"
#include "Include.h"

void SA::solve()
{
    init();

    size_t cnt = 0;
    while(_curTemp >= _endTemp){
        // cout << "curTemp: " << _curTemp << endl;
        for(size_t i = 0; i < _P; i++){
            pair<vector<string>, vector<size_t>> neighbor = pickNeighbor();
            vector<string> newGammaV = neighbor.first;
            vector<size_t> newOrientV = neighbor.second;
            double newCost = calCost(newGammaV, newOrientV).first;
            double oldCost = calCost(_curGammaV, _curOrientV).first;
            double delta = newCost - oldCost;
            if(delta <= 0){
                _curGammaV = newGammaV;
                _curOrientV = newOrientV;
                if(newCost < _bestCost){
                    _bestGammaV = newGammaV;
                    _bestOrientV = newOrientV;
                    _bestCost = newCost;
                }
                continue;
            }

            double prob = rand()%10001/10000.0;
            // cout << "(T, prob, exp(-delta/_curTemp/_totalVolume)): (" << _curTemp << ", " << prob << ", " << exp(-delta/_curTemp/_totalVolume) << ")" << endl;
            if(prob <= exp(-delta/_curTemp/_totalVolume)){
                _curGammaV = newGammaV;
                _curOrientV = newOrientV;
            }
        }
        _curTemp *= _r;
        // if(cnt%500 == 0){
        //     cout << "best cost(scaled, true): (" << calCost(_bestGammaV, _bestOrientV).first << ", " << calCost(_bestGammaV, _bestOrientV).second << ")" << endl;
        // }
        cnt++;
    }
    if(_totalVolume > _bestCost){
        cout << "Error! space disappear! (totalVolume, bestCost): (" << _totalVolume << ", " << _bestCost << ")" << endl;
    }
    cout << "bestCost: " << calCost(_bestGammaV, _bestOrientV).second << endl;
    cout << "Utilization rate: " << _totalVolume*1.0/_bestCost << endl;
    updateBoxInfo(_bestGammaV, _bestOrientV);
}

void SA::init()
{
    if(_n > 127){
        cout << "I cannot handle it" << endl;
    }
    // Generate gamma1, gamma2, gamma3
    for(size_t i = 0; i < _n; i++){
        char ch = (char)i;
        _curGammaV[0] += ch;
        _curGammaV[1] += ch;
        _curGammaV[2] += ch;
    }
    random_shuffle(_curGammaV[0].begin(), _curGammaV[0].end());
    random_shuffle(_curGammaV[1].begin(), _curGammaV[1].end());
    random_shuffle(_curGammaV[2].begin(), _curGammaV[2].end());

    for(size_t i = 0; i < _n; i++){
        _curOrientV[i] = 0;
        _bestOrientV[i] = 0;
    }

    _bestGammaV = _curGammaV;
    _bestOrientV = _curOrientV;

    _curTemp = 1000;
    _endTemp = 0.00001;
    _r = 0.9995;
    _P = 15;

    _bestCost = calCost(_curGammaV, _curOrientV).first;
}

pair<vector<string>, vector<size_t>> SA::pickNeighbor()
{
    vector<string> newGammaV = _curGammaV;
    vector<size_t> newOrient = _curOrientV;
    size_t i = rand()%5;
    if(i <= 2){
        size_t j = rand()%_n;
        size_t k = rand()%_n;
        while(k == j){
            k = rand()%_n;
        }
        newGammaV[i][j] = _curGammaV[i][k];
        newGammaV[i][k] = _curGammaV[i][j];
    }
    else{
        size_t j = rand()%_n;
        size_t curOrientJ = _curOrientV[j];
        size_t newOrientJ = rand()%6;
        while(curOrientJ == newOrientJ){
            newOrientJ = rand()%6;
        }
        newOrient[j] = newOrientJ;
    }

    return make_pair(newGammaV, newOrient);
}

pair<double, double> SA::calCost(vector<string> gammaV, vector<size_t> orientV)
{
    // Compute the relationship between i&&j
    for(size_t i = 0; i < _n; i++){
        for(size_t j = 0; j < i; j++){
            if(i == j)
                continue;
            char chI = (char)i;
            char chJ = (char)j;
            for(size_t k = 0; k < 3; k++){
                size_t indexI = gammaV[k].find(chI);
                size_t indexJ = gammaV[k].find(chJ);
                if(indexI < indexJ){
                    _isBefore[i][j][k] = true;
                }
                else if(indexI > indexJ){
                    _isBefore[i][j][k] = false;
                }
                else{
                    cout << "ERROR! duplicate objects" << endl;
                }
            }
        }
    }

    // Create graph
    Graph graphRear(_n);
    Graph graphRight(_n);
    Graph graphBelow(_n);
    for(size_t i = 0; i < _n; i++){
        for(size_t j = 0; j < i; j++){
            if(i == j)
                continue;
            if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 1)&&(_isBefore[i][j][2] == 1)){
                graphRear.addEdge(i, j);    // rear
            }
            else if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 1)&&(_isBefore[i][j][2] == 0)){
                graphRight.addEdge(j, i);   // left
            }
            else if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 1)){
                graphRight.addEdge(i, j);   // right
            }
            else if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 0)){
                graphBelow.addEdge(i, j);   // below
            }
            else if((_isBefore[i][j][0] == 0)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 0)){
                graphRear.addEdge(j, i);    // front
            }
            else if((_isBefore[i][j][0] == 0)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 1)){
                graphRight.addEdge(i, j);   // right
            }
            else if((_isBefore[i][j][0] == 0)&&(_isBefore[i][j][1] == 1)&&(_isBefore[i][j][2] == 0)){
                graphRight.addEdge(j, i);   // left
            }
            else{
                graphBelow.addEdge(j, i);   // above
            }
        }
    }

    vector<size_t> orderRear = graphRear.topologicalSort();
    vector<size_t> orderRight = graphRight.topologicalSort();
    vector<size_t> orderBelow = graphBelow.topologicalSort();
    vector<size_t> rearV, rightV, belowV;
    rearV.resize(_n);
    rightV.resize(_n);
    belowV.resize(_n);
    for(size_t i = 0; i < _n; i++){
        rearV[i] = rightV[i] = belowV[i] = 0;
    }
    size_t globalMaxRear = 0;
    size_t globalMaxRight = 0;
    size_t globalMaxBelow = 0;
    for(size_t i = 0; i < _n; i++){
        // Rear(deepth)
        size_t iRear = orderRear[i];
        size_t maxRear = 0;
        for(auto& id : graphRear.parentOfNode(iRear)){
            size_t curRear = _boxV[id]->getDeepth(orientV[id]) + rearV[id];
            if(curRear > maxRear){
                maxRear = curRear;
            }
        }
        rearV[iRear] = maxRear;
        size_t deepth = _boxV[iRear]->getDeepth(orientV[iRear]);
        if((maxRear + deepth) > globalMaxRear){
            globalMaxRear = maxRear + deepth;
        }

        // Right(width)
        size_t iRight = orderRight[i];
        size_t maxRight = 0;
        for(auto& id : graphRight.parentOfNode(iRight)){
            size_t curRight = _boxV[id]->getWidth(orientV[id]) + rightV[id];
            if(curRight > maxRight){
                maxRight = curRight;
            }
        }
        rightV[iRight] = maxRight;
        size_t width = _boxV[iRight]->getWidth(orientV[iRight]);
        if((maxRight + width) > globalMaxRight){
            globalMaxRight = maxRight + width;
        }

        // Below(height)
        size_t iBelow = orderBelow[i];
        size_t maxBelow = 0;
        for(auto& id : graphBelow.parentOfNode(iBelow)){
            size_t curBelow =  + (_boxV[id]->getHeight(orientV[id]) + belowV[id]);
            if(curBelow > maxBelow){
                maxBelow = curBelow;
            }
        }
        belowV[iBelow] = maxBelow;
        size_t height = _boxV[iBelow]->getHeight(orientV[iBelow]);
        if((maxBelow + height) > globalMaxBelow){
            globalMaxBelow = maxBelow + height;
        }
    }

    double trueCost = globalMaxRear*globalMaxRight*globalMaxBelow;
    double scaledCost = trueCost;

    bool isValid = true;
    if(globalMaxRear > _container[0]*0.95){
        scaledCost *= pow(globalMaxRear*1.0/_container[0]/0.95, 10);
        isValid = false;
    }    
    if(globalMaxRight > _container[1]*0.95){
        scaledCost *= pow(globalMaxRight*1.0/_container[1]/0.95, 10);
        isValid = false;
    }    
    if(globalMaxBelow > _container[2]*0.95){
        scaledCost *= pow(globalMaxBelow*1.0/_container[2]/0.95, 10);
        isValid = false;
    }

    return make_pair(scaledCost, trueCost);
}

void SA::updateBoxInfo(vector<string> gammaV, vector<size_t> orientV)
{
    // Compute the relationship between i&&j
    for(size_t i = 0; i < _n; i++){
        for(size_t j = 0; j < i; j++){
            if(i == j){
                continue;
            }
            char chI = (char)i;
            char chJ = (char)j;
            for(size_t k = 0; k < 3; k++){
                size_t indexI = gammaV[k].find(chI);
                size_t indexJ = gammaV[k].find(chJ);
                if(indexI < indexJ){
                    _isBefore[i][j][k] = true;
                }
                else if(indexI > indexJ){
                    _isBefore[i][j][k] = false;
                }
                else{
                    cout << "ERROR! duplicate objects" << endl;
                }
            }
        }
    }

    // Create graph
    Graph graphRear(_n);
    Graph graphRight(_n);
    Graph graphBelow(_n);
    for(size_t i = 0; i < _n; i++){
        for(size_t j = 0; j < i; j++){
            if(i == j)
                continue;
            if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 1)&&(_isBefore[i][j][2] == 1)){
                graphRear.addEdge(i, j);    // rear
            }
            else if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 1)&&(_isBefore[i][j][2] == 0)){
                graphRight.addEdge(j, i);   // left
            }
            else if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 1)){
                graphRight.addEdge(i, j);   // right
            }
            else if((_isBefore[i][j][0] == 1)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 0)){
                graphBelow.addEdge(i, j);   // below
            }
            else if((_isBefore[i][j][0] == 0)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 0)){
                graphRear.addEdge(j, i);    // front
            }
            else if((_isBefore[i][j][0] == 0)&&(_isBefore[i][j][1] == 0)&&(_isBefore[i][j][2] == 1)){
                graphRight.addEdge(i, j);   // right
            }
            else if((_isBefore[i][j][0] == 0)&&(_isBefore[i][j][1] == 1)&&(_isBefore[i][j][2] == 0)){
                graphRight.addEdge(j, i);   // left
            }
            else{
                graphBelow.addEdge(j, i);   // above
            }
        }
    }

    vector<size_t> orderRear = graphRear.topologicalSort();
    vector<size_t> orderRight = graphRight.topologicalSort();
    vector<size_t> orderBelow = graphBelow.topologicalSort();

    // cout << "Rear order: ";
    // for(size_t i = 0; i < _n; i++){
    //     cout << orderRear[i] << " ";
    // }
    // cout << endl;
    // cout << "Right order: ";
    // for(size_t i = 0; i < _n; i++){
    //     cout << orderRight[i] << " ";
    // }
    // cout << endl;
    // cout << "Below order: ";
    // for(size_t i = 0; i < _n; i++){
    //     cout << orderBelow[i] << " ";
    // }
    // cout << endl;

    vector<size_t> rearV, rightV, belowV;
    rearV.resize(_n);
    rightV.resize(_n);
    belowV.resize(_n);
    for(size_t i = 0; i < _n; i++){
        rearV[i] = rightV[i] = belowV[i] = 0;
    }
    size_t globalMaxRear = 0;
    size_t globalMaxRight = 0;
    size_t globalMaxBelow = 0;
    for(size_t i = 0; i < _n; i++){
        // Rear
        size_t iRear = orderRear[i];
        // cout << "Rear: " << iRear << endl;
        size_t maxRear = 0;
        for(auto& id : graphRear.parentOfNode(iRear)){
            size_t curRear = _boxV[id]->getDeepth(orientV[id]) + rearV[id];
            if(curRear > maxRear){
                maxRear = curRear;
            }
        }
        rearV[iRear] = maxRear;
        _boxV[iRear]->_x = maxRear;

        // Right
        size_t iRight = orderRight[i];
        size_t maxRight = 0;
        for(auto& id : graphRight.parentOfNode(iRight)){
            size_t curRight = _boxV[id]->getWidth(orientV[id]) + rightV[id];
            if(curRight > maxRight){
                maxRight = curRight;
            }
        }
        rightV[iRight] = maxRight;
        _boxV[iRight]->_y = maxRight;

        // Below(height)
        size_t iBelow = orderBelow[i];
        size_t maxBelow = 0;
        for(auto& id : graphBelow.parentOfNode(iBelow)){
            size_t curBelow = _boxV[id]->getHeight(orientV[id]) + belowV[id];
            if(curBelow > maxBelow){
                maxBelow = curBelow;
            }
        }
        belowV[iBelow] = maxBelow;
        _boxV[iBelow]->_z = maxBelow;
    }

    for(size_t i = 0; i < _n; i++){
        _boxV[i]->_orient = orientV[i];
    }

    // Create x, y, z, orient, d, w, h
    for(size_t i = 0; i < _n; i++){
        _boxV[i]->_corner.resize(8);
        for(size_t j = 0; j < 8; j++){
            _boxV[i]->_corner[j].resize(3);
        }
        size_t x = _boxV[i]->_x;
        size_t y = _boxV[i]->_y;
        size_t z = _boxV[i]->_z;
        size_t orient = _boxV[i]->_orient;
        size_t d = _boxV[i]->getDeepth(orient);
        size_t w = _boxV[i]->getWidth(orient);
        size_t h = _boxV[i]->getHeight(orient);

        size_t arr[8][3] = {
            {0, 0, 0},
            {d, 0, 0},
            {0, w, 0},
            {d, w, 0},
            {0, 0, h},
            {d, 0, h},
            {0, w, h},
            {d, w, h}
        };
        for(size_t j = 0; j < 8; j++){
            // cout << "(arr[j][0], arr[j][1], arr[j][2]): (" << arr[j][0] << ", " << arr[j][1] << ", " << arr[j][2] << ")" << endl;
            _boxV[i]->_corner[j][0] = x + arr[j][0];
            _boxV[i]->_corner[j][1] = y + arr[j][1];
            _boxV[i]->_corner[j][2] = z + arr[j][2];
        }
    }

    // Legalization along z-axis
    for(size_t i = 0; i < _n; i++){
        // Below(height)
        size_t iBelow = orderBelow[i];
        size_t maxBelow = 0;
        for(auto& id : graphBelow.parentOfNode(iBelow)){
            size_t jx = _boxV[id]->_x;
            size_t jy = _boxV[id]->_y;
            size_t jorient = _boxV[id]->_orient;
            size_t jd = _boxV[id]->getDeepth(jorient);
            size_t jw = _boxV[id]->getWidth(jorient);
            for(size_t k = 0; k < 4; k++){
                size_t ix = _boxV[iBelow]->_corner[k][0];
                size_t iy = _boxV[iBelow]->_corner[k][1];
                // if ix within jx && iy within jy
                bool isWithin = true;
                if((ix < jx) || ((jx+jd) < ix)){
                    isWithin = false;
                }
                if((iy < jy) || ((jy+jw) < iy)){
                    isWithin = false;
                }
                if(isWithin = true){
                    // update z axis of iBelow
                    size_t curBelow = _boxV[id]->getHeight(orientV[id]) + belowV[id];
                    if((curBelow > maxBelow)){
                        maxBelow = curBelow;
                    }
                }
            }
        }
        belowV[iBelow] = maxBelow;
        _boxV[iBelow]->_z = maxBelow;
    }

    // Create eight angles
    for(size_t i = 0; i < _n; i++){
        _boxV[i]->_corner.resize(8);
        for(size_t j = 0; j < 8; j++){
            _boxV[i]->_corner[j].resize(3);
        }
        size_t x = _boxV[i]->_x;
        size_t y = _boxV[i]->_y;
        size_t z = _boxV[i]->_z;
        size_t orient = _boxV[i]->_orient;
        size_t d = _boxV[i]->getDeepth(orient);
        size_t w = _boxV[i]->getWidth(orient);
        size_t h = _boxV[i]->getHeight(orient);

        size_t arr[8][3] = {
            {0, 0, 0},
            {d, 0, 0},
            {0, w, 0},
            {d, w, 0},
            {0, 0, h},
            {d, 0, h},
            {0, w, h},
            {d, w, h}
        };
        for(size_t j = 0; j < 8; j++){
            // cout << "(arr[j][0], arr[j][1], arr[j][2]): (" << arr[j][0] << ", " << arr[j][1] << ", " << arr[j][2] << ")" << endl;
            _boxV[i]->_corner[j][0] = x + arr[j][0];
            _boxV[i]->_corner[j][1] = y + arr[j][1];
            _boxV[i]->_corner[j][2] = z + arr[j][2];
        }
    }
}

void SA::printBoxInfo()
{
    size_t maxX = 0, maxY = 0, maxZ = 0;
    for(size_t i = 0; i < _n; i++){
        size_t x = _boxV[i]->_x;
        size_t y = _boxV[i]->_y;
        size_t z = _boxV[i]->_z;
        size_t orient = _boxV[i]->_orient;
        size_t w = _boxV[i]->getWidth(orient);
        size_t d = _boxV[i]->getDeepth(orient);
        size_t h = _boxV[i]->getHeight(orient);
        cout << "(i: x, y, z, d, w, h): (" << i << ", " << x << ", " << y << ", " << z << ", " << d << ", " << w << ", " << h << ")" << endl; 
        if((x+d) > maxX){
            maxX = x+d;
        }
        if((y+w) > maxY){
            maxY = y+w;
        }
        if((z+h) > maxZ){
            maxZ = z+h;
        }
    }
    // cout << "(maxX, maxY, maxZ, maxX*maxY*maxZ): (" << maxX << ", " << maxY << ", " << maxZ << ", " << maxX*maxY*maxZ << ")" << endl;
}

void SA::outputBoxResult()
{
    ofstream fout("SA_box_result.txt",ios_base::app);
    // pallete <w> <h> <d> <nBox>
    fout << "pallete " << _container[0] << " " << 12 << " " << _container[1] << " " << _n << " " << "\n";
    for(size_t i = 0; i < _n; i++){
        size_t type_id = _boxV[i]->_type_id;
        size_t x = _boxV[i]->_x;
        size_t z = _boxV[i]->_y;
        size_t y = _boxV[i]->_z;
        size_t orient = _boxV[i]->_orient;

        // switch (_boxV[i]->_orient){
        //     case 0: orient = 0;
        //     case 1: orient = 1; 
        //     case 2: orient = 3;
        //     case 3: orient = 2;
        //     case 4: orient = 5;
        //     case 5: orient = 4;
        // }
        // size_t d = _boxV[i]->getWidth(orient);
        // size_t h = _boxV[i]->getDeepth(orient);
        // size_t w = _boxV[i]->getHeight(orient);
        size_t d = _boxV[i]->_width;
        size_t w = _boxV[i]->_deepth;
        size_t h = _boxV[i]->_height;
        if (w == 0 && h == 0 && d == 0) continue;

        // <type_id> <w> <h> <d> <x> <y> <z> <r>
        fout << type_id << " " << w << " " << h << " " << d << " " << x << " " << y << " " << z << " " << orient << "\n";
    }
    fout.close();
}

bool SA::evaluate(vector<Box*>& unFitBoxed)
{
    bool isValid = true; // if this solution valid?
    size_t maxX = 0, maxY = 0, maxZ = 0;
    for(size_t i = 0; i < _n; i++){
        for(size_t j = 0; j < i; j++){
            // Check whether i&&j conflicts
            for(size_t k = 0; k < 8; k++){
                size_t ix = _boxV[i]->_corner[k][0];
                size_t iy = _boxV[i]->_corner[k][1];
                size_t iz = _boxV[i]->_corner[k][2];
                size_t jx = _boxV[j]->_x;
                size_t jy = _boxV[j]->_y;
                size_t jz = _boxV[j]->_z;
                size_t jorient = _boxV[j]->_orient;
                size_t jd = _boxV[j]->getDeepth(jorient);
                size_t jw = _boxV[j]->getWidth(jorient);
                size_t jh = _boxV[j]->getHeight(jorient);
                bool isWithin = false;

                if ((jx < ix) && (ix < jx + jd) &&
                    (jy < iy) && (iy < jy + jw) &&
                    (jz < iz) && (iz < jz + jh)) {
                    isWithin = true;
                }

                if(isWithin == true){
                    cout << "Error! overlap!" << endl;
                    return false;
                }

            }
        }
        // Check out of bound
        bool isOutOfBound = false;
        for(size_t k = 0; k < 8; k++){
            size_t ix = _boxV[i]->_corner[k][0];
            size_t iy = _boxV[i]->_corner[k][1];
            size_t iz = _boxV[i]->_corner[k][2];
            if(maxX < ix){
                maxX = ix;
            }
            if(maxY < iy){
                maxY = iy;
            }
            if(maxZ < iz){
                maxZ = iz;
            }

            if (ix > _container[0] ||
                iy > _container[1] ||
                iz > _container[2]) {
                isValid = false;
                isOutOfBound = true;
            }
        }

        if (isOutOfBound) {
            unFitBoxed.emplace_back(new Box());
            unFitBoxed[unFitBoxed.size() - 1] = _boxV[i];
            _boxV[i]->_width = 0;
            _boxV[i]->_height = 0;
            _boxV[i]->_deepth = 0;
        }
    }

    if(isValid == false){
        cout << "Pallet(x, y, z, x*y*z): (" << _container[0] << ", " << _container[1] << ", " << _container[2] << ", " << _container[0]*_container[1]*_container[2] << ")" << endl;
        cout << "Our(maxX, maxY, maxZ, maxX*maxY*maxZ): (" << maxX << ", " << maxY << ", " << maxZ << ", " << maxX*maxY*maxZ << ")" << endl;
        cout << "Error! out of pallet!" << endl;

        cout << "Try updateBoxInfo\n";
        updateBoxInfo(_bestGammaV, _bestOrientV);
        // return false;
    }

    cout << "Information Evaluate pass!" << endl;
    cout << "Pallet(x, y, z, x*y*z): (" << _container[0] << ", " << _container[1] << ", " << _container[2] << ", " << _container[0]*_container[1]*_container[2] << ")" << endl;
    cout << "_totalVolume: " << _totalVolume << endl;
    cout << "Our(maxX, maxY, maxZ, maxX*maxY*maxZ): (" << maxX << ", " << maxY << ", " << maxZ << ", " << maxX*maxY*maxZ << ")" << endl;
    cout << "Origial utilization rate: " << _totalVolume*1.0/(_container[0]*_container[1]*_container[2]) << endl;
    cout << "Our utilization rate: " << (maxX*maxY*maxZ)*1.0/(_container[0]*_container[1]*_container[2]) << endl;
    return true;
}