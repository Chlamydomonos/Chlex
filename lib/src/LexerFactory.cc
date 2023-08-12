/**
 * @file LexerFactory.cc
 * @brief LexerFactory.hh的实现
 * @date 2023-8-13
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#include "LexerFactory.hh"

using namespace chlex;

LexerFactory LexerFactory::instance;

static const std::string code1 =
    "#include <iostream>\n"
    "#include <fstream>\n"
    "\n";

static const std::string code2 =
    "\n"
    "int lex(std::istream &in)\n"
    "{\n"
    "    int state = ";

static const std::string code3 =
    ";\n"
    "    int lastEndState = 0;\n"
    "    int lastEndStateIndex = 0;\n"
    "\n"
    "    char currentChar;\n"
    "\n"
    "    while (in.read(&currentChar, 1))\n"
    "    {\n"
    "        lastEndStateIndex++;\n"
    "        switch (state)\n"
    "        {\n";

static const std::string code4 =
    "        default:\n"
    "            goto end;\n"
    "        }\n"
    "    }\n"
    "\n"
    "    in.seekg(-lastEndStateIndex, std::ios::cur);\n"
    "\n"
    "end:\n"
    "    switch (lastEndState)\n"
    "    {\n";

static const std::string code5 =
    "    default:\n"
    "        return -1;\n"
    "    }\n"
    "};\n"
    "\n"
    "int main(int argc, char **argv)\n"
    "{\n"
    "    if (argc != 3)\n"
    "    {\n"
    "        std::cout << \"Usage: \" << argv[0] << \" <input file> <output file>\" << std::endl;\n"
    "        return 1;\n"
    "    }\n"
    "\n"
    "    std::ifstream in(argv[1]);\n"
    "    std::ofstream out(argv[2]);\n"
    "\n"
    "    while (true)\n"
    "    {\n"
    "        if (in.eof())\n"
    "            break;\n"
    "        int token = lex(in);\n"
    "        if (token == -1)\n"
    "            break;\n"
    "        out << token << ' ';\n"
    "    }\n"
    "\n"
    "    out << std::endl;\n"
    "    out.close();\n"
    "\n"
    "    return 0;\n"
    "}\n";

std::string LexerFactory::fromState(const DFA &dfa, int stateId)
{
    std::string result =
        "        case " + std::to_string(stateId) + ":\n" +
        "        {\n"
        "            switch (currentChar)\n"
        "            {\n";

    auto &state = dfa.getStates().at(stateId);
    for (auto &i : state->paths)
    {
        auto byChar = i.first;
        auto to = i.second;

        result +=
            "            case " + std::to_string(byChar) + ":\n" +
            "                state = " + std::to_string(to) + ";\n" +
            "                break;\n";
    }

    result +=
        "            default:\n"
        "                goto end;\n"
        "            }\n"
        "            break;\n"
        "        }\n";

    return result;
}

std::string LexerFactory::generateCode(const MinimizedDFAChlex &chlex)
{
    auto &tokens = chlex.getDFAChlex().getNFAChlex().getParsedChlex().getRawChlex().getTokens();

    std::string tokenDecl;
    for (int i = 0; i < tokens.size(); i++)
    {
        tokenDecl += "const int " + tokens[i] + " = " + std::to_string(i) + ";\n";
    }

    std::string stateSwitch;
    for (auto &i : chlex.getMinimizedDFA().getStates())
    {
        auto stateId = i.first;
        stateSwitch += fromState(chlex.getMinimizedDFA(), stateId);
    }

    std::string endSwitch;
    for (auto &i : chlex.getMinimizedDFA().getEndStates())
    {
        auto stateId = i.first;
        DFAEndState &state = i.second;
        endSwitch +=
            "        case " + std::to_string(stateId) + ":\n" +
            "        {\n" +
            state.code +
            "        break;\n" +
            "        }\n";
    }

    return code1 +
           tokenDecl +
           code2 +
           std::to_string(chlex.getMinimizedDFA().getStartState().id) +
           code3 +
           stateSwitch +
           code4 +
           endSwitch +
           code5;
}

std::unique_ptr<ChlexLexer> LexerFactory::generate(std::shared_ptr<MinimizedDFAChlex> chlex)
{
    auto lexer = std::make_unique<ChlexLexer>();
    lexer->code = generateCode(*chlex);
    lexer->minimizedDFAChlex = chlex;
    return lexer;
}