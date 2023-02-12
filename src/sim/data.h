//////////////////////////////////////////////////////////////////////////////////////

// YAACARS is Copyright 2018-2021 Markus Hahlbeck (markus@hahlbeck.com)
// 
// This file is part of YAACARS.
// 
// YAACARS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// YAACARS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with YAACARS.  If not, see <http://www.gnu.org/licenses/>.

//////////////////////////////////////////////////////////////////////////////////////

#ifndef FLIGHTDATA_H
#define FLIGHTDATA_H

//enum eFlightState
//{
//    FS_OFFLINE = -1,
//    FS_CONNECTED,
//    FS_PREFLIGHT,
//    FS_TAXITORWY,
//    //---------
//    FS_CLIMB,
//    FS_CRUISE,
//    FS_DESCEND,
//    //---------
//    FS_TAXITOGATE,
//    FS_POSTFLIGHT
//};

enum eAcarsState
{
    AS_IDLE = 0,
    AS_LOADED = 1, //FP loaded
    AS_TRACKING
};

enum eConnState
{
    CS_UNKNOWN = -1,
    CS_OFFLINE = 0,
    CS_CONNECTED = 1
};

enum eProto
{
    XACARS = 0,
    FTWJSON
};

enum eComm
{
    XPUDP = 0,
    FSUIPC
};

//bool bea, nav, ldn, str, txi;
//double qnhReal, qnhSet

struct T_Status
{
    QPair<double,double> coordsDEP;
    QPair<double,double> coordsARR;
    QPair<double,double> coordsALT;
    QPair<double,double> coordsCURR;
    QPair<double,double> coordsLAST;
    QPair<double,double> coordsLOADED;
    eAcarsState acarsState;
    eConnState connState;
    bool inhibitFlightNotStartedWarning;
    bool inhibitFlightNotStartedWarnSnd;
    bool isSuspended;
    bool isPaused;
    bool isAborted;
    bool isRefuelled;
    int isResumedFlightFTW;
    int missionTypeFTW;
    QString trackingIdFTW;
    qint64 trackingCount;
    unsigned int timeStamp;
    unsigned int rref;
    unsigned int simRate;
    double pitch;
    double bank;
    double HDG;
    double VS_fpm;
    double IAS_kts;
    double TAS_kts;
    double GS_kts;
    double ASL_ft;
    double AGL_ft;
    double PAYL_lb;
    double FUELftwTotal_lb;
    double FUELftwUsed_lb;
    double FUELsimCur_lb;
    double FUELsimNew_lb;
    double distance_nm;
    double timeBlock_secs;
    double timeFlight_secs;
    double timePause_secs;
    double timeTakeoff_secs;
    double timeLanding_secs;
    double timeRemoved_secs;
    double winddeg;
    double windknots;
    double oat;
};

#endif // FLIGHTDATA_H
