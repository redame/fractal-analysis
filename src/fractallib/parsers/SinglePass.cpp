#include "SinglePass.h"

using namespace FL::Parsers;
using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::Trees;

SinglePass::SinglePass()
{
}

FL::ParseResult SinglePass::analyze(
    const TimeSeries &ts, Forest &forest, PatternsSet &patterns,
    int begin, int end)
{
    m_result.reset();

    try
    {
        if (ts.values().size() < 2)
            throw EAnalyze(E_EMPTY_TIME_SERIES);
        if (forest.size() == 0)
            throw EAnalyze(E_EMPTY_FOREST);
        if (patterns.size() == 0)
            throw EAnalyze(E_EMPTY_PATTERNS);
        if (begin >= (int)ts.values().size() ||
            end >= (int)ts.values().size() ||
            (end != -1 && begin >= end))
            throw EAnalyze(E_INVALID_SEGMENT);

        m_begin = begin;
        if (end > 0)
            m_end = end;
        else
            m_end = ts.values().size()-1;

        Forest::Iterator tree;
        forall(tree, forest)
        {
            (*tree)->validateStructure();
            analyzeTree(ts, **tree, patterns);
        }
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
        m_lastError.setArg(descriptionOf(e.id()));
    }

    return m_result;
}

void SinglePass::analyzeTree(const TimeSeries &ts, Tree &tree, PatternsSet &patterns)
{
    // Initialize iterators
    PatternsSet::Iterator pattern;

    // Initialize analyse context
    Context context;
    context.setTimeSeries(&ts);
    context.setParseTree(&tree);
    context.setOutputTree(&tree);
    context.setCandidateNode(new Node());
    context.advanceCurrentRootToPos(m_begin);

    // Look for applicable patterns in each position of tree
    while (!context.isAt(m_end))
    {
        bool found = false;
        forall(pattern, patterns)
        {
            if (applyPattern(**pattern, context))
            {
                found = true;
                break;
            }
        }

        if (!found)
            context.advanceCurrentRoot(1);
    }
    delete context.candidateNode();
}

bool SinglePass::applyPattern(Pattern &pattern, Context &context)
{
    CheckInfo info;
    if (pattern.check(context, info) == crOK)
    {
        // Pattern sequence that was applied (use only one)
        CISequence &seq = *info.applicableSequences[0];

        // Insert candidate node into output tree
        context.buildLastParsed(seq);
        Layer::Iterator child;
        forall(child, context.lastParsed())
            (*child)->setParent(context.candidateNode());
        context.outputTree().add(context.candidateNode());

        // Remember modification
//        context.modification().push_back(context.candidateNode());

        // Update result, advance current roots position
        context.advanceCurrentRoot(seq.size());
        m_result.nodesAdded += 1;
        context.setCandidateNode(new Node());

        return true;
    }

    return false;
}
