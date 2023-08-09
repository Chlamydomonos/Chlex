/**
 * @file RegExp.hh
 * @brief 有关正则表达式的各个类的声明
 * @date 2023-8-6
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "chlex_base.hh"

#include <string>
#include <memory>

CHLEX_NAMESPACE_BEGIN

/**
 * @brief 正则表达式类
 * @details 用于表示正则表达式以及正则表达式匹配后执行的代码
 */
struct RegExp
{
    std::string pattern; ///< 正则表达式的模式
    std::string code;    ///< 正则表达式匹配后执行的代码
};

struct RENode;

/**
 * @brief 解析后的正则表达式类
 * @details 用于表示正则表达式解析后的结果，包含原始正则表达式和抽象语法树
 */
struct ParsedRegExp
{
    std::shared_ptr<RegExp> regExp; ///< 原始正则表达式
    std::unique_ptr<RENode> ast;    ///< 抽象语法树

    /**
     * @brief 构造函数
     * @param regExp 原始正则表达式
     * @param ast 抽象语法树
     */
    ParsedRegExp(const std::shared_ptr<RegExp> &regExp, std::unique_ptr<RENode> ast)
        : regExp(regExp), ast(std::move(ast)) {}
};

CHLEX_NAMESPACE_END