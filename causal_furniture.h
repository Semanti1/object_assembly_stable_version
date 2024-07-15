#ifndef CAUSAL_FURNITURE_H
#define CAUSAL_FURNITURE_H

#include "simulator.h"
#include "grid.h"
#include <list>
#include <string>
#include <vector>
#include <utility>

struct OBJECT_PARTS
{
    int num;
    bool legit;
    bool visited;
};

class CAUSAL_FURNITURE_STATE : public STATE
{
public:
    std::vector<OBJECT_PARTS> unassembled;
    int numleft;
};

class CAUSAL_FURNITURE : public SIMULATOR
{
public:
    CAUSAL_FURNITURE(std::string name = "kerosene_lamp", int numparts = 6, bool causal = true, std::vector<std::pair<std::string, std::string> > allpairs = {}, double conf = 0.75);

    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual STATE* CreateStartStateReal(int real) const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action, int& observation, double& reward) const;
    void GenerateLegal(const STATE& state, const HISTORY& history, std::vector<int>& legal, const STATUS& status) const;
    virtual bool LocalMove(STATE& state, const HISTORY& history, int stepObs, const STATUS& status) const;
    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState, std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    std::vector<std::pair<std::string, std::string> > allpairs;

private:
    bool isValid(std::pair<std::string, std::string> connection) const;
    bool isCausal(std::pair<std::string, std::string> connection) const;

    std::string furniture_name;
    int numParts;
    bool causal;
    double conf;
    std::vector<std::string> names;
    mutable MEMORY_POOL<CAUSAL_FURNITURE_STATE> MemoryPool;
};

#endif // BATTLESHIP_H
