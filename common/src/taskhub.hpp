/*
 * =====================================================================================
 *
 *       Filename: taskhub.hpp
 *        Created: 04/03/2016 22:14:46
 *  Last Modified: 05/16/2016 19:26:50
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

#include <list>
#include <condition_variable>

#include "task.hpp"
#include "basehub.hpp"
#include "memoryblockpn.hpp"

using TaskBlockPN = MemoryBlockPN<sizeof(Task)>;

class TaskHub: public BaseHub<TaskHub>
{
    protected:
        std::mutex              m_TaskLock;
        std::condition_variable m_TaskSignal;
        std::list<Task*>        m_TaskList;
        uint64_t                m_Cycle;
        TaskBlockPN             m_TaskBlockPN;

    public:
        TaskHub()
            : BaseHub<TaskHub>()
            , m_Cycle(0)
        {}

        virtual ~TaskHub() = default;
        
    public:
        void Add(Task* pTask, bool bPushHead = false)
        {
            if(!pTask){ return; }

            bool bDoSignal = false;
            m_TaskLock.lock();

            // still we are running
            if(State() == 2){
                bDoSignal = m_TaskList.empty();

                if(bPushHead){
                    m_TaskList.push_front(pTask);
                }else{
                    m_TaskList.push_back(pTask);
                }
            }else{
                DeleteTask(pTask);
            }

            m_TaskLock.unlock();

            if(bDoSignal){
                m_TaskSignal.notify_one();
            }
        }

        void Add(const std::function<void()> &fnOp, bool bPushHead = false)
        {
            Add(CreateTask(fnOp), bPushHead);
        }

        void Add(uint32_t nDuraMS, const std::function<void()> &fnOp, bool bPushHead = false)
        {
            Add(CreateTask(nDuraMS, fnOp), bPushHead);
        }

    public:
        // TODO do I need to make a pool for this
        Task *CreateTask(uint32_t nDuraMS, const std::function<void()> &fnOp)
        {
            void *pData = m_MBP.Get();

            // passing null argument to placement new is undefined behavior
            if(!pData){ return nullptr; }
            return new (pData) Task(nDuraMS, fnOp);
        }

        Task *CreateTask(const std::function<void()> &fnOp)
        {
            void *pData = m_MBP.Get();

            // passing null argument to placement new is undefined behavior
            if(!pData){ return nullptr; }
            return new (pData) Task(fnOp);
        }

        void DeleteTask(Task *pTask)
        {
            pTask->~Task();
            m_TaskBlockPN->Free(pTask);
        }

    public:

        // I have no idea of the purpose for the m-func
        uint64_t Cycle() const
        {
            return m_Cycle;
        }

    public:
        void MainLoop()
        {
            // NOTE: second argument defer_lock is to prevent from immediate locking
            std::unique_lock<std::mutex> stTaskUniqueLock(m_TaskLock, std::defer_lock);

            while(State() != 0){
                // check if there are tasks waiting
                stTaskUniqueLock.lock();

                if(m_TaskList.empty()){
                    //if the list is empty, then wait for signal
                    m_TaskSignal.wait(stTaskUniqueLock);
                }

                if(!m_TaskList.empty()){
                    auto pTask = m_TaskList.front();
                    m_TaskList.pop_front();
                    stTaskUniqueLock.unlock();

                    if(!pTask->Expired()){
                        ++m_Cycle;
                        // execute it
                        (*pTask)();
                    }
                    delete pTask;
                } else {
                    stTaskUniqueLock.unlock();
                }
            }
        }

        void Shutdown()
        {
            // TODO
            // we may need to clear the task queue
            // otherwise we'll run into trouble if we do
            //
            //      pTaskHub = new TaskHub();
            //      pTaskHub->Shutdown();
            //      pTaskHub->Launch();
            // think about how to implement it by either
            //  1. implement an task clean function
            //  2. let Shutdown() block until all task finished
            auto pTask = CreateTask([this](){
                    State(0);
                    m_TaskSignal.notify_one();
                });

            std::lock_guard<std::mutex> stLockGuard(m_TaskLock);
            m_TaskList.push_back(pTask);
            m_TaskSignal.notify_one();
        }
};
