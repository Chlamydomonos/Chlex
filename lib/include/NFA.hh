/**
 * @file NFA.hh
 * @brief 有关NFA的各个类的声明
 * @date 2023-8-5
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "chlex_base.hh"

#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>

CHLEX_NAMESPACE_BEGIN

struct NFAPath;

/**
 * @brief NFA状态类
 * @details 用于表示NFA中的状态
 */
struct NFAState
{
    unsigned int id;                             ///< 状态的id，也用作它的名称
    std::vector<std::unique_ptr<NFAPath>> paths; ///< 从该状态出发的路径
};

/**
 * @brief NFA终止状态类
 * @details 用于表示NFA中的终止状态
 */
struct NFAEndState : public NFAState
{
    std::string code; ///< 终止在该状态后执行的代码
};

/**
 * @brief NFA路径类
 * @details 用于表示NFA中的路径
 */
struct NFAPath
{
    const NFAState &from; ///< 路径的起点
    const NFAState &to;   ///< 路径的终点
    char byChar;          ///< 路径上的字符
};

/**
 * @brief NFA类
 * @details 用于表示NFA
 */
class NFA
{
private:
    std::map<int, std::unique_ptr<NFAState>> states;              ///< NFA中的所有状态
    const NFAState &startState;                                   ///< NFA的起始状态
    std::map<int, std::reference_wrapper<NFAEndState>> endStates; ///< NFA的终止状态

public:
    /**
     * @brief 构造函数
     * @param startState 起始状态
     */
    NFA(const NFAState &startState) : startState(const_cast<NFAState &>(startState)) {}
    ~NFA() = default; ///< 默认析构函数

    /**
     * @brief 获取NFA中的所有状态
     * @return NFA中的所有状态
     */
    std::map<int, std::unique_ptr<NFAState>> &getStates() { return states; }

    /**
     * @brief 获取NFA中的所有状态
     * @return NFA中的所有状态
     */
    const std::map<int, std::unique_ptr<NFAState>> &getStates() const { return states; }

    /**
     * @brief 获取NFA的起始状态
     * @return NFA的起始状态
     */
    const NFAState &getStartState() const { return startState; }

    /**
     * @brief 获取NFA的终止状态
     * @return NFA的终止状态
     */
    std::map<int, std::reference_wrapper<NFAEndState>> &getEndStates() { return endStates; }

    /**
     * @brief 获取NFA的终止状态
     * @return NFA的终止状态
     */
    const std::map<int, std::reference_wrapper<NFAEndState>> &getEndStates() const { return endStates; }
};

CHLEX_NAMESPACE_END