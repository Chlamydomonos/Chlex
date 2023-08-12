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

/**
 * @brief 正则表达式节点类型类
 * @details 用于表示正则表达式抽象语法树的节点类型
 */
enum class RENodeType
{
    CHAR,     ///< 字符
    OR,       ///< 或
    CONCAT,   ///< 连接
    STAR,     ///< 星闭包
    PLUS,     ///< 正闭包
    QUESTION, ///< 问号闭包
};

/**
 * @brief 正则表达式节点类
 * @details 用于表示正则表达式抽象语法树的节点，是所有具体节点的基类
 */
struct RENode
{
    RENodeType type; ///< 节点类型

    /**
     * @brief 构造函数
     * @param type 节点类型
     */
    RENode(RENodeType type) : type(type) {}
};

/**
 * @brief 字符节点类
 * @details 用于表示正则表达式抽象语法树中的字符节点
 */
struct CharNode : public RENode
{
    char value; ///< 字符的值

    /**
     * @brief 构造函数
     * @param value 字符的值
     *
     * @note 节点类型会被自动设为 RENodeType::CHAR
     */
    CharNode(char value) : RENode(RENodeType::CHAR), value(value) {}
};

/**
 * @brief 单目运算符节点类
 * @details 用于表示正则表达式抽象语法树中的单目运算符节点，包括星闭包、正闭包和问号闭包
 */
struct MonoOpNode : public RENode
{
    std::unique_ptr<RENode> child; ///< 子节点

    /**
     * @brief 构造函数
     * @param type 节点类型，必须是 RENodeType::STAR、RENodeType::PLUS 或 RENodeType::QUESTION
     * @param child 子节点
     */
    MonoOpNode(RENodeType type, std::unique_ptr<RENode> child) : RENode(type), child(std::move(child)) {}
};

/**
 * @brief 双目运算符节点类
 * @details 用于表示正则表达式抽象语法树中的双目运算符节点，包括或和连接
 */
struct BiOpNode : public RENode
{
    std::unique_ptr<RENode> left;  ///< 左子节点
    std::unique_ptr<RENode> right; ///< 右子节点

    /**
     * @brief 构造函数
     * @param type 节点类型，必须是 RENodeType::OR 或 RENodeType::CONCAT
     * @param left 左子节点
     * @param right 右子节点
     */
    BiOpNode(RENodeType type, std::unique_ptr<RENode> left, std::unique_ptr<RENode> right)
        : RENode(type), left(std::move(left)), right(std::move(right)) {}
};

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