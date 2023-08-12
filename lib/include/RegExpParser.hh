/**
 * @file RegExpParser.hh
 * @brief 有关正则表达式解析的各个类的声明
 * @date 2023-8-5
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "Chlex.hh"

#include <vector>

CHLEX_NAMESPACE_BEGIN

/**
 * @brief 正则表达式解析器类
 * @details 此类是一个单例类，用于将正则表达式解析为抽象语法树。
 * 其正则表达式支持以下运算符：|，+，*，?，()，[]，-，.。
 * 在解析过程中，()，[]，-和.会被展开，所以它们不会出现在抽象语法树中。
 * 另外，正则表达式支持以下转义字符：\\，\"，\d，\s，\xhh
 */
class RegExpParser
{
private:
    static RegExpParser instance; ///< 单例对象

    /**
     * @brief 从某个位置开始解析正则表达式
     *
     * @param re 正则表达式
     * @param pos 开始解析的位置
     * @param inBrace 是否在()中
     * @param inBracket 是否在[]中
     * @param endPos 解析结束后，此变量会被设为解析结束的位置
     *
     * @return 解析得到的节点
     */
    std::unique_ptr<RENode> parseFrom(const std::string &re, int pos, int &endPos, bool inBrace, bool inBracket);

    /**
     * @brief 清空运算符栈和节点栈，将其中的内容组装为一个节点，然后压入节点栈
     *
     * @param opStack 运算符栈
     * @param nodeStack 节点栈
     */
    void popStacks(std::vector<char> &opStack, std::vector<std::unique_ptr<RENode>> &nodeStack);

    /**
     * @brief 通过'.'构造一个节点
     *
     * @return 一个由所有字符通过或运算符连接而成的节点
     */
    std::unique_ptr<RENode> makeFromDot();

    /**
     * @brief 通过'-'构造一个节点
     *
     * @param from 起始字符
     * @param to 终止字符
     *
     * @return 一个由 @ref from 和 @ref to 之间所有字符通过或运算符连接而成的节点
     */
    std::unique_ptr<RENode> makeFromRange(char from, char to);

    /**
     * @brief 通过'\d'构造一个节点
     *
     * @return 一个由所有数字字符通过或运算符连接而成的节点
     */
    std::unique_ptr<RENode> makeFromDigits();

    /**
     * @brief 通过'\s'构造一个节点
     *
     * @return 一个由所有空白字符通过或运算符连接而成的节点
     */
    std::unique_ptr<RENode> makeFromEmpty();

public:
    /**
     * @brief 获取单例对象
     * @return 单例对象
     */
    static RegExpParser &getInstance() { return instance; }

    /**
     * @brief 解析正则表达式
     * @param re 正则表达式
     * @return 解析得到的抽象语法树
     */
    std::unique_ptr<RENode> parse(const std::string &re)
    {
        int _;
        return parseFrom(re, 0, _, false, false);
    }

    /**
     * @brief 解析正则表达式对象
     * @param regExp 正则表达式
     * @return 解析得到的结果
     */
    std::unique_ptr<ParsedRegExp> parse(std::shared_ptr<RegExp> regExp)
    {
        auto ast = parse(regExp->pattern);
        return std::make_unique<ParsedRegExp>(regExp, std::move(ast));
    }

    /**
     * @brief 解析Chlex对象
     * @param raw 原始Chlex
     * @return 解析得到的结果
     */
    std::unique_ptr<ParsedChlex> parse(std::shared_ptr<RawChlex> raw);
};

/**
 * @brief 正则表达式解析器异常类
 * @details 用于表示正则表达式解析器的异常
 */
struct RegExpParserException : public std::exception
{
    std::string message; ///< 异常信息
    int pos;             ///< 异常位置。-1表示没有位置信息

    /**
     * @brief 构造函数
     * @param message 异常信息
     * @param pos 异常位置
     */
    RegExpParserException(const std::string &message, int pos) : message(message), pos(pos) {}
};

CHLEX_NAMESPACE_END