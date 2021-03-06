/*
 * =====================================================================================
 *
 *       Filename: eventtask.hpp
 *        Created: 04/03/2016 22:55:21
 *  Last Modified: 04/21/2016 18:11:02
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

#include "task.hpp"

class EventTask: public Task
{
    public:
        friend class EventTaskHub;

    protected:
		uint32_t m_EventID;

	protected:
		EventTask(uint32_t nDelayMS, const std::function<void (void)>& fnOp)
            : Task(nDelayMS, fnOp)
            , m_EventID(0)
        {}

        virtual ~EventTask() = default;

	public:
		void ID(uint32_t nID)
        {
			m_EventID = nID;
		}

		uint32_t ID() const
        {
			return m_EventID;
		}

        std::chrono::system_clock::time_point Cycle() const
        {
            return m_Expiration;
        }
};
