/**
 * @file NFAFactory.cc
 * @brief NFAFactory.hh的实现
 * @date 2023-8-6
 * @version 0.1
 * @author Chlamydomonos
 * @copyright
 */

#include "NFAFactory.hh"

#include "RegExpParser.hh"

#include <stdexcept>

using namespace chlex;

NFAFactory NFAFactory::instance;

void NFAFactory::connect(NFAState &from, const NFAState &to, char byChar)
{
    from.paths.push_back(std::make_unique<NFAPath>(NFAPath{from, to, byChar}));
}

std::unique_ptr<NFA> NFAFactory::fromChar(char c, IDAllocator &idAllocator)
{
    auto start = std::make_unique<NFAState>(NFAState{idAllocator.nextID()});
    auto end = std::make_unique<NFAEndState>(NFAEndState{idAllocator.nextID()});

    connect(*start, *end, c);

    auto nfa = std::make_unique<NFA>(*start);
    nfa->getStates().insert({start->id, std::move(start)});
    nfa->getStates().insert({end->id, std::move(end)});

    return nfa;
}

std::unique_ptr<NFA> NFAFactory::fromOr(std::unique_ptr<NFA> left, std::unique_ptr<NFA> right, IDAllocator &idAllocator)
{
    auto start = std::make_unique<NFAState>(NFAState{idAllocator.nextID()});
    auto end = std::make_unique<NFAEndState>(NFAEndState{idAllocator.nextID()});

    connect(*start, left->getStartState(), 0);
    connect(*start, right->getStartState(), 0);
    connect(left->getEndStates().begin()->second, *end, 0);
    connect(right->getEndStates().begin()->second, *end, 0);

    auto nfa = std::make_unique<NFA>(*start);
    nfa->getEndStates().insert({end->id, *end});
    nfa->getStates().insert({start->id, std::move(start)});
    nfa->getStates().insert({end->id, std::move(end)});

    for (auto &state : left->getStates())
        nfa->getStates().insert(std::move(state));

    for (auto &state : right->getStates())
        nfa->getStates().insert(std::move(state));

    return nfa;
}

std::unique_ptr<NFA> NFAFactory::fromConcat(std::unique_ptr<NFA> left, std::unique_ptr<NFA> right, IDAllocator &idAllocator)
{
    auto nfa = std::make_unique<NFA>(left->getStartState());

    for (auto &i : left->getEndStates())
    {
        auto &endState = i.second;
        connect(endState, right->getStartState(), 0);
        nfa->getEndStates().insert(i);
    }

    for (auto &state : left->getStates())
        nfa->getStates().insert(std::move(state));

    for (auto &state : right->getStates())
        nfa->getStates().insert(std::move(state));

    return nfa;
}

std::unique_ptr<NFA> NFAFactory::fromClosure(std::unique_ptr<NFA> nfa, IDAllocator &idAllocator)
{
    auto start = std::make_unique<NFAState>(NFAState{idAllocator.nextID()});
    auto end = std::make_unique<NFAEndState>(NFAEndState{idAllocator.nextID()});

    connect(*start, nfa->getStartState(), 0);
    connect(nfa->getEndStates().begin()->second, *end, 0);
    connect(*start, *end, 0);
    connect(nfa->getEndStates().begin()->second, nfa->getStartState(), 0);

    auto newNFA = std::make_unique<NFA>(*start);
    newNFA->getEndStates().insert({end->id, *end});
    newNFA->getStates().insert({start->id, std::move(start)});
    newNFA->getStates().insert({end->id, std::move(end)});

    for (auto &state : nfa->getStates())
        newNFA->getStates().insert(std::move(state));

    return newNFA;
}

std::unique_ptr<NFA> NFAFactory::fromPlus(std::unique_ptr<NFA> nfa, IDAllocator &idAllocator)
{
    auto start = std::make_unique<NFAState>(NFAState{idAllocator.nextID()});
    auto end = std::make_unique<NFAEndState>(NFAEndState{idAllocator.nextID()});

    connect(*start, nfa->getStartState(), 0);
    connect(nfa->getEndStates().begin()->second, *end, 0);
    connect(nfa->getEndStates().begin()->second, nfa->getStartState(), 0);

    auto newNFA = std::make_unique<NFA>(*start);
    newNFA->getEndStates().insert({end->id, *end});
    newNFA->getStates().insert({start->id, std::move(start)});
    newNFA->getStates().insert({end->id, std::move(end)});

    for (auto &state : nfa->getStates())
        newNFA->getStates().insert(std::move(state));

    return newNFA;
}

std::unique_ptr<NFA> NFAFactory::fromQuestion(std::unique_ptr<NFA> nfa, IDAllocator &idAllocator)
{
    auto start = std::make_unique<NFAState>(NFAState{idAllocator.nextID()});
    auto end = std::make_unique<NFAEndState>(NFAEndState{idAllocator.nextID()});

    connect(*start, nfa->getStartState(), 0);
    connect(nfa->getEndStates().begin()->second, *end, 0);
    connect(*start, *end, 0);

    auto newNFA = std::make_unique<NFA>(*start);
    newNFA->getEndStates().insert({end->id, *end});
    newNFA->getStates().insert({start->id, std::move(start)});
    newNFA->getStates().insert({end->id, std::move(end)});

    for (auto &state : nfa->getStates())
        newNFA->getStates().insert(std::move(state));

    return newNFA;
}

std::unique_ptr<NFA> NFAFactory::generate(const RENode &ast, IDAllocator &idAllocator)
{
    switch (ast.type)
    {
    case RENodeType::CHAR:
    {
        const auto &charNode = static_cast<const CharNode &>(ast);
        return fromChar(charNode.value, idAllocator);
    }
    case RENodeType::OR:
    {
        const auto &orNode = static_cast<const BiOpNode &>(ast);
        auto leftNFA = generate(*orNode.left, idAllocator);
        auto rightNFA = generate(*orNode.right, idAllocator);
        return fromOr(std::move(leftNFA), std::move(rightNFA), idAllocator);
    }
    case RENodeType::CONCAT:
    {
        const auto &concatNode = static_cast<const BiOpNode &>(ast);
        auto leftNFA = generate(*concatNode.left, idAllocator);
        auto rightNFA = generate(*concatNode.right, idAllocator);
        return fromConcat(std::move(leftNFA), std::move(rightNFA), idAllocator);
    }
    case RENodeType::STAR:
    {
        const auto &starNode = static_cast<const MonoOpNode &>(ast);
        auto childNFA = generate(*starNode.child, idAllocator);
        return fromClosure(std::move(childNFA), idAllocator);
    }
    case RENodeType::PLUS:
    {
        const auto &plusNode = static_cast<const MonoOpNode &>(ast);
        auto childNFA = generate(*plusNode.child, idAllocator);
        return fromPlus(std::move(childNFA), idAllocator);
    }
    case RENodeType::QUESTION:
    {
        const auto &questionNode = static_cast<const MonoOpNode &>(ast);
        auto childNFA = generate(*questionNode.child, idAllocator);
        return fromQuestion(std::move(childNFA), idAllocator);
    }
    default:
        throw std::runtime_error("Unknown RENodeType (this should never happen)");
    }
}

std::unique_ptr<NFA> NFAFactory::generate(const ParsedRegExp &parsedRegExp, IDAllocator &idAllocator)
{
    auto nfa = generate(*parsedRegExp.ast, idAllocator);
    auto &endState = static_cast<NFAEndState &>(nfa->getEndStates().begin()->second);
    endState.code = parsedRegExp.regExp->code;
    return nfa;
}

std::unique_ptr<NFAChlex> NFAFactory::generate(std::shared_ptr<ParsedChlex> parsedChlex)
{
    auto nfaChlex = std::make_unique<NFAChlex>();
    nfaChlex->parsedChlex = parsedChlex;

    IDAllocator idAllocator;
    std::vector<std::unique_ptr<NFA>> nfas;

    // 对于每一个正则表达式，生成一个NFA
    for (auto &parsedRegExp : parsedChlex->getRegExps())
        nfas.push_back(generate(*parsedRegExp, idAllocator));

    // 创建一个新的起始状态，将所有NFA的起始状态连接到这个新的起始状态
    auto start = std::make_unique<NFAState>(NFAState{idAllocator.nextID()});

    for (auto &nfa : nfas)
        connect(*start, nfa->getStartState(), 0);

    // 合并所有NFA
    auto nfa = std::make_unique<NFA>(*start);
    for (auto &nfa : nfas)
        for (auto &endState : nfa->getEndStates())
            nfa->getEndStates().insert(endState);

    nfa->getStates().insert({start->id, std::move(start)});
    for (auto &nfa : nfas)
        for (auto &state : nfa->getStates())
            nfa->getStates().insert(std::move(state));

    nfaChlex->nfa = std::move(nfa);
    return nfaChlex;
}
