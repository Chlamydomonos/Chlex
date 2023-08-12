/**
 * @file DFAMinimizer.hh
 * @brief 有关DFA最小化的各个类的声明
 * @date 2023-8-10
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "Chlex.hh"

CHLEX_NAMESPACE_BEGIN

/**
 * @brief 状态在组中的信息
 * @details 用于最小化过程中的临时信息
 */
struct StateInGroup
{
    int stateId;
    int groupId;
};

/**
 * @brief 状态组
 * @details 用于最小化过程中的临时信息
 */
struct StateGroup
{
    int groupId;
    std::vector<int> states;
};

/**
 * @brief 状态组的移动信息
 * @details 用于最小化过程中的临时信息
 */
using MoveInfo = std::map<char, int>;

/**
 * @brief DFA最小化类
 * @details 用于将DFA最小化，是一个单例类
 */
class DFAMinimizer
{
private:
    static DFAMinimizer instance; ///< 单例对象

    /**
     * @brief 判断两个状态的移动信息是否相同
     * @param a 状态1的移动信息
     * @param b 状态2的移动信息
     * @return 是否相同
     */
    bool isSame(const MoveInfo &a, const MoveInfo &b);

    /**
     * @brief 处理最小化DFA的终态
     * @details 检查每个状态组，如果该组中的状态有终态，则将该组对应的状态设置为终态
     * @param groupSet 状态组集合
     * @param newStates 最小化后的DFA中的状态
     * @param dfa 最小化前的DFA
     * @return 最小化后的DFA中的终态ID
     */
    std::set<int> handleEndStates(std::set<std::shared_ptr<StateGroup>> &groupSet, std::vector<std::unique_ptr<DFAState>> &newStates, const DFA &dfa);

public:
    static DFAMinimizer &getInstance() { return instance; } ///< 获取单例对象

    /**
     * @brief 最小化DFA
     * @param dfa 要最小化的DFA
     * @return 最小化后的DFA
     */
    std::unique_ptr<DFA> minimize(const DFA &dfa);

    /**
     * @brief 最小化DFAChlex对象
     * @param dfaChlex 要最小化的DFAChlex
     * @return 最小化后的DFAChlex
     */
    std::unique_ptr<MinimizedDFAChlex> minimize(std::shared_ptr<DFAChlex> dfaChlex);
};

CHLEX_NAMESPACE_END