/**
 * @file LexerFactory.hh
 * @brief 有关词法分析程序生成的各个类的声明
 * @date 2023-8-13
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "Chlex.hh"

CHLEX_NAMESPACE_BEGIN

/**
 * @brief 词法分析程序生成器
 * @details 用于生成词法分析程序，是一个单例类
 */
class LexerFactory
{
private:
    static LexerFactory instance; ///< 单例对象

    /**
     * @brief 生成状态转移代码
     * @param dfa DFA
     * @return 状态转移代码
     */
    std::string fromState(const DFA &dfa, int stateId);

    /**
     * @brief 生成词法分析程序
     * @param chlex Chlex对象
     * @return 词法分析程序
     */
    std::string generateCode(const MinimizedDFAChlex &chlex);

public:
    /**
     * @brief 获取单例对象
     * @return 单例对象
     */
    static LexerFactory &getInstance() { return instance; }

    /**
     * @brief 生成词法分析程序
     * @param chlex Chlex对象
     * @return 词法分析程序
     */
    std::unique_ptr<ChlexLexer> generate(std::shared_ptr<MinimizedDFAChlex> chlex);
};

CHLEX_NAMESPACE_END