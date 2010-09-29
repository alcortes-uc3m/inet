//
// Copyright (C) 2010 Alberto Cortés Martín
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include <omnetpp.h>
#include "SCTPAp.h"
#include "SCTPCommand_m.h"

SCTPAp::SCTPAp(bool enable,
               double periodSecs,
               int burst,
               double giveUpSecs,
               SCTPPathVariables& rPath)
    : mIsEnabled(enable),
      mPeriodSecs(periodSecs),
      mBurst(burst),
      mrPath(rPath),
      mrAssoc(*rPath.association),
      mIsActivated(false),
      mAlreadySent(0)
{
    const IPvXAddress& addr = rPath.remoteAddress;
    SCTPPathInfo* pinfo = new SCTPPathInfo("pinfo");
    pinfo->setRemoteAddress(addr);
    char str[128];

    snprintf(str, sizeof(str), "AP_PERIOD_TIMER %d:%s",
             mrAssoc.assocId,
             addr.str().c_str());
    mPeriodTimer = cMessage(str);
    mPeriodTimer.setContextPointer(&mrAssoc);
    mPeriodTimer.setControlInfo(pinfo->dup());

    snprintf(str, sizeof(str), "AP_GIVEUP_TIMER %d:%s",
             mrAssoc.assocId,
             addr.str().c_str());
    mGiveUpTimer = cMessage(str);
    mGiveUpTimer.setContextPointer(&mrAssoc);
    mGiveUpTimer.setControlInfo(pinfo->dup());

    delete pinfo;
}

SCTPAp::~SCTPAp()
{
    mrAssoc.stopTimer(&mPeriodTimer);
    mrAssoc.stopTimer(&mGiveUpTimer);
}


bool
SCTPAp::ActivateIfNeeded()
{
    // TODO check more activation conditions
    if (!mIsEnabled)
        return false;
    if (IsActivated())
        return false;

    EV << "---- Active Probing activated" << endl;

    // send the first HEARTBEAT
    mrAssoc.sendHeartbeat(&mrPath);
    mAlreadySent++;

    // schedule the next HEARTBEAT
    mrAssoc.startTimer(&mPeriodTimer, mPeriodSecs);
    // schedule the give up timer
    mrAssoc.startTimer(&mGiveUpTimer, mGiveUpSecs);

    mIsActivated = true;
    return true;
}

void
SCTPAp::Deactivate()
{
    if (!mIsEnabled)
        return;

    EV << "---- Probing deactivated" << std::endl;
    mrAssoc.stopTimer(&mPeriodTimer);
    mrAssoc.stopTimer(&mGiveUpTimer);
    mAlreadySent = 0;
    mIsActivated = false;
}

void
SCTPAp::DeactivateOnAllPaths()
{
    if (!mIsEnabled)
        return;

    EV << "---- Active Probing deactivating in all paths" << std::endl;
    SCTPAssociation::SCTPPathMap path_map = mrAssoc.sctpPathMap;
    for (SCTPAssociation::SCTPPathMap::iterator iterator = path_map.begin();
         iterator != path_map.end() ;
         ++iterator) {
        SCTPPathVariables* p_path = iterator->second;
        p_path->mpActiveProbing->Deactivate();
    }
}

bool
SCTPAp::IsActivated()
{
    if (!mIsEnabled)
        return false;
    return mIsActivated;
}

bool
SCTPAp::IsApTimer(const cMessage* const pMsg)
{
    if (pMsg == &mPeriodTimer)
        return true;
    if (pMsg == &mGiveUpTimer)
        return true;
    return false;
}

void
SCTPAp::ProcessTimeout(const cMessage* const pMsg)
{
    if (pMsg == &mPeriodTimer) {
        mrAssoc.stopTimer(&mPeriodTimer);

        if (! mIsEnabled)
            return;
        if (! mAlreadySent < mBurst)
            return;

        EV << "---- Active Probing period timeout" << endl;
        mrAssoc.sendHeartbeat(&mrPath);
        mAlreadySent++;
        mrAssoc.startTimer(&mPeriodTimer, mPeriodSecs);
        return;
    }

    if (pMsg == &mGiveUpTimer) {
        EV << "---- Active Probing: give up timeout TODO" << endl;
        return;
    }

    EV << "---- Active Probing error: asked to process unknown timer" << endl;
    return;
}
