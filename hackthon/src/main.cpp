#include "Pallet.h"
#include "Box.h"
#include "SA.h"
#include "Include.h"

int main(int argc, char *argv[])
{
    vector<size_t> container;
    vector<Pallet*> palletV;
    // vector<Box*> boxV;
    vector<vector<Box*>> boxVV;

    container.resize(3);
    container[0] = 140;
    container[1] = 102;
    container[2] = 220;

    size_t n1 = 3;
    vector<size_t> totalVolumeV;
    // macro
    size_t cntPallet = 0;
    vector<Box*> tempBoxV;
    boxVV.resize(1);
    totalVolumeV.resize(1);
    // Insert box if is possible
    for(size_t i = 0; i < n1; i++){
        size_t width = rand()%(int)floor(container[0]*0.9)+1;
        size_t height = rand()%(int)floor(container[1]*0.9)+1;
        size_t deepth = rand()%(int)floor(container[2]*0.9)+1;
        size_t weight = rand()%1000+1;
        Box* BoxP = new Box(to_string(i), i, width, height, deepth, weight);
        totalVolumeV[cntPallet] += width*height*deepth;

        if(totalVolumeV[cntPallet] >= 0.50*container[0]*container[1]*container[2]){
            totalVolumeV[cntPallet] -= width*height*deepth;
            cout << "Utilization rate: " << totalVolumeV[cntPallet]*1.0/container[0]/container[1]/container[2] << endl;
            cntPallet++;
            // cout << "Create pallet " << cntPallet << endl;

            // cout << i << endl;
            vector<Box*> tempBoxV;
            boxVV.push_back(tempBoxV);

            size_t totalVolume = width*height*deepth;
            totalVolumeV.push_back(totalVolume);
        }

        boxVV[cntPallet].push_back(BoxP);
    }

    vector<SA*> SAEngineV;
    vector<thread> vecOfThreads;
    // if(BoxVV[i].size() == 1): should do something special!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for(size_t i = 0; i < boxVV.size(); i++){
        if(boxVV[i].size() > 1){
            cout << "Begin solve thread " << i << endl;
            SA* SAp = new SA(container, palletV, boxVV[i], totalVolumeV[i]);
            SAEngineV.push_back(SAp);
            
            thread th(&SA::solve, *SAp);
            vecOfThreads.push_back(move(th));
        }
    }

    for(thread & th : vecOfThreads)
    {
        // If thread Object is Joinable then Join that thread.
        if(th.joinable())
            th.join();
    }

    for(size_t i = 0; i < SAEngineV.size(); i++){
        cout << "Print result " << i << endl;
        SAEngineV[i]->printBoxInfo();
        SAEngineV[i]->evaluate();
        SAEngineV[i]->outputBoxResult();
    }

    // SA SAEngine(container, palletV, boxV, totalVolume);
    // SAEngine.solve();
    // SAEngine.printBoxInfo();
    // SAEngine.evaluate();

    return 0;
}