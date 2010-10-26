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

#ifndef SCTPAP_H
#define SCTPAP_H

#include "SCTPAssociation.h"

class SCTPPathVariables;

/* The class ActiveProbing is a new path variable to be added to the
   SCTPPathVariables inet class */
class SCTPAp
{
public:
    SCTPAp(bool enabled,
           double periodSecs,
           int burst,
           double giveUpSecs,
           SCTPPathVariables& rPath);
    ~SCTPAp();

    bool IsOn();
    bool TurnOnIfNeeded();
    void TurnOff();
    void TurnOffOnAllPaths();

    bool IsApTimer(const cMessage* const pMsg);
    void ProcessTimeout(const cMessage* const pMsg);

protected:
    // ====== Simulation Parameters ===========================================
    bool mIsEnabled;
    // Period between HEARTBEATs while AP is on
    double mPeriodSecs;
    // Number of HEARTBEATs to send while AP is on
    int mBurst;
    // Time to wait for SACKs or HEARTBEAT-ACKs
    double mGiveUpSecs;

    // ====== Housekeeping ====================================================
    // we belong to this path
    SCTPPathVariables& mrPath;
    // we belong to this association
    SCTPAssociation& mrAssoc;
    // is AP running for this path?
    bool mIsOn;
    // how many HEARTBEATs already sent while AP is on
    int mAlreadySent;
    // Timer for the periodic send of HEARBEATs
    cMessage* mpPeriodTimer;
    // Timer to set the path as inactive
    cMessage* mpGiveUpTimer;
};

#endif // SCTPAP_H
