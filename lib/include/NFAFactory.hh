/**
 * @file NFAFactory.hh
 * @brief 有关NFA工厂的各个类的声明
 * @date 2023-8-6
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "Chlex.hh"

CHLEX_NAMESPACE_BEGIN

/**
 * @brief ID分配器类
 * @details 用于分配NFA状态的id，保证每个状态的id不同
 */
class IDAllocator
{
private:
    unsigned int id = 0; ///< 自增的id

public:
    IDAllocator() = default; ///< 默认构造函数

    /**
     * @brief 获取下一个id
     * @return 下一个id
     */
    unsigned int nextID()
    {
        return id++;
    }
};

/**
 * @brief NFA工厂类
 * @details 用于通过正则表达式生成NFA，是一个单例类
 */
class NFAFactory
{
private:
    static NFAFactory instance; ///< 单例对象

    /**
     * @brief 连接两个状态
     * @param from 起点
     * @param to 终点
     * @param byChar 路径上的字符
     * @note 用0表示ε
     */
    void connect(NFAState &from, const NFAState &to, char byChar);

    /**
     * @brief 从字符生成NFA
     * @param c 字符
     * @param idAllocator id分配器
     * @return 接受该字符的NFA
     */
    std::unique_ptr<NFA> fromChar(char c, IDAllocator &idAllocator);

    /**
     * @brief 从或运算生成NFA
     * @param left 左操作数
     * @param right 右操作数
     * @param idAllocator id分配器
     * @return 两个操作数的或运算的NFA
     * @note 输入的两个NFA必须只有一个终止状态
     */
    std::unique_ptr<NFA> fromOr(std::unique_ptr<NFA> left, std::unique_ptr<NFA> right, IDAllocator &idAllocator);

    /**
     * @brief 从连接生成NFA
     * @param left 左操作数
     * @param right 右操作数
     * @param idAllocator id分配器
     * @return 两个操作数的连接的NFA
     * @note 输入的两个NFA必须只有一个终止状态
     */
    std::unique_ptr<NFA> fromConcat(std::unique_ptr<NFA> left, std::unique_ptr<NFA> right, IDAllocator &idAllocator);

    /**
     * @brief 从闭包生成NFA
     * @param nfa 操作数
     * @param idAllocator id分配器
     * @return 操作数的闭包的NFA
     * @note 输入的NFA必须只有一个终止状态
     */
    std::unique_ptr<NFA> fromClosure(std::unique_ptr<NFA> nfa, IDAllocator &idAllocator);

    /**
     * @brief 从正闭包生成NFA
     * @param nfa 操作数
     * @param idAllocator id分配器
     * @return 操作数的正闭包的NFA
     * @note 输入的NFA必须只有一个终止状态
     */
    std::unique_ptr<NFA> fromPlus(std::unique_ptr<NFA> nfa, IDAllocator &idAllocator);

    /**
     * @brief 从问号闭包生成NFA
     * @param nfa 操作数
     * @param idAllocator id分配器
     * @return 操作数的问号闭包的NFA
     * @note 输入的NFA必须只有一个终止状态
     */
    std::unique_ptr<NFA> fromQuestion(std::unique_ptr<NFA> nfa, IDAllocator &idAllocator);

    /**
     * @brief 从正则表达式语法树生成NFA
     * @param ast 语法树根节点
     * @param idAllocator id分配器
     * @return 生成的NFA
     */
    std::unique_ptr<NFA> generate(const RENode &ast, IDAllocator &idAllocator);

public:
    /**
     * @brief 获取单例对象
     * @return 单例对象
     */
    static NFAFactory &getInstance() { return instance; }

    /**
     * @brief 从正则表达式语法树生成NFA
     * @param parsedRegExp 解析后的正则表达式
     * @param idAllocator id分配器
     * @return 生成的NFA
     */
    std::unique_ptr<NFA> generate(const ParsedRegExp &parsedRegExp, IDAllocator &idAllocator);

    /**
     * @brief 从解析后的Chlex对象生成NFA
     * @param parsedChlex 解析后的Chlex
     * @return 生成的NFA
     * @note 此函数中会自动创建一个id分配器
     */
    std::unique_ptr<NFAChlex> generate(std::shared_ptr<ParsedChlex> parsedChlex);
};

CHLEX_NAMESPACE_END