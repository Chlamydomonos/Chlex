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
 * @brief DFA最小化类
 * @details 用于将DFA最小化，是一个单例类
 */
class DFAMinimizer
{
private:
    static DFAMinimizer instance; ///< 单例对象
public:
    static DFAMinimizer &getInstance() { return instance; } ///< 获取单例对象
};

CHLEX_NAMESPACE_END