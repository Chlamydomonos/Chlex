/**
 * @file ChlexReader.cc
 * @brief ChlexReader.hh的实现
 * @date 2023-8-7
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#include "ChlexReader.hh"

#include <fstream>

using namespace chlex;

ChlexReader ChlexReader::instance;

std::unique_ptr<RawChlex> ChlexReader::read(std::istream &in)
{
    auto rawChlex = std::make_unique<RawChlex>();
    int lineNum = 0;

    // 读取第一行，获取所有Token
    std::string line;
    std::getline(in, line);
    lineNum++;

    std::string token;
    for (auto c : line)
    {
        if (c == ' ')
        {
            rawChlex->tokens.push_back(token);
            token.clear();
        }
        else
            token.push_back(c);
    }
    rawChlex->tokens.push_back(token);

    // 读取其余行，获取所有正则表达式
    while (std::getline(in, line))
    {
        lineNum++;

        auto regExp = std::make_unique<RegExp>();

        int patternLeftIndex = -1, patternRightIndex = -1, codeLeftIndex = -1, codeRightIndex = -1;
        bool afterSlash = false;

        for (int i = 0; i < line.length(); i++)
        {
            auto current = line[i];
            if (patternLeftIndex == -1 && current == '"')
            {
                patternLeftIndex = i;
                continue;
            }

            if (patternLeftIndex != -1 && current == '\\')
            {
                afterSlash = true;
                continue;
            }

            if (afterSlash)
            {
                afterSlash = false;
                continue;
            }

            if (patternLeftIndex != -1 && patternRightIndex == -1 && current == '"')
            {
                patternRightIndex = i;
                continue;
            }

            if (patternRightIndex != -1 && current == '{')
            {
                codeLeftIndex = i;
                break;
            }
        }

        for (int i = line.length() - 1; i >= 0; i--)
        {
            auto current = line[i];
            if (current == '}')
            {
                codeRightIndex = i;
                break;
            }
        }

        if (patternLeftIndex == -1 || patternRightIndex == -1 || codeLeftIndex == -1 || codeRightIndex == -1)
            throw ChlexReaderException("Invalid line", lineNum);

        if (codeLeftIndex >= codeRightIndex)
            throw ChlexReaderException("Invalid line", lineNum);

        regExp->pattern = line.substr(patternLeftIndex + 1, patternRightIndex - patternLeftIndex - 1);
        regExp->code = line.substr(codeLeftIndex + 1, codeRightIndex - codeLeftIndex - 1);

        rawChlex->regExps.push_back(std::move(regExp));
    }

    return rawChlex;
}

std::unique_ptr<RawChlex> ChlexReader::read(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in)
        throw ChlexReaderException("Cannot open file", -1);

    return read(in);
}