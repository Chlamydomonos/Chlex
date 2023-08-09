/**
 * @file ChlexReader.hh
 * @brief 有关Chlex读取的各个类的声明
 * @date 2023-8-7
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#pragma once

#include "Chlex.hh"

#include <iostream>

CHLEX_NAMESPACE_BEGIN

/**
 * @brief Chlex读取类
 * @details 用于从文件中读取Chlex，是一个单例类
 * Chlex语法：Chlex文件分为若干行，其中第一行为Token声明，其余行为正则表达式声明。
 * 在第一行中，声明Chlex文件中的所有Token，中间用空格分隔。
 * 在其余行中，声明正则表达式，格式为："正则表达式" {代码}。
 */
class ChlexReader
{
private:
    static ChlexReader instance; ///< 单例对象
public:
    /**
     * @brief 获取单例对象
     * @return 单例对象
     */
    static ChlexReader &getInstance() { return instance; }

    /**
     * @brief 从文件中读取Chlex
     * @param in 输入流
     * @return 读取到的Chlex对象
     */
    std::unique_ptr<RawChlex> read(std::istream &in);

    /**
     * @brief 从文件中读取Chlex
     * @param filename 文件名
     * @return 读取到的Chlex对象
     */
    std::unique_ptr<RawChlex> read(const std::string &filename);
};

/**
 * @brief Chlex读取异常类
 * @details 用于表示Chlex读取过程中的异常
 */
struct ChlexReaderException : public std::exception
{
    std::string message; ///< 异常信息
    int lineNum;         ///< 出错的行号

    /**
     * @brief 构造函数
     * @param message 异常信息
     * @param lineNum 出错的行号
     */
    ChlexReaderException(const std::string &message, int lineNum) : message(message), lineNum(lineNum) {}
};

CHLEX_NAMESPACE_END