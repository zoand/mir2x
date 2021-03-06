/*
 * =====================================================================================
 *
 *       Filename: myhero.hpp
 *        Created: 04/07/2016 03:48:41 AM
 *  Last Modified: 05/05/2017 18:21:55
 *
 *    Description: 
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
#include "hero.hpp"
#include "actionnode.hpp"

class MyHero: public Hero
{
    private:
        std::deque<ActionNode> m_ActionQueue;

    public:
        MyHero(uint32_t, uint32_t, bool, uint32_t, ProcessRun *, const ActionNode &);

    public:
       ~MyHero() = default;

    public:
        bool RequestMove(int, int);

    public:
        bool Update();

    public:
        bool MoveNextMotion();

    public:
        bool ParseNewState (const  StateNode &, bool);
        bool ParseNewAction(const ActionNode &, bool);

    public:
        bool ParseActionQueue();
};
