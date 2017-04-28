/*
 * =====================================================================================
 *
 *       Filename: player.cpp
 *        Created: 04/07/2016 03:48:41 AM
 *  Last Modified: 04/27/2017 15:17:25
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

#include "netpod.hpp"
#include "player.hpp"
#include "memorypn.hpp"
#include "charobject.hpp"
#include "protocoldef.hpp"

Player::Player(uint32_t nDBID,
        ServiceCore    *pServiceCore,
        ServerMap      *pServerMap,
        int             nMapX,
        int             nMapY,
        int             nDirection,
        uint8_t         nLifeState)
    : CharObject(pServiceCore, pServerMap, nMapX, nMapY, nDirection, nLifeState)
    , m_DBID(nDBID)
    , m_JobID(0)        // will provide after bind
    , m_SessionID(0)    // provide by bind
    , m_Level(0)        // after bind
{
    m_StateHook.Install("CheckTime", [this](){ For_CheckTime(); return false; });

    ResetType(TYPE_CHAR, TYPE_PLAYER);
    ResetType(TYPE_PLAYER, TYPE_PLAYER);

    ResetType(TYPE_CREATURE, TYPE_HUMAN);
    ResetType(TYPE_HUMAN,  TYPE_HUMAN);
}

void Player::Operate(const MessagePack &rstMPK, const Theron::Address &rstFromAddr)
{
    switch(rstMPK.Type()){
        case MPK_METRONOME:
            {
                On_MPK_METRONOME(rstMPK, rstFromAddr);
                break;
            }
        case MPK_ACTION:
            {
                On_MPK_ACTION(rstMPK, rstFromAddr);
                break;
            }
        case MPK_BINDSESSION:
            {
                On_MPK_BINDSESSION(rstMPK, rstFromAddr);
                break;
            }
        case MPK_NETPACKAGE:
            {
                On_MPK_NETPACKAGE(rstMPK, rstFromAddr);
                break;
            }
        case MPK_PULLCOINFO:
            {
                On_MPK_PULLCOINFO(rstMPK, rstFromAddr);
                break;
            }
        default:
            {
                extern MonoServer *g_MonoServer;
                g_MonoServer->AddLog(LOGTYPE_WARNING, "unsupported message: %s", rstMPK.Name());
                g_MonoServer->Restart();
                break;
            }
    }
}

bool Player::Update()
{
    return true;
}

uint8_t Player::Type(uint8_t nType)
{
    return m_TypeV[nType];
}

bool Player::ResetType(uint8_t nType, uint8_t nThisType)
{
    m_TypeV[nType] = nThisType;
    return true;
}

uint8_t Player::State(uint8_t nState)
{
    return m_StateV[nState];
}

bool Player::ResetState(uint8_t nState, uint8_t nThisState)
{
    m_StateV[nState] = nThisState;
    return true;
}

uint32_t Player::NameColor()
{
    return 0XFFFFFFFF;
}

const char *Player::CharName()
{
    return "hello";
}

int Player::Range(uint8_t)
{
    return 20;
}

void Player::OperateNet(uint8_t nType, const uint8_t *pData, size_t nDataLen)
{
    switch(nType){
        case CM_QUERYMONSTERGINFO: Net_CM_QUERYMONSTERGINFO(nType, pData, nDataLen); break;
        case CM_ACTION           : Net_CM_ACTION           (nType, pData, nDataLen); break;
        default                  :                                                   break;
    }
}

void Player::For_CheckTime()
{
}

bool Player::Bind(uint32_t nSessionID)
{
    m_SessionID = nSessionID;

    extern NetPodN *g_NetPodN;
    g_NetPodN->Bind(m_SessionID, GetAddress());
    return true;
}

void Player::ReportCORecord(uint32_t nSessionID)
{
    if(nSessionID){
        SMCORecord stSMCOR;

        stSMCOR.Type = CREATURE_PLAYER;

        stSMCOR.Common.UID       = UID();
        stSMCOR.Common.MapID     = MapID();
        stSMCOR.Common.X         = X();
        stSMCOR.Common.Y         = Y();
        stSMCOR.Common.EndX      = X();
        stSMCOR.Common.EndY      = Y();
        stSMCOR.Common.Direction = Direction();
        stSMCOR.Common.Speed     = Speed();
        stSMCOR.Common.Action    = ACTION_STAND;

        stSMCOR.Player.DBID      = m_DBID;
        stSMCOR.Player.JobID     = m_JobID;
        stSMCOR.Player.Level     = m_Level;

        extern NetPodN *g_NetPodN;
        g_NetPodN->Send(nSessionID, SM_CORECORD, stSMCOR);
    }else{
        extern MonoServer *g_MonoServer;
        g_MonoServer->AddLog(LOGTYPE_WARNING, "invalid session id");
        g_MonoServer->Restart();
    }
}
