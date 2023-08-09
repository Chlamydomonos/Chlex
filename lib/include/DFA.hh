/**
 * @file DFA.hh
 * @brief 有关DFA的各个类的声明
 * @date 2023-8-8
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "chlex_base.hh"

#include <set>
#include <map>
#include <string>
#include <memory>

CHLEX_NAMESPACE_BEGIN

/**
 * @brief DFA状态类
 * @details 用于表示DFA中的状态
 */
struct DFAState
{
    unsigned int id;           ///< 状态的id，也用作它的名称
    std::map<char, int> paths; ///< 从该状态出发的路径
};

/**
 * @brief DFA终止状态类
 * @details 用于表示DFA中的终止状态
 */
struct DFAEndState : public DFAState
{
    std::string code; ///< 终止在该状态后执行的代码
};

/**
 * @brief DFA类
 * @details 用于表示DFA
 */
class DFA
{
private:
    std::map<int, std::unique_ptr<DFAState>> states;         ///< DFA中的所有状态
    const DFAState &startState;                              ///< DFA的起始状态
    std::set<std::reference_wrapper<DFAEndState>> endStates; ///< DFA的终止状态

public:
    /**
     * @brief 构造函数
     * @param startState 起始状态
     */
    DFA(const DFAState &startState) : startState(startState) {}

    /**
     * @brief 获取DFA中的所有状态
     * @return DFA中的所有状态
     */
    std::map<int, std::unique_ptr<DFAState>> &getStates() { return states; }

    /**
     * @brief 获取DFA中的所有状态
     * @return DFA中的所有状态
     */
    const std::map<int, std::unique_ptr<DFAState>> &getStates() const { return states; }

    /**
     * @brief 获取DFA的起始状态
     * @return DFA的起始状态
     */
    const DFAState &getStartState() const { return startState; }

    /**
     * @brief 获取DFA的终止状态
     * @return DFA的终止状态
     */
    std::set<std::reference_wrapper<DFAEndState>> &getEndStates() { return endStates; }

    /**
     * @brief 获取DFA的终止状态
     * @return DFA的终止状态
     */
    const std::set<std::reference_wrapper<DFAEndState>> &getEndStates() const { return endStates; }
};

CHLEX_NAMESPACE_END