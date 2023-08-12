/**
 * @file RegExpParser.cc
 * @brief RegExpParser.hh的实现
 * @date 2023-8-5
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#include "RegExpParser.hh"

#include <stdexcept>

using namespace chlex;

RegExpParser RegExpParser::instance;

std::unique_ptr<RENode> RegExpParser::parseFrom(const std::string &re, int pos, int &endPos, bool inBrace, bool inBracket)
{
    std::vector<char> opStack;                      // 操作符栈
    std::vector<std::unique_ptr<RENode>> nodeStack; // 节点栈

    bool readChar = false; // 上一个读取到的是否是字符
                           // 此变量的意义是检测是否有两个字符相邻，如果有，则需要插入一个连接符

    for (auto i = pos; i < re.length(); i++)
    {
        auto current = re[i];

        switch (current)
        {
        case '|': // 遇到或运算符。由于它是优先级最低的双目运算符，所以直接将栈中的所有运算符弹出，然后将它压入栈中
        {
            if (inBracket) // []中只能出现'-'
                throw RegExpParserException("Unexpected operator '|'", i);

            readChar = false;

            try
            {
                popStacks(opStack, nodeStack);
            }
            catch (const std::exception &e)
            {
                throw RegExpParserException("Unexpected operator '|'", i);
            }

            opStack.push_back('|');
            break;
        }
        case '*': // 遇到单目运算符，直接把栈中最后一个节点弹出，然后将它作为子节点，构造一个新的节点，再压入栈中
                  // 之后的'+'和'?'的处理方式与此相同
        {
            if (inBracket)
                throw RegExpParserException("Unexpected operator '*'", i);

            readChar = true; // 考虑类似"a*b"的情况，此时需要插入一个连接符
            if (nodeStack.empty())
                throw RegExpParserException("Unexpected operator '*'", i);

            auto node = std::move(nodeStack.back());
            nodeStack.pop_back();

            auto newNode = std::make_unique<MonoOpNode>(RENodeType::STAR, std::move(node));
            nodeStack.push_back(std::move(newNode));
            break;
        }
        case '+':
        {
            if (inBracket)
                throw RegExpParserException("Unexpected operator '+'", i);

            readChar = true;
            if (nodeStack.empty())
                throw RegExpParserException("Unexpected operator '+'", i);

            auto node = std::move(nodeStack.back());
            nodeStack.pop_back();

            auto newNode = std::make_unique<MonoOpNode>(RENodeType::PLUS, std::move(node));
            nodeStack.push_back(std::move(newNode));
            break;
        }
        case '?':
        {
            if (inBracket)
                throw RegExpParserException("Unexpected operator '?'", i);

            readChar = true;
            if (nodeStack.empty())
                throw RegExpParserException("Unexpected operator '?'", i);

            auto node = std::move(nodeStack.back());
            nodeStack.pop_back();

            auto newNode = std::make_unique<MonoOpNode>(RENodeType::QUESTION, std::move(node));
            nodeStack.push_back(std::move(newNode));
            break;
        }
        case '(': // 遇到左括号，递归调用parseFrom()，然后将返回值压入栈中
        {
            if (inBracket)
                throw RegExpParserException("Unexpected operator '('", i);

            if (readChar) // 考虑类似"a(b...)"的情况，此时需要插入一个连接符
                          // 由于连接符的优先级最高，所以直接将它压入栈中
                opStack.push_back('&');

            readChar = true; // 考虑类似"(a...)b"的情况，此时需要插入一个连接符

            int resultPos;
            auto result = parseFrom(re, i + 1, resultPos, true, false);
            nodeStack.push_back(std::move(result));
            i = resultPos + 1;
            break;
        }
        case ')': // 遇到右括号，尝试清空栈并返回最终结果
        {
            if (!inBrace || inBracket)
                throw RegExpParserException("Unexpected operator ')'", i);

            try
            {
                popStacks(opStack, nodeStack);
            }
            catch (const std::exception &e)
            {
                throw RegExpParserException("Unexpected operator ')'", i);
            }

            endPos = i;
            return std::move(nodeStack.back());
        }
        case '[': // 遇到左中括号，递归调用parseFrom()，然后将返回值压入栈中
        {
            if (inBracket)
                throw RegExpParserException("Unexpected operator '['", i);

            if (readChar) // 考虑类似"a[b...]"的情况，此时需要插入一个连接符
                opStack.push_back('&');

            readChar = true; // 考虑类似"[a...]b"的情况，此时需要插入一个连接符

            int resultPos;
            auto result = parseFrom(re, i + 1, resultPos, inBrace, true);
            nodeStack.push_back(std::move(result));
            i = resultPos + 1;
            break;
        }
        case ']': // 遇到右中括号，尝试清空栈并返回最终结果
        {
            if (!inBracket)
                throw RegExpParserException("Unexpected operator ']'", i);

            try
            {
                popStacks(opStack, nodeStack);
            }
            catch (const std::exception &e)
            {
                throw RegExpParserException("Unexpected operator ']'", i);
            }

            endPos = i;
            return std::move(nodeStack.back());
        }
        case '-': // 遇到'-'，检查栈顶运算符
                  // 若为'-'，说明出现类似'a-b-c'的情况，是非法的
                  // 若为'&'，同样非法（该运算符不会出现在中括号内）
                  // 若为'|'，把它压入栈中
                  // 若栈为空，把它压入栈中
        {
            if (!inBracket) // 只有在[]中才能出现'-'
                throw RegExpParserException("Unexpected operator '-'", i);

            if (nodeStack.back()->type != RENodeType::CHAR) // '-'只能出现在字符之间，诸如'\s-a'是非法的
                throw RegExpParserException("Unexpected operator '-'", i);

            readChar = false;

            if (opStack.empty())
                opStack.push_back('-');
            else
            {
                auto top = opStack.back();

                if (top == '|')
                {
                    opStack.push_back('-');
                }
                else
                {
                    throw RegExpParserException("Unexpected operator '-'", i);
                }
            }
            break;
        }
        case '.': // 遇到'.'，直接将其展开为所有字符的或运算
        {
            if (inBracket)
                throw RegExpParserException("Unexpected operator '.'", i);

            if (readChar)
                opStack.push_back('&');

            readChar = true;
            auto node = makeFromDot();
            nodeStack.push_back(std::move(node));
            break;
        }
        case '\\': // 遇到'\'，检查下一个字符
        {
            if (readChar) // 在中括号内时，两个字符相邻代表或运算，否则代表连接
            {
                if (inBracket)
                    opStack.push_back('|');
                else
                    opStack.push_back('&');
            }

            readChar = true;
            i++;
            if (i >= re.length())
                throw RegExpParserException("Unexpected '\\'", i);

            auto next = re[i];
            switch (next)
            {
            case 'd': // 遇到'\d'，直接将其展开为所有数字的或运算
            {
                auto node = makeFromDigits();
                nodeStack.push_back(std::move(node));
                break;
            }
            case 's': // 遇到'\s'，直接将其展开为所有空白字符的或运算
            {
                auto node = makeFromEmpty();
                nodeStack.push_back(std::move(node));
                break;
            }
            case 'x': // 遇到'\x'，检查下两个字符
            {
                i++;
                if (i >= re.length())
                    throw RegExpParserException("Unexpected '\\x'", i);

                auto c1 = re[i];
                i++;
                if (i >= re.length())
                    throw RegExpParserException("Unexpected '\\x'", i);

                auto c2 = re[i];

                if (c1 >= '0' && c1 <= '9')
                {
                    c1 = c1 - '0';
                }
                else if (c1 >= 'a' && c1 <= 'f')
                {
                    c1 = c1 - 'a' + 10;
                }
                else if (c1 >= 'A' && c1 <= 'F')
                {
                    c1 = c1 - 'A' + 10;
                }
                else
                {
                    throw RegExpParserException("Unexpected '\\x'", i);
                }

                if (c2 >= '0' && c2 <= '9')
                {
                    c2 = c2 - '0';
                }
                else if (c2 >= 'a' && c2 <= 'f')
                {
                    c2 = c2 - 'a' + 10;
                }
                else if (c2 >= 'A' && c2 <= 'F')
                {
                    c2 = c2 - 'A' + 10;
                }
                else
                {
                    throw RegExpParserException("Unexpected '\\x'", i);
                }

                auto node = std::make_unique<CharNode>(c1 << 4 + c2);
                nodeStack.push_back(std::move(node));
                break;
            }
            case '\\': // 遇到'\\'，直接按照'\'处理
            {
                auto node = std::make_unique<CharNode>('\\');
                nodeStack.push_back(std::move(node));
                break;
            }
            case '"': // 遇到'\"'，直接按照'"'处理
            {
                auto node = std::make_unique<CharNode>('"');
                nodeStack.push_back(std::move(node));
                break;
            }
            default:
            {
                throw RegExpParserException("Unexpected '\\'", i);
            }
            }
        }
        default: // 遇到字符，直接将其压入栈中
        {
            if (readChar)
            {
                if (inBracket)
                    opStack.push_back('|');
                else
                    opStack.push_back('&');
            }

            readChar = true;
            auto node = std::make_unique<CharNode>(current);
            nodeStack.push_back(std::move(node));
        }
        }
    }

    if (inBracket)
        throw RegExpParserException("Missing ']'", re.length() - 1);
    if (inBrace)
        throw RegExpParserException("Missing ')'", re.length() - 1);

    try
    {
        popStacks(opStack, nodeStack);
    }
    catch (const std::exception &e)
    {
        throw RegExpParserException("Unexpected end of regular expression", re.length() - 1);
    }

    return std::move(nodeStack.back());
}

void RegExpParser::popStacks(std::vector<char> &opStack, std::vector<std::unique_ptr<RENode>> &nodeStack)
{
    while (!opStack.empty())
    {
        auto op = opStack.back();
        opStack.pop_back();

        if (nodeStack.empty())
            throw std::runtime_error("Invalid regular expression");
        auto right = std::move(nodeStack.back());
        nodeStack.pop_back();

        if (nodeStack.empty())
            throw std::runtime_error("Invalid regular expression");
        auto left = std::move(nodeStack.back());
        nodeStack.pop_back();

        std::unique_ptr<RENode> node = nullptr;
        switch (op)
        {
        case '&':
            node = std::make_unique<BiOpNode>(RENodeType::CONCAT, std::move(left), std::move(right));
            break;
        case '|':
            node = std::make_unique<BiOpNode>(RENodeType::OR, std::move(left), std::move(right));
            break;
        }
        nodeStack.push_back(std::move(node));
    }

    if (nodeStack.size() != 1)
        throw std::runtime_error("Invalid regular expression");
}

std::unique_ptr<RENode> RegExpParser::makeFromDot()
{
    std::unique_ptr<RENode> result = nullptr;
    for (char c = 0; c < 128; c++)
    {
        auto node = std::make_unique<CharNode>(c);
        if (result == nullptr)
        {
            result = std::move(node);
        }
        else
        {
            result = std::make_unique<BiOpNode>(RENodeType::OR, std::move(result), std::move(node));
        }
    }

    return result;
}

std::unique_ptr<RENode> RegExpParser::makeFromRange(char from, char to)
{
    if (from > to)
    {
        throw std::runtime_error("Invalid range");
    }

    std::unique_ptr<RENode> result = nullptr;
    for (char c = from; c <= to; c++)
    {
        auto node = std::make_unique<CharNode>(c);
        if (result == nullptr)
        {
            result = std::move(node);
        }
        else
        {
            result = std::make_unique<BiOpNode>(RENodeType::OR, std::move(result), std::move(node));
        }
    }

    return result;
}

std::unique_ptr<RENode> RegExpParser::makeFromDigits()
{
    std::unique_ptr<RENode> result = nullptr;
    for (char c = '0'; c <= '9'; c++)
    {
        auto node = std::make_unique<CharNode>(c);
        if (result == nullptr)
        {
            result = std::move(node);
        }
        else
        {
            result = std::make_unique<BiOpNode>(RENodeType::OR, std::move(result), std::move(node));
        }
    }

    return result;
}

std::unique_ptr<RENode> RegExpParser::makeFromEmpty()
{
    std::unique_ptr<RENode> result = nullptr;
    for (char c = 0; c < 256; c++)
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            auto node = std::make_unique<CharNode>(c);
            if (result == nullptr)
            {
                result = std::move(node);
            }
            else
            {
                result = std::make_unique<BiOpNode>(RENodeType::OR, std::move(result), std::move(node));
            }
        }
    }

    return result;
}

std::unique_ptr<ParsedChlex> RegExpParser::parse(std::shared_ptr<RawChlex> raw)
{
    auto parsedChlex = std::make_unique<ParsedChlex>();

    for (auto &i : raw->regExps)
    {
        auto parsed = parse(i);
        parsedChlex->regExps.push_back(std::move(parsed));
    }

    parsedChlex->rawChlex = raw;
    return parsedChlex;
}