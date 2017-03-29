/*
 * =====================================================================================
 *
 *       Filename: pathfinder.hpp
 *        Created: 03/28/2017 17:04:54
 *  Last Modified: 03/28/2017 19:50:57
 *
 *    Description: A-Star algorithm for path find
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#pragma once

#include <functional>

#include "fsa.h"
#include "stlastar.h"

class AStarPathFinderNode;
class AStarPathFinder: public AStarSearch<AStarPathFinderNode>
{
    public:
        friend class AStarPathFinderNode;

    private:
        std::function<bool(int, int, int, int)> m_MoveChecker;
        std::function<int (int, int, int, int)> m_MoveCost;

    public:
        AStarPathFinder(std::function<bool(int, int, int, int)> fnMoveChecker)
            : AStarSearch<AStarPathFinderNode>()
            , m_MoveChecker(fnMoveChecker)
            , m_MoveCost()
        {
            assert(fnMoveChecker);
        }

        AStarPathFinder(std::function<bool(int, int, int, int)> fnMoveChecker, std::function<int(int, int, int, int)> fnMoveCost)
            : AStarSearch<AStarPathFinderNode>()
            , m_MoveChecker(fnMoveChecker)
            , m_MoveCost(fnMoveCost)
        {
            assert(fnMoveChecker);
        }

        ~AStarPathFinder()
        {
            FreeSolutionNodes();
            EnsureMemoryFreed();
        }

    public:
        bool Search(int, int, int, int);
};

class AStarPathFinderNode
{
    public:
        friend class AStarPathFinder;
        friend class AStarSearch<AStarPathFinderNode>;

    private:
        int m_CurrX;
        int m_CurrY;

    private:
        AStarPathFinder *m_Finder;

    private:
        AStarPathFinderNode() = default;
        AStarPathFinderNode(int nX, int nY, AStarPathFinder *pFinder)
            : m_CurrX(nX)
            , m_CurrY(nY)
            , m_Finder(pFinder)
        {
            assert(pFinder);
        }

    public:
       ~AStarPathFinderNode() = default;

    public:
       int X(){ return m_CurrX; }
       int Y(){ return m_CurrY; }

    public:
        float GoalDistanceEstimate(AStarPathFinderNode &rstGoalNode)
        {
            return 1.0 * (std::abs(rstGoalNode.m_CurrX - m_CurrX) + std::abs(rstGoalNode.m_CurrY - m_CurrY));
        }

        bool IsGoal(AStarPathFinderNode &rstGoalNode)
        {
            return (m_CurrX == rstGoalNode.m_CurrX) && (m_CurrY == rstGoalNode.m_CurrY);
        }

        bool GetSuccessors(AStarSearch<AStarPathFinderNode> *pAStarSearch, AStarPathFinderNode *pParentNode)
        {
            static const int nDX[] = { 0, +1, +1, +1,  0, -1, -1, -1};
            static const int nDY[] = {-1, -1,  0, +1, +1, +1,  0, -1};

            for(int nIndex = 0; nIndex < 8; ++nIndex){
                int nNewX = m_CurrX + nDX[nIndex];
                int nNewY = m_CurrY + nDY[nIndex];

                if(true
                        && pParentNode
                        && pParentNode->X() == nNewX
                        && pParentNode->Y() == nNewY){ continue; }

                // m_MoveChecker() directly refuse invalid (nX, nY) as false
                // this ensures if pParentNode not null then pParentNode->(X, Y) is always valid
                if(m_Finder->m_MoveChecker(m_CurrX, m_CurrY, nNewX, nNewY)){
                    AStarPathFinderNode stFinderNode {nNewX, nNewY, m_Finder};
                    pAStarSearch->AddSuccessor(stFinderNode);
                }
            }

            // seems we have to always return true
            // return false means out of memory in the code
            return true;
        }

        float GetCost(AStarPathFinderNode &rstNode)
        {
            if(m_Finder->m_MoveCost){
                auto nCost = m_Finder->m_MoveCost(m_CurrX, m_CurrY, rstNode.m_CurrX, rstNode.m_CurrY);
                return (nCost > 0) ? (nCost * 1.0) : 1.0;
            }else{
                return 1.0;
            }
        }

        bool IsSameState(AStarPathFinderNode &rstNode)
        {
            return (m_CurrX == rstNode.m_CurrX) && (m_CurrY == rstNode.m_CurrY);
        }
};

bool AStarPathFinder::Search(int nX0, int nY0, int nX1, int nY1)
{
    AStarPathFinderNode stNode0 {nX0, nY0, this};
    AStarPathFinderNode stNode1 {nX1, nY1, this};
    SetStartAndGoalStates(stNode0, stNode1);

    unsigned int nSearchState;
    do{
        nSearchState = SearchStep();
    }while(nSearchState == AStarSearch<AStarPathFinderNode>::SEARCH_STATE_SEARCHING);
    return nSearchState == AStarSearch<AStarPathFinderNode>::SEARCH_STATE_SUCCEEDED;
}
