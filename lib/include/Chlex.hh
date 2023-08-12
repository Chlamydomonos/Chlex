/**
 * @file Chlex.hh
 * @brief 有关Chlex对象的各个类的声明
 * @date 2023-8-7
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "RegExp.hh"
#include "NFA.hh"
#include "DFA.hh"

#include <vector>

CHLEX_NAMESPACE_BEGIN

/**
 * @brief 原始Chlex
 * @details 从文件中读入，未经处理的Chlex
 * @note 由ChlexReader生成
 */
class RawChlex
{
private:
    std::vector<std::string> tokens;              ///< 所有Token
    std::vector<std::shared_ptr<RegExp>> regExps; ///< 所有正则表达式

    friend class ChlexReader;
    friend class RegExpParser;

public:
    /**
     * @brief 获取所有Token
     * @return 所有Token
     */
    const std::vector<std::string> &getTokens() const { return tokens; }

    /**
     * @brief 获取所有正则表达式
     * @return 所有正则表达式
     */
    const std::vector<std::shared_ptr<RegExp>> &getRegExps() const { return regExps; }
};

/**
 * @brief 解析后的Chlex
 * @details 经过解析后的Chlex，包含了所有解析后的正则表达式
 * @note 由RegExpParser生成
 */
class ParsedChlex
{
private:
    std::shared_ptr<RawChlex> rawChlex;                 ///< 原始Chlex
    std::vector<std::unique_ptr<ParsedRegExp>> regExps; ///< 所有解析后的正则表达式

    friend class RegExpParser;
    friend class NFAFactory;

public:
    /**
     * @brief 获取原始Chlex
     * @return 原始Chlex
     */
    const RawChlex &getRawChlex() const { return *rawChlex; }

    /**
     * @brief 获取所有解析后的正则表达式
     * @return 所有解析后的正则表达式
     */
    const std::vector<std::unique_ptr<ParsedRegExp>> &getRegExps() const { return regExps; }
};

/**
 * @brief 含有NFA的Chlex
 * @details 包含了解析后的Chlex和对应的NFA
 * @note 由NFAFactory生成
 */
class NFAChlex
{
private:
    std::shared_ptr<ParsedChlex> parsedChlex; ///< 解析后的Chlex
    std::unique_ptr<NFA> nfa;                 ///< 对应的NFA

    friend class NFAFactory;
    friend class DFAFactory;

public:
    /**
     * @brief 获取解析后的Chlex
     * @return 解析后的Chlex
     */
    const ParsedChlex &getParsedChlex() const { return *parsedChlex; }

    /**
     * @brief 获取对应的NFA
     * @return 对应的NFA
     */
    const NFA &getNFA() const { return *nfa; }
};

/**
 * @brief 含有DFA的Chlex
 * @details 包含了含有NFA的Chlex和对应的DFA
 * @note 由DFAFactory生成
 */
class DFAChlex
{
private:
    std::shared_ptr<NFAChlex> nfaChlex; ///< 含有NFA的Chlex
    std::unique_ptr<DFA> dfa;           ///< 对应的DFA

    friend class DFAFactory;
    friend class DFAMinimizer;

public:
    /**
     * @brief 获取含有NFA的Chlex
     * @return 含有NFA的Chlex
     */
    const NFAChlex &getNFAChlex() const { return *nfaChlex; }

    /**
     * @brief 获取对应的DFA
     * @return 对应的DFA
     */
    const DFA &getDFA() const { return *dfa; }
};

/**
 * @brief 含有最小化DFA的Chlex
 * @details 包含了含有DFA的Chlex和对应的最小化DFA
 * @note 由DFAMinimizer生成
 */
class MinimizedDFAChlex
{
private:
    std::shared_ptr<DFAChlex> dfaChlex; ///< 含有DFA的Chlex
    std::unique_ptr<DFA> minimizedDFA;  ///< 对应的DFA

    friend class DFAMinimizer;

public:
    /**
     * @brief 获取含有DFA的Chlex
     * @return 含有DFA的Chlex
     */
    const DFAChlex &getDFAChlex() const { return *dfaChlex; }

    /**
     * @brief 获取对应的DFA
     * @return 对应的DFA
     */
    const DFA &getMinimizedDFA() const { return *minimizedDFA; }
};

class ChlexLexer
{
private:
    std::shared_ptr<MinimizedDFAChlex> minimizedDFAChlex; ///< 含有最小化DFA的Chlex
    std::string code;                                     ///< 代码

    friend class LexerFactory;

public:
    /**
     * @brief 获取含有最小化DFA的Chlex
     * @return 含有最小化DFA的Chlex
     */
    const MinimizedDFAChlex &getMinimizedDFAChlex() const { return *minimizedDFAChlex; }

    /**
     * @brief 获取代码
     * @return 代码
     */
    const std::string &getCode() const { return code; }
};

CHLEX_NAMESPACE_END