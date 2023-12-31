/**
 * @file DFAFactory.cc
 * @brief DFAFactory.hh的实现
 * @date 2023-8-8
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#include "DFAFactory.hh"

#include <queue>

using namespace chlex;

DFAFactory DFAFactory::instance;

void DFAFactory::closure(std::set<int> &stateSet, const NFA &nfa)
{
    std::queue<int> stateQueue;
    for (auto state : stateSet)
        stateQueue.push(state);

    while (!stateQueue.empty())
    {
        int state = stateQueue.front();
        stateQueue.pop();
        const auto &nfaState = nfa.getStates().at(state);
        for (auto &path : nfaState->paths)
        {
            if (path->byChar == 0)
            {
                if (stateSet.find(path->to.id) == stateSet.end())
                {
                    stateSet.insert(path->to.id);
                    stateQueue.push(path->to.id);
                }
            }
        }
    }
}

std::set<int> DFAFactory::move(const std::set<int> &stateSet, char byChar, const NFA &nfa)
{
    std::set<int> result;
    for (auto state : stateSet)
    {
        const auto &nfaState = nfa.getStates().at(state);
        for (auto &path : nfaState->paths)
        {
            if (path->byChar == byChar)
            {
                result.insert(path->to.id);
            }
        }
    }
    return result;
};

bool DFAFactory::isEqual(const std::set<int> &stateSet1, const std::set<int> &stateSet2)
{
    if (stateSet1.size() != stateSet2.size())
        return false;
    for (auto state : stateSet1)
    {
        if (stateSet2.find(state) == stateSet2.end())
            return false;
    }
    return true;
}

std::set<int> DFAFactory::checkEndStates(std::vector<std::shared_ptr<std::set<int>>> &stateSets, std::vector<std::shared_ptr<DFAState>> &dfaStates, const NFA &nfa)
{
    std::set<int> endStates;

    for (int i = 0; i < stateSets.size(); i++)
    {
        auto stateSet = stateSets[i];
        for (auto state : *stateSet)
        {
            if (nfa.getEndStates().find(state) != nfa.getEndStates().end())
            {
                endStates.insert(dfaStates[i]->id);
                auto dfaState = dfaStates[i];

                // 创建shared_ptr时传入一个假的deleter，用于允许其指向的内容向unique_ptr转移
                std::shared_ptr<DFAEndState> dfaEndState(new DFAEndState(), [](DFAEndState *p) {});
                dfaEndState->id = dfaState->id;
                dfaEndState->code = nfa.getEndStates().at(state).get().code;
                dfaStates[i] = dfaEndState;

                // 由于原来的dfaState也具有假的deleter，因此需要手动释放
                delete dfaState.get();
                break;
            }
        }
    }

    return endStates;
}

std::unique_ptr<DFA> DFAFactory::generate(const NFA &nfa)
{
    // 两个队列同步操作，实现DFA状态与状态集合的一一对应
    std::queue<std::shared_ptr<std::set<int>>> stateSetQueue;
    std::queue<std::shared_ptr<DFAState>> dfaStateQueue;

    std::vector<std::shared_ptr<std::set<int>>> stateSets;
    std::vector<std::shared_ptr<DFAState>> dfaStates;

    auto startStateSet = std::make_shared<std::set<int>>();
    startStateSet->insert(nfa.getStartState().id);
    closure(*startStateSet, nfa);

    // 创建shared_ptr时传入一个假的deleter，用于允许其指向的内容向unique_ptr转移
    std::shared_ptr<DFAState> startDFAState(new DFAState(), [](DFAState *p) {});
    startDFAState->id = 0;

    stateSetQueue.push(startStateSet);
    stateSets.push_back(startStateSet);
    dfaStateQueue.push(startDFAState);
    dfaStates.push_back(startDFAState);

    int nextId = 1;
    // 《编译原理》第97页的算法
    while (!stateSetQueue.empty())
    {
        auto stateSet = stateSetQueue.front();
        stateSetQueue.pop();
        auto dfaState = dfaStateQueue.front();
        dfaStateQueue.pop();

        for (char byChar = 1; byChar < 128; byChar++)
        {
            auto nextStateSet = std::make_shared<std::set<int>>(move(*stateSet, byChar, nfa));
            if (nextStateSet->empty())
                continue;
            closure(*nextStateSet, nfa);

            int isExistingStateSet = -1; // 用于查询是否已经存在相同的状态集合
                                         // 如果存在，该变量的值为其ID
                                         // 否则，该变量的值为-1
            for (int i = 0; i < stateSets.size(); i++)
            {
                if (isEqual(*stateSet, *nextStateSet))
                {
                    isExistingStateSet = dfaStates[i]->id;
                    break;
                }
            }

            if (isExistingStateSet == -1)
            {
                // 创建shared_ptr时传入一个假的deleter，用于允许其指向的内容向unique_ptr转移
                std::shared_ptr<DFAState> nextDFAState(new DFAState(), [](DFAState *p) {});
                nextDFAState->id = nextId++;
                dfaState->paths[byChar] = nextDFAState->id;

                stateSetQueue.push(nextStateSet);
                dfaStateQueue.push(nextDFAState);

                stateSets.push_back(nextStateSet);
                dfaStates.push_back(nextDFAState);
            }
            else
                dfaState->paths[byChar] = isExistingStateSet;
        }
    }

    auto endStates = checkEndStates(stateSets, dfaStates, nfa);

    auto dfa = std::make_unique<DFA>(*dfaStates[0]);

    for (int i = 0; i < dfaStates.size(); i++)
    {
        std::unique_ptr<DFAState> dfaState(dfaStates[i].get());
        dfa->getStates().insert({dfaState->id, std::move(dfaState)});
        if (endStates.find(dfaState->id) != endStates.end())
            dfa->getEndStates().insert({dfaState->id, (DFAEndState &)*dfaState});
    }

    return dfa;
}

std::unique_ptr<DFAChlex> DFAFactory::generate(std::shared_ptr<NFAChlex> nfaChlex)
{
    auto dfaChlex = std::make_unique<DFAChlex>();
    auto dfa = generate(nfaChlex->getNFA());
    dfaChlex->dfa = std::move(dfa);
    dfaChlex->nfaChlex = nfaChlex;
    return dfaChlex;
}