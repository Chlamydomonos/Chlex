/**
 * @file DFAFactory.hh
 * @brief 有关DFA工厂的各个类的声明
 * @date 2023-8-8
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "Chlex.hh"

CHLEX_NAMESPACE_BEGIN

/**
 * @brief DFA工厂类
 * @details 用于通过NFA生成DFA，是一个单例类
 */
class DFAFactory
{
private:
    static DFAFactory instance; ///< 单例对象

    /**
     * @brief 生成状态集合的epsilon闭包
     * @param stateSet 状态集合
     * @param nfa 状态集合所在的NFA
     * @note 会修改stateSet
     */
    void closure(std::set<int> &stateSet, const NFA &nfa);

    /**
     * @brief 生成状态集合的move
     * @param stateSet 状态集合
     * @param byChar 字符
     * @param nfa 状态集合所在的NFA
     * @return move后的状态集合
     */
    std::set<int> move(const std::set<int> &stateSet, char byChar, const NFA &nfa);

    /**
     * @brief 判断两个状态集合是否相等
     * @param stateSet1 状态集合1
     * @param stateSet2 状态集合2
     * @return 是否相等
     */
    bool isEqual(const std::set<int> &stateSet1, const std::set<int> &stateSet2);

    /**
     * @brief 检查终止状态集合
     * @details 此函数检查每个状态集合，如果其中有一个状态是NFA中的终止状态，则将该状态集合作为DFA中的终止状态。
     * @param stateSets 状态集合
     * @param dfaStates DFA中的状态
     * @param nfa 状态集合所在的NFA
     * @return DFA所有终止状态的ID
     * @note 如果状态集合中有多个终止状态，则只取第一个终止状态的代码
     */
    std::set<int> checkEndStates(std::vector<std::shared_ptr<std::set<int>>> &stateSets, std::vector<std::shared_ptr<DFAState>> &dfaStates, const NFA &nfa);

public:
    /**
     * @brief 获取单例对象
     * @return 单例对象
     */
    static DFAFactory &getInstance()
    {
        return instance;
    }

    /**
     * @brief 通过NFA生成DFA
     * @param nfa 用于生成DFA的NFA
     * @return 生成的DFA
     */
    std::unique_ptr<DFA> generate(const NFA &nfa);

    /**
     * @brief 通过Chlex对象生成DFA
     * @param nfaChlex 用于生成DFA的Chlex对象
     * @return 生成的DFA
     */
    std::unique_ptr<DFAChlex> generate(std::shared_ptr<NFAChlex> nfaChlex);
};

CHLEX_NAMESPACE_END