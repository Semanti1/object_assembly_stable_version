#include "causal_furniture.h"
#include "beliefstate.h"
#include "utils.h"
#include <math.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
using namespace UTILS;


CAUSAL_FURNITURE::CAUSAL_FURNITURE(std::string name, int numparts, bool causal, std::vector<std::pair<std::string, std::string> > allpairs, double conf)
    : furniture_name(name),
      numParts(numparts),
      causal(causal),
      allpairs(allpairs),
      conf(conf)

{
    NumActions = numparts * (numparts - 1) / 2;
    NumObservations = 2;
    //RewardRange = 100;// NumActions / 4.0;
    //Discount = 0.95;
  // names = { "kerosenetank","burner", "chimney", "wick", "lighter" };
  //  names = { "batteries", "switchandcircuit", "lens", "lightbulb"};
  names = { "cord","outlet", "base", "lightbulb", "pullchain", "shade"};
  // names = { "backplate","lampbody","cord","lightbulb" };
  //  names = { "frame","grip","triggerguard","barrel","hammer","sights","trigger","magazine","firingpin","cartridge","slide" };
   // names = { "watersupply","taps","faucet","faucetaerator","basin", "shutoff", "tailpiece", "drain","sinktrap" };
    cout << "numparts" << numParts << Discount <<endl;
    cout << "all pair size" << allpairs.size() << endl;
    
}

STATE* CAUSAL_FURNITURE::Copy(const STATE& state) const
{
    assert(state.IsAllocated());
    const CAUSAL_FURNITURE_STATE& oldstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    CAUSAL_FURNITURE_STATE* newstate = MemoryPool.Allocate();
    *newstate = oldstate;
    return newstate;
}

void CAUSAL_FURNITURE::Validate(const STATE& state) const
{
    /*const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    for (int i = 0; i < XSize * YSize; ++i)
    {
        if (bsstate.Cells(i).Diagonal && bsstate.Cells(i).Occupied)
        {
            DisplayState(bsstate, cout);
            assert(false);
        }
    }*/
}
STATE* CAUSAL_FURNITURE::CreateStartStateReal(int real) const
{
    CAUSAL_FURNITURE_STATE* cfstate = MemoryPool.Allocate();
    cfstate->numleft = 0;
    cfstate->unassembled.clear();
    for (int i = 0; i < NumActions; i++)
    {
        OBJECT_PARTS obj;
        obj.num = i;
        int n = numParts;
        int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        std::string obj1name = names[x_ind];
        std::string obj2name = names[y_ind];
        std::pair<std::string, std::string> connection(obj1name, obj2name);
        if (isValid(connection))
        {
            obj.legit = 1;
        }
        else
        {
            obj.legit = 0;
        }
        //obj.legit = Random(2);
        if (obj.legit)
            cfstate->numleft++;
        obj.visited = false;
        cfstate->unassembled.push_back(obj);
    }

    return cfstate;
}

STATE* CAUSAL_FURNITURE::CreateStartState() const
{
    CAUSAL_FURNITURE_STATE* cfstate = MemoryPool.Allocate();
  
    cfstate->unassembled.clear();
    cfstate->numleft = 0;
    if (causal)
    {
       // cout << "CAUSAL";
        
            for (int i = 0; i < NumActions; i++)
            {
                OBJECT_PARTS obj;
                obj.num = i;
                int n = numParts;
                int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
                int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
                std::string obj1name = names[x_ind];
                std::string obj2name = names[y_ind];
                std::pair<std::string, std::string> connection(obj1name, obj2name);
                if (isCausal(connection))
                {
                    obj.legit = Bernoulli(conf);
                }
                else {
                    obj.legit = Bernoulli(1-conf);
                }
                if (obj.legit)
                    cfstate->numleft++;
                obj.visited = false;
                cfstate->unassembled.push_back(obj);
            }
        
        
        return cfstate;
    }
    else
    {
        //cout << " NOT in causal" << endl;
        for (int i = 0; i < NumActions; i++)
        {
            OBJECT_PARTS obj;
            obj.num = i;
            
            obj.legit = Random(2);
            
            if (obj.legit)
                cfstate->numleft++;
            obj.visited = false;
            cfstate->unassembled.push_back(obj);
        }
        return cfstate;
    }
}



void CAUSAL_FURNITURE::FreeState(STATE* state) const
{
    CAUSAL_FURNITURE_STATE* cfstate = safe_cast<CAUSAL_FURNITURE_STATE*>(state);
    MemoryPool.Free(cfstate);
}

bool CAUSAL_FURNITURE::Step(STATE& state, int action,
    int& observation, double& reward) const
{
    CAUSAL_FURNITURE_STATE& cfstate = safe_cast<CAUSAL_FURNITURE_STATE&>(state);

    
OBJECT_PARTS& obj = cfstate.unassembled[action];
if (obj.visited)
{
    reward = -10;
    observation = 0;

}
else
{
    if (obj.legit)
    {
        reward = -1;
        //reward = 10;
        observation = 1;
        cfstate.numleft = cfstate.numleft - 1;
    }
    else
    {
        reward = -1;
        observation = 0;
    }
    obj.visited = true;
}
if (cfstate.numleft == 0)
{
    reward += NumActions;
    //cout << "num left " << cfstate.numleft << endl;
    return true;
}
else
{
    return false;
}
}
bool CAUSAL_FURNITURE::isValid(std::pair<std::string, std::string> connection) const
{
   std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("backplate","lampbody"), std::make_pair("lampbody","lightbulb"), std::make_pair("cord","lightbulb") };
//   std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("kerosenetank", "burner"), std::make_pair("burner","wick"), std::make_pair("lighter","wick"),std::make_pair("chimney","burner") };
 // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("batteries", "switchandcircuit"),std::make_pair("batteries", "lightbulb"), std::make_pair("switchandcircuit","lightbulb"), std::make_pair("lens","lightbulb") };
//    std::vector<std::pair<std::string, std::string> > validconnections{ std::make_pair("outlet", "cord"),std::make_pair("cord", "lightbulb"), std::make_pair("base","lightbulb"), std::make_pair("pullchain","lightbulb"), std::make_pair("shade","lightbulb") };
   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("firingpin","slide"), std::make_pair("sights","slide"),std::make_pair("barrel","slide"),std::make_pair("trigger","hammer"),std::make_pair("hammer","slide"),std::make_pair("slide","frame"),std::make_pair("magazine","frame"),std::make_pair("trigger","frame"),std::make_pair("triggerguard","frame"),std::make_pair("grip","frame"),std::make_pair("cartridge","magazine") };
   // std::vector<std::pair<std::string, std::string>> validconnections{ std::make_pair("shutoff","watersupply"),std::make_pair("watersupply","faucet"), std::make_pair("taps","faucet"), std::make_pair("faucetaerator","faucet"), std::make_pair("sinktrap","tailpiece"), std::make_pair("tailpiece", "drain"), std::make_pair("drain", "basin") };
    auto it3 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
    auto it4 = std::find_if(validconnections.begin(), validconnections.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });
    if ((validconnections.end() == it3) && (validconnections.end() == it4))
        return false;
    else
        return true;
}

bool CAUSAL_FURNITURE::isCausal(std::pair<std::string, std::string> connection) const
{
    

    auto it3 = std::find_if(allpairs.begin(), allpairs.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.first) && (obj.second == connection.second)); });
    auto it4 = std::find_if(allpairs.begin(), allpairs.end(), [&](std::pair<std::string, std::string> obj) { return ((obj.first == connection.second) && (obj.second == connection.first)); });

    if ((allpairs.end() == it3) && (allpairs.end() == it4))
        return false;
    else
        return true;
}
bool CAUSAL_FURNITURE::LocalMove(STATE& state, const HISTORY& history,
    int stepObs, const STATUS& status) const
{
    CAUSAL_FURNITURE_STATE& cfstate = safe_cast<CAUSAL_FURNITURE_STATE&>(state);
    while (1)
    {
        int action = Random(NumActions);
        OBJECT_PARTS& obj = cfstate.unassembled[action];
        if (obj.visited)
        {
            //cout << "in visited, continuing" << endl;
            continue;
        }
        else
        {
            /*int chance;
            int n = numParts;
            int x_ind = n - 2 - floor(sqrt(-8 * action + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
            int y_ind = action + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
            std::string obj1name = names[x_ind];
            std::string obj2name = names[y_ind];
            std::pair<std::string, std::string> connection(obj1name, obj2name);*/
            /*if (isCausal(connection))
                chance = Bernoulli(0.75);
            else
                chance = Bernoulli(0.25);

            if (obj.legit == chance)
                continue;*/

            
            obj.legit = !obj.legit;
            if (obj.legit)
                cfstate.numleft++;
            else
                cfstate.numleft--;
            //return true;

            //cout << "history" << history.Size() << endl;
            for (int t = 0; t < history.Size(); ++t)

            {
                // Ensure that ships are consistent with observation history
                int a = history[t].Action;
                const OBJECT_PARTS& obj2 = cfstate.unassembled[a];

                assert(obj2.visited);

                if (obj2.legit != history[t].Observation)
                {
                    if (int(t == history.Size() - 1)==0)
                        cout << "return false: " << int(t==history.Size()-1) << endl;
                    return false;
                }
            }
            //cout << "return true" << endl;
            return true;
        }


    }
    return false;
    
   
}


void CAUSAL_FURNITURE::GenerateLegal(const STATE& state, const HISTORY& history,
    vector<int>& legal, const STATUS& status) const
{
    const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    for (int a = 0; a < NumActions; ++a)
    {
        if (!cfstate.unassembled[a].visited)
        {
            legal.push_back(a);
        }
    }
    //bool diagonals = Knowledge.Level(status.Phase) == KNOWLEDGE::SMART;
    /*if (diagonals)
    {
        for (int a = 0; a < NumActions; ++a)
            if (!bsstate.Cells(a).Visited && !bsstate.Cells(a).Diagonal)
                legal.push_back(a);
    }
    else
    {*/
    /*for (int a = 0; a < NumActions; ++a)
    {
        // reference: https://stackoverflow.com/questions/27086195/linear-index-upper-triangular-matrix
        int n = numParts;
        int x_ind = n - 2 - floor(sqrt(-8 * a + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        int y_ind = a + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        std::string obj1name = names[x_ind];
        std::string obj2name = names[y_ind];

        auto it = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return (obj.name == obj1name); });
        auto it2 = std::find_if(cfstate.unassembled.begin(), cfstate.unassembled.end(), [&](OBJECT_PARTS obj) { return obj2name == obj.name; });
        if ((cfstate.unassembled.end() != it) && (cfstate.unassembled.end() != it2))
        {
            if ((*it).ports_remaining > 0 && (*it2).ports_remaining > 0)
            {
                legal.push_back(a);
            }
            //cout << "legal" << a ;
        }
        //cout << endl;

    }
            //if (!bsstate.Cells(a).Visited)
                
    //}*/
}



void CAUSAL_FURNITURE::DisplayState(const STATE& state, ostream& ostr) const
{
    const CAUSAL_FURNITURE_STATE& cfstate = safe_cast<const CAUSAL_FURNITURE_STATE&>(state);
    
    std::string outstate[numParts+1][numParts+1];
    for (int x = 0; x < numParts+1; x++)
    {
        for (int y = 0; y < numParts+1; y++)
        {
            if ((x == 0) && (y == 0))
            {
                outstate[x][y] = "|";
            }
            else if (x == 0)
                outstate[x][y] = names[y - 1];
            else if (y == 0)
                outstate[x][y] = names[x - 1];
            else
                outstate[x][y] = ".";



        }
    }
    ostr << endl << "  ";
    for (int i = 0; i < NumActions; i++)
    {
        const OBJECT_PARTS& obj = cfstate.unassembled[i];
        
        int n = numParts;
        int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
        int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
        if (obj.visited && obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "V + L";
        else if (obj.visited && !obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "V + NL";
        else if (!obj.visited && obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "NV + L";
        else if (!obj.visited && !obj.legit)
            outstate[x_ind + 1][y_ind + 1] = "NV + NL";

        
    }

    for (int x = 0; x < numParts + 1; x++)
    {
        for (int y = 0; y < numParts + 1; y++)
        {
            ostr << setw(10) << outstate[x][y] << ' ';
        }
        ostr << endl;
    }
    
    /*cout << "unassembled" << endl;
    for (int i = 0; i < cfstate.unassembled.size(); i++)
    {
        
        int num = cfstate.unassembled[i].num;
        DisplayAction(num, cout);
        cout << "legit " << cfstate.unassembled[i].legit << ", visited" <<  cfstate.unassembled[i].visited << endl;
    }*/
}
/*
void BATTLESHIP::DisplayObservation(const STATE& state, int observation, ostream& ostr) const
{
    if (observation)
        cout << "Hit\n";
    else
        cout << "Miss\n";
}

void BATTLESHIP::DisplayAction(int action, ostream& ostr) const
{
    COORD actionPos = COORD(action % XSize, action / XSize);

    ostr << endl << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
    for (int y = YSize - 1; y >= 0; y--)
    {
        ostr << setw(1) << y << ' ';
        for (int x = 0; x < XSize; x++)
        {
            char c = ' ';
            if (actionPos == COORD(x, y))
                c = '@';
            ostr << c << ' ';
        }
        ostr << setw(1) << y << endl;
    }
    ostr << "  ";
    for (int x = 0; x < XSize; x++)
        ostr << setw(1) << x << ' ';
    ostr << "  " << endl;
}
*/
void CAUSAL_FURNITURE::DisplayBeliefs(const BELIEF_STATE& beliefState,
    ostream& ostr) const
{
    GRID<int> counts(numParts, numParts);
    counts.SetAllValues(0);

    for (int j = 0; j < beliefState.GetNumSamples(); j++)
    {
        const CAUSAL_FURNITURE_STATE* cfstate =
            safe_cast<const CAUSAL_FURNITURE_STATE*>(
                beliefState.GetSample(j));
        for (int i = 0; i < NumActions; i++)
        {
            const OBJECT_PARTS& obj = cfstate->unassembled[i];

            int n = numParts;
            int x_ind = n - 2 - floor(sqrt(-8 * i + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
            int y_ind = i + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;

            counts(x_ind, y_ind) += obj.legit;
        }
    }


    for (int x = 0; x < numParts + 1; x++)
    {
        ostr << endl;
        for (int y = 0; y < numParts + 1; y++)
        {
            if ((x == 0) && (y == 0))
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << "|";
            }
            else if (x == 0)
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << names[y - 1] << " ";
            }

            else if (y == 0)
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << names[x - 1] << " ";
            }
            else
            {
                ostr.width(10);
                ostr.precision(2);
                ostr << fixed << (double)counts(x - 1, y - 1) / beliefState.GetNumSamples() << " ";
            }



        }
        ostr << endl;
    }
}
void CAUSAL_FURNITURE::DisplayAction(int action, ostream& ostr) const
{


    int n = numParts;
    int x_ind = n - 2 - floor(sqrt(-8 * action + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
    int y_ind = action + x_ind + 1 - n * (n - 1) / 2 + (n - x_ind) * ((n - x_ind) - 1) / 2;
    std::string obj1name = names[x_ind];
    std::string obj2name = names[y_ind];
    ostr << "connect " << obj1name << " to " << obj2name << endl;
}