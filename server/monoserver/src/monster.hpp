/*
 * =====================================================================================
 *
 *       Filename: monster.hpp
 *        Created: 04/10/2016 02:32:45 AM
 *  Last Modified: 03/22/2017 17:00:26
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
#include "charobject.hpp"

enum MonsterType: uint32_t
{
    MONSTER_DEER,
};

typedef struct stMONSTERITEMINFO
{
    int     MonsterIndex;
    int     Type;
    int     Chance;
    int     Count;

    stMONSTERITEMINFO(int nMonsterIndex = -1)
        : MonsterIndex(nMonsterIndex)
    {}
}MONSTERITEMINFO;

typedef struct stMONSTERRACEINFO
{
    int     Index;
    int     Race;
    int     LID;
    int     Undead;
    int     Level;
    int     HP;
    int     MP;
    int     AC;
    int     MAC;
    int     DC;
    int     AttackSpead;
    int     WalkSpead;
    int     Spead;
    int     Hit;
    int     ViewRange;
    int     RaceIndex;
    int     Exp;
    int     Escape;
    int     Water;
    int     Fire;
    int     Wind;
    int     Light;
    int     Earth;

    std::string Name;
    std::vector<MONSTERITEMINFO> ItemV;

    stMONSTERRACEINFO(int nIndex = -1)
        : Index(nIndex)
        , Name("")
    {}
}MONSTERRACEINFO;

class Monster: public CharObject
{
    protected:
        uint32_t    m_MonsterID;
        bool        m_FreezeWalk;

    public:
        Monster(uint32_t,               // monster id
                ServiceCore *,          // service core
                ServerMap *,            // server map
                int,                    // map x
                int,                    // map y
                int,                    // direction
                uint8_t,                // life cycle state
                uint8_t);               // action state
       ~Monster() = default;

    public:
        uint32_t NameColor();
        const char *CharName();

        int Range(uint8_t);

    public:
        void SearchViewRange();

    public:

        bool RequestMove(int, int);
        int Speed()
        {
            return 20;
        }

        // TODO
        bool Update();

    public:
        void RequestSpaceMove(const char *, int, int);
        bool RandomWalk();

    private:
        bool UpdateLocation();

    private:
        void On_MPK_HI(const MessagePack &, const Theron::Address &);
        void On_MPK_METRONOME(const MessagePack &, const Theron::Address &);
        void On_MPK_UPDATECOINFO(const MessagePack &, const Theron::Address &);

    protected:
        void Operate(const MessagePack &, const Theron::Address &);

    protected:
        void ReportCORecord(uint32_t);

#if defined(MIR2X_DEBUG) && (MIR2X_DEBUG >= 5)
    protected:
        const char *ClassName()
        {
            return "Monster";
        }
#endif
};
