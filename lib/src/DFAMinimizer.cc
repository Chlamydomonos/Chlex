/**
 * @file DFAMinimizer.cc
 * @brief DFAMinimizer.hh的实现
 * @date 2023-8-8
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#include "DFAMinimizer.hh"

#include <queue>

using namespace chlex;

DFAMinimizer DFAMinimizer::instance;

bool DFAMinimizer::isSame(const MoveInfo &a, const MoveInfo &b)
{
    if (a.size() != b.size())
        return false;
    for (auto &i : a)
    {
        auto &byChar = i.first;
        auto &to = i.second;
        if (b.find(byChar) == b.end())
            return false;
        if (b.at(byChar) != to)
            return false;
    }
    return true;
}

std::set<int> DFAMinimizer::handleEndStates(std::set<std::shared_ptr<StateGroup>> &groupSet, std::vector<std::unique_ptr<DFAState>> &newStates, const DFA &dfa)
{
    std::set<int> endStates;
    for (auto &group : groupSet)
    {
        for (auto &stateId : group->states)
        {
            auto &state = dfa.getStates().at(stateId);
            if (dfa.getEndStates().find(state->id) != dfa.getEndStates().end())
            {
                endStates.insert(group->groupId);
                auto endState = std::make_unique<DFAEndState>();
                endState->id = group->groupId;
                for (auto &path : state->paths)
                {
                    auto &byChar = path.first;
                    auto &to = path.second;
                    endState->paths.insert({byChar, to});
                }
                endState->code = static_cast<DFAEndState &>(*state).code;
                newStates[group->groupId] = std::move(endState);
                break;
            }
        }
    }
    return endStates;
}

std::unique_ptr<DFA> DFAMinimizer::minimize(const DFA &dfa)
{
    // 保存每个状态在哪个组中
    std::vector<StateInGroup> statesInGroup(dfa.getStates().size());

    // 初始情况，将所有终态放入组1，其余放入组0
    auto group0 = std::make_shared<StateGroup>();
    auto group1 = std::make_shared<StateGroup>();
    for (auto &i : dfa.getStates())
    {
        auto stateId = i.first;
        auto &state = i.second;
        statesInGroup[stateId].stateId = stateId;

        if (dfa.getEndStates().find(state->id) != dfa.getEndStates().end())
        {
            statesInGroup[stateId].groupId = 1;
            group1->states.push_back(stateId);
        }
        else
        {
            statesInGroup[stateId].groupId = 0;
            group0->states.push_back(stateId);
        }
    }

    std::queue<std::shared_ptr<StateGroup>> groupQueue;
    std::set<std::shared_ptr<StateGroup>> groupSet;

    groupQueue.push(group0);
    groupQueue.push(group1);

    // 循环处理每个组，尝试将其划分
    int nextGroupId = 2;
    while (!groupQueue.empty())
    {
        auto group = groupQueue.front();
        groupQueue.pop();

        if (group->states.size() == 1)
        {
            groupSet.insert(group);
            break;
        }

        // 记录每个状态对于每个字符移动到哪个组
        std::map<int, MoveInfo> groupMoveInfo;
        for (auto &state : group->states)
        {
            auto &dfaState = *(dfa.getStates().at(state));
            MoveInfo moveInfo;
            for (auto &path : dfaState.paths)
            {
                auto &byChar = path.first;
                auto &to = path.second;

                moveInfo[byChar] = statesInGroup[to].groupId;
            }
        }

        // 把移动到同一组的状态放到一起
        std::vector<std::vector<int>> newGroups;
        newGroups.push_back(std::vector<int>(group->states[0]));

        for (int i = 1; i < group->states.size(); i++)
        {
            auto stateId = group->states[i];

            bool inCurrentGroups = false;
            for (int j = 0; j < newGroups.size(); j++)
            {
                auto &newGroup = newGroups[j];
                auto &firstStateId = newGroup[0];
                if (isSame(groupMoveInfo[stateId], groupMoveInfo[firstStateId]))
                {
                    newGroup.push_back(stateId);
                    inCurrentGroups = true;
                    break;
                }
            }
            if (!inCurrentGroups)
                newGroups.push_back(std::vector<int>(stateId));
        }

        if (newGroups.size() == 1)
        {
            groupSet.insert(group);
            continue;
        }

        for (int i = 0; i < newGroups.size(); i++)
        {
            auto &newGroup = newGroups[i];
            auto newGroupPtr = std::make_shared<StateGroup>();
            newGroupPtr->groupId = nextGroupId++;
            for (auto &stateId : newGroup)
            {
                statesInGroup[stateId].groupId = newGroupPtr->groupId;
                newGroupPtr->states.push_back(stateId);
            }
            groupQueue.push(newGroupPtr);
        }
    }

    // 找到初态所在的组
    auto startGroupId = statesInGroup[dfa.getStartState().id].groupId;
    std::unique_ptr<DFAState> newStartState = std::make_unique<DFAState>();
    newStartState->id = startGroupId;

    // 生成新的DFA
    std::vector<std::unique_ptr<DFAState>> newStates;
    newStates.push_back(std::move(newStartState));

    for (auto path : dfa.getStartState().paths)
    {
        auto &byChar = path.first;
        auto &to = path.second;

        auto toGroup = statesInGroup[to].groupId;
        newStartState->paths[byChar] = toGroup;
    }

    for (auto &group : groupSet)
    {
        if (group->groupId == startGroupId)
            continue;

        auto newState = std::make_unique<DFAState>();
        newState->id = group->groupId;

        auto &groupState = dfa.getStates().at(group->states[0]);

        for (auto path : groupState->paths)
        {
            auto &byChar = path.first;
            auto &to = path.second;

            auto toGroup = statesInGroup[to].groupId;
            newState->paths[byChar] = toGroup;
        }

        newStates.push_back(std::move(newState));
    }

    auto endStates = handleEndStates(groupSet, newStates, dfa);

    auto newDFA = std::make_unique<DFA>(*newStates[0]);

    for (int i = 0; i < newStates.size(); i++)
    {
        auto state = std::move(newStates[i]);

        if (endStates.find(state->id) != endStates.end())
            newDFA->getEndStates().insert({state->id, static_cast<DFAEndState &>(*state)});

        newDFA->getStates().insert({state->id, std::move(state)});
    }

    return newDFA;
}

std::unique_ptr<MinimizedDFAChlex> DFAMinimizer::minimize(std::shared_ptr<DFAChlex> dfaChlex)
{
    auto minimizedDFAChlex = std::make_unique<MinimizedDFAChlex>();
    minimizedDFAChlex->dfaChlex = dfaChlex;
    minimizedDFAChlex->minimizedDFA = minimize(*dfaChlex->dfa);
    return minimizedDFAChlex;
}