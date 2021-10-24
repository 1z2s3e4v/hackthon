#include "Pallet.h"
#include "Box.h"
#include "SA.h"
#include "Include.h"

// (v_container, v_box)
pair<vector<vector<size_t> >, vector<Box*> > parseInput(string filename){
    vector<vector<size_t> > v_container;
    vector<Box*> v_Box;
    ifstream fin(filename);
    string line;
    int count_box = 0;
    while(getline(fin,line)){
        stringstream ss(line);
        string head;
        ss >> head;
        if(head == "pallet"){
            vector<size_t> container(3);
            string s_d,s_w,s_h;
            ss >> s_d >> s_h >> s_w;
            container[0] = stoi(s_d); // 140
            container[1] = stoi(s_w); // 102
            container[2] = 225 - stoi(s_h); // 225-12 = 213
            v_container.push_back(container);
        }
        if(head == "box"){
            string s_typeId,s_d,s_h,s_w,s_kg;
            ss >> s_typeId >> s_d >> s_h >> s_w >> s_kg;
            Box* BoxP = new Box(to_string(count_box), count_box, stoi(s_typeId), stoi(s_w), stoi(s_h), stoi(s_d), stoi(s_kg));
            v_Box.push_back(BoxP);
            count_box++;
        }
    }
    return make_pair(v_container, v_Box);
}

int main(int argc, char *argv[])
{
    vector<vector<size_t> > v_container;
    vector<Box*> v_Box;
    vector<Pallet*> palletV; // maybe can remove
    vector<vector<Box*>> boxVV(1);
    vector<size_t> totalVolumeV(1);
    vector<Box*> unFitBoxes;
    size_t n1;
    bool use_rand = false;

    // input parser
    pair<vector<vector<size_t> >, vector<Box*> > data_pair;
    if(argc < 2){
        cout << "Use random case.\n";
        use_rand = true;
        n1 = 20;
    }
    else{
        data_pair = parseInput(argv[1]);
        v_container = data_pair.first;
        v_Box = data_pair.second;
        n1 = v_Box.size();
    }

    // macro
    size_t cntPallet = 0;
    vector<Box*> tempBoxV;
    // Insert box if is possible
    for(size_t i = 0; i < n1; i++){
        vector<size_t> container;
        Box* BoxP;
        if(use_rand){
            container.resize(3);
            container[0] = 140;
            container[1] = 102;
            container[2] = 220;
            size_t deepth = rand()%(int)floor(container[0]*0.3)+1;
            size_t width = rand()%(int)floor(container[1]*0.3)+1;
            size_t height = rand()%(int)floor(container[2]*0.3)+1;
            size_t weight = rand()%1000+1;
            BoxP = new Box(to_string(i), i, i, width, height, deepth, weight);
        }
        else{
            container = v_container[cntPallet];
            BoxP = v_Box[i];
        }
        BoxP->_container_id = cntPallet;
        size_t volume = BoxP->getVolume();
        totalVolumeV[cntPallet] += volume;

        if(totalVolumeV[cntPallet] >= 0.50*container[0]*container[1]*container[2]){
            totalVolumeV[cntPallet] -= volume;
            cout << "Utilization rate: " << totalVolumeV[cntPallet]*1.0/container[0]/container[1]/container[2] << endl;
            cntPallet++;
            // cout << "Create pallet " << cntPallet << endl;

            // cout << i << endl;
            boxVV.push_back({}); // add one pallet

            totalVolumeV.push_back(volume);
        }
        boxVV[cntPallet].push_back(BoxP);
    }

    vector<SA*> SAEngineV;
    vector<thread> vecOfThreads;
    // if(BoxVV[i].size() == 1): should do something special!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for(size_t i = 0; i < boxVV.size(); i++){
        if(boxVV[i].size() > 1) {
            cout << "Begin solve thread " << i << endl;
            SA* SAp;
            if(use_rand){
                vector<size_t> container(3);
                container[0] = 140;
                container[1] = 102;
                container[2] = 220;
                SAp = new SA(container, palletV, boxVV[i], totalVolumeV[i]);
            }
            else{
                SAp = new SA(v_container[i], palletV, boxVV[i], totalVolumeV[i]);
            }
            SAEngineV.push_back(SAp);
            
            thread th(&SA::solve, SAp);
            vecOfThreads.push_back(move(th));
        }
        else
            unFitBoxes.emplace_back(move(boxVV[i][0]));
    }

    for(thread & th : vecOfThreads)
    {
        // If thread Object is Joinable then Join that thread.
        if(th.joinable())
            th.join();
    }

    system("rm -f SA_box_result.txt");
    for(size_t i = 0; i < SAEngineV.size(); i++) {
        cout << "Print result " << i << endl;
        SAEngineV[i]->printBoxInfo();
        SAEngineV[i]->evaluate(unFitBoxes);
        SAEngineV[i]->outputBoxResult();
    }

    cout << "unFitBox size: " << unFitBoxes.size() << '\n';
    for (auto& box : unFitBoxes) {
        cout << box->_name << ": (" << box->_deepth << ", " << box->_width << ", " << box->_height << ")\n";
    }

    // SA SAEngine(container, palletV, boxV, totalVolume);
    // SAEngine.solve();
    // SAEngine.printBoxInfo();
    // SAEngine.evaluate();

    return 0;
}