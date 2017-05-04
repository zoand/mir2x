/*
 * =====================================================================================
 *
 *       Filename: servermapop.cpp
 *        Created: 05/03/2016 20:21:32
 *  Last Modified: 05/03/2017 23:32:46
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
#include <cinttypes>
#include "player.hpp"
#include "monster.hpp"
#include "mathfunc.hpp"
#include "sysconst.hpp"
#include "actorpod.hpp"
#include "metronome.hpp"
#include "servermap.hpp"
#include "monoserver.hpp"

void ServerMap::On_MPK_HI(const MessagePack &, const Theron::Address &)
{
    delete m_Metronome;
    m_Metronome = new Metronome(1000);
    m_Metronome->Activate(GetAddress());
}

void ServerMap::On_MPK_METRONOME(const MessagePack &, const Theron::Address &)
{
    for(auto &rstRecordLine: m_UIDRecordV2D){
        for(auto &rstRecordV: rstRecordLine){
            for(auto nUID: rstRecordV){
                extern MonoServer *g_MonoServer;
                if(auto stUIDRecord = g_MonoServer->GetUIDRecord(nUID)){
                    if(stUIDRecord.ClassFrom<ActiveObject>()){
                        m_ActorPod->Forward(MPK_METRONOME, stUIDRecord.Address);
                    }
                }
            }
        }
    }
}

void ServerMap::On_MPK_ACTION(const MessagePack &rstMPK, const Theron::Address &)
{
    AMAction stAMA;
    std::memcpy(&stAMA, rstMPK.Data(), sizeof(stAMA));

    if(ValidC(stAMA.X, stAMA.Y)){
        auto nX0 = std::max<int>(0,   (stAMA.X - SYS_MAPVISIBLEW));
        auto nY0 = std::max<int>(0,   (stAMA.Y - SYS_MAPVISIBLEH));
        auto nX1 = std::min<int>(W(), (stAMA.X + SYS_MAPVISIBLEW));
        auto nY1 = std::min<int>(H(), (stAMA.Y + SYS_MAPVISIBLEH));

        for(int nX = nX0; nX <= nX1; ++nX){
            for(int nY = nY0; nY <= nY1; ++nY){
                if(ValidC(nX, nY)){
                    for(auto nUID: m_UIDRecordV2D[nX][nY]){
                        extern MonoServer *g_MonoServer;
                        if(auto stUIDRecord = g_MonoServer->GetUIDRecord(nUID)){
                            if(stUIDRecord.ClassFrom<Player>()){
                                m_ActorPod->Forward({MPK_ACTION, stAMA}, stUIDRecord.Address);
                            }
                        }
                    }
                }
            }
        }
    }
}

void ServerMap::On_MPK_ADDCHAROBJECT(const MessagePack &rstMPK, const Theron::Address &rstFromAddr)
{
    AMAddCharObject stAMACO;
    std::memcpy(&stAMACO, rstMPK.Data(), sizeof(stAMACO));

    if(!In(stAMACO.Common.MapID, stAMACO.Common.X, stAMACO.Common.Y)){
        extern MonoServer *g_MonoServer;
        g_MonoServer->AddLog(LOGTYPE_WARNING, "invalid argument in adding char object package");
        g_MonoServer->Restart();
    }

    if(!CanMove(stAMACO.Common.X, stAMACO.Common.Y)){
        m_ActorPod->Forward(MPK_ERROR, rstFromAddr, rstMPK.ID());
        return;
    }

    if(m_CellRecordV2D[stAMACO.Common.X][stAMACO.Common.Y].Freezed){
        m_ActorPod->Forward(MPK_ERROR, rstFromAddr, rstMPK.ID());
        return;
    }

    switch(stAMACO.Type){
        case TYPE_MONSTER:
            {
                auto pCO = new Monster(stAMACO.Monster.MonsterID,
                        m_ServiceCore,
                        this,
                        stAMACO.Common.X,
                        stAMACO.Common.Y,
                        DIR_UP,
                        STATE_INCARNATED);

                auto nUID = pCO->UID();
                auto nX   = stAMACO.Common.X;
                auto nY   = stAMACO.Common.Y;

                pCO->Activate();
                m_UIDRecordV2D[nX][nY].push_back(nUID);
                m_ActorPod->Forward(MPK_OK, rstFromAddr, rstMPK.ID());
                break;
            }
        case TYPE_PLAYER:
            {
                auto pCO = new Player(stAMACO.Player.DBID,
                        m_ServiceCore,
                        this,
                        stAMACO.Common.X,
                        stAMACO.Common.Y,
                        stAMACO.Player.Direction,
                        STATE_INCARNATED);

                auto nUID = pCO->UID();
                auto nX   = stAMACO.Common.X;
                auto nY   = stAMACO.Common.Y;

                pCO->Activate();
                m_UIDRecordV2D[nX][nY].push_back(nUID);
                m_ActorPod->Forward(MPK_OK, rstFromAddr, rstMPK.ID());
                m_ActorPod->Forward({MPK_BINDSESSION, stAMACO.Player.SessionID}, pCO->GetAddress());
                break;
            }
        default:
            {
                m_ActorPod->Forward(MPK_ERROR, rstFromAddr, rstMPK.ID());
                break;
            }
    }
}

void ServerMap::On_MPK_TRYSPACEMOVE(const MessagePack &rstMPK, const Theron::Address &)
{
    AMTrySpaceMove stAMTSM;
    std::memcpy(&stAMTSM, rstMPK.Data(), sizeof(stAMTSM));

    if(!In(stAMTSM.MapID, stAMTSM.X, stAMTSM.Y)){
        extern MonoServer *g_MonoServer;
        g_MonoServer->AddLog(LOGTYPE_WARNING, "destination is not in current map, routing error");
        g_MonoServer->Restart();
    }
}

void ServerMap::On_MPK_TRYMOVE(const MessagePack &rstMPK, const Theron::Address &rstFromAddr)
{
    AMTryMove stAMTM;
    std::memcpy(&stAMTM, rstMPK.Data(), sizeof(stAMTM));

    if(!In(stAMTM.MapID, stAMTM.EndX, stAMTM.EndY)){
        extern MonoServer *g_MonoServer;
        g_MonoServer->AddLog(LOGTYPE_WARNING, "destination is not in current map, routing error");
        g_MonoServer->Restart();
    }

    if(!CanMove(stAMTM.EndX, stAMTM.EndY)){
        m_ActorPod->Forward(MPK_ERROR, rstFromAddr, rstMPK.ID());
        return;
    }

    if(m_CellRecordV2D[stAMTM.EndX][stAMTM.EndY].Freezed){
        m_ActorPod->Forward(MPK_ERROR, rstFromAddr, rstMPK.ID());
        return;
    }

    auto fnOnR = [this, stAMTM](const MessagePack &rstRMPK, const Theron::Address &){
        switch(rstRMPK.Type()){
            case MPK_OK:
                {
                    // the object comfirm to move
                    // and it's internal state has changed

                    // 1. leave last cell
                    {
                        bool bFind = false;
                        auto &rstRecordV = m_UIDRecordV2D[stAMTM.X][stAMTM.Y];

                        for(auto &nUID: rstRecordV){
                            if(nUID == stAMTM.UID){
                                // 1. mark as find
                                bFind = true;

                                // 2. remove from the object list
                                std::swap(nUID, rstRecordV.back());
                                rstRecordV.pop_back();

                                break;
                            }
                        }

                        if(!bFind){
                            extern MonoServer *g_MonoServer;
                            g_MonoServer->AddLog(LOGTYPE_FATAL, "CharObject is not in current map: UID = %" PRIu32 , stAMTM.UID);
                            g_MonoServer->Restart();
                        }
                    }

                    // 2. push it to the new cell
                    //    check if it should switch the map
                    extern MonoServer *g_MonoServer;
                    if(auto stRecord = g_MonoServer->GetUIDRecord(stAMTM.UID)){
                        m_UIDRecordV2D[stAMTM.EndX][stAMTM.EndY].push_back(stRecord.UID);
                        if(m_CellRecordV2D[stAMTM.EndX][stAMTM.EndY].MapID){
                            AMMapSwitch stAMMS;
                            stAMMS.UID   = m_CellRecordV2D[stAMTM.EndX][stAMTM.EndY].UID;
                            stAMMS.MapID = m_CellRecordV2D[stAMTM.EndX][stAMTM.EndY].MapID;
                            m_ActorPod->Forward({MPK_MAPSWITCH, stAMMS}, stRecord.Address);
                        }
                    }
                    break;
                }
            default:
                {
                    break;
                }
        }
        m_CellRecordV2D[stAMTM.EndX][stAMTM.EndY].Freezed = false;
    };

    m_CellRecordV2D[stAMTM.EndX][stAMTM.EndY].Freezed = true;
    m_ActorPod->Forward(MPK_OK, rstFromAddr, rstMPK.ID(), fnOnR);
}

void ServerMap::On_MPK_TRYLEAVE(const MessagePack &rstMPK, const Theron::Address &rstFromAddr)
{
    AMTryLeave stAMTL;
    std::memcpy(&stAMTL, rstMPK.Data(), rstMPK.DataLen());

    if(true
            && stAMTL.UID
            && stAMTL.MapID
            && ValidC(stAMTL.X, stAMTL.Y)){
        auto &rstRecordV = m_UIDRecordV2D[stAMTL.X][stAMTL.Y];
        for(auto &nUID: rstRecordV){
            if(nUID == stAMTL.UID){
                std::swap(rstRecordV.back(), nUID);
                rstRecordV.pop_back();
                m_ActorPod->Forward(MPK_OK, rstFromAddr, rstMPK.ID());
                return;
            }
        }
    }

    // otherwise try leave failed
    // can't leave means it's illegal then we won't report MPK_ERROR
    extern MonoServer *g_MonoServer;
    g_MonoServer->AddLog(LOGTYPE_WARNING, "Leave request failed: UID = " PRIu32 ", X = %d, Y = %d", stAMTL.UID, stAMTL.X, stAMTL.Y);
}

void ServerMap::On_MPK_PULLCOINFO(const MessagePack &rstMPK, const Theron::Address &)
{
    AMPullCOInfo stAMPCOI;
    std::memcpy(&stAMPCOI, rstMPK.Data(), sizeof(stAMPCOI));

    for(auto &rstRecordLine: m_UIDRecordV2D){
        for(auto &rstRecordV: rstRecordLine){
            for(auto nUID: rstRecordV){
                extern MonoServer *g_MonoServer;
                if(auto stUIDRecord = g_MonoServer->GetUIDRecord(nUID)){
                    m_ActorPod->Forward({MPK_PULLCOINFO, stAMPCOI.SessionID}, stUIDRecord.Address);
                }
            }
        }
    }
}

void ServerMap::On_MPK_TRYMAPSWITCH(const MessagePack &rstMPK, const Theron::Address &)
{
    AMTryMapSwitch stAMTMS;
    std::memcpy(&stAMTMS, rstMPK.Data(), sizeof(stAMTMS));
}
