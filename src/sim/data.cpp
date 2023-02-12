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

#include "src/ui/yaacars.h"
#include "ui_mainwindow.h"

//////////////////////////////////////////////////////////////////////////////////////
#define LOGGER_SECTION "DATA"

void MainWindow::calcFlightData(int diff_msec)
{
    static qint64 refuelDelayTimer=0;
    static qint64 limitDelayTimer=0;

    static qint64 waypointTimer=0;
    static double lastHDG=0;
    static QPair<double,double> lastCoords;
    static bool fuelSlip=false;

    //computation may get INF for pausetime and nm/s, so block this here!
    if(diff_msec<900)
    {
        //wrn("TimeDiff "+QS2I(diff_msec)+" ms is too low! Please report this bug!");
        return;
    }

    if(diff_msec>15000)
    {
        //wrn("TimeDiff "+QS2I(diff_msec)+" ms is too high! Please report this bug!");
        return;
    }

    if(fuelSlip==false
    && flightData.acarsState==AS_TRACKING
    && flightData.FUELsimNew_lb > 1000000)
    {
        log("REFUELLED in SIM: "+QS2F(flightData.FUELsimCur_lb,3)+" lb => "+QS2F(flightData.FUELsimNew_lb,3)+" lb (ignored SPIKE once)");
        fuelSlip=true;
        return;
    }
    fuelSlip=false;

    double tickDist=greatcircle(flightData.coordsCURR, flightData.coordsLAST);

    if(flightData.acarsState==AS_TRACKING)
    {
        {   //// ~~~~ PAUSE
            if (flightData.simRate<0.5 || (tickDist<0.00000001 && flightData.GS_kts>0.1))
            {
                if (!flightData.isPaused)
                {
                    //critPaused();
                    flightData.isPaused = true;
                }
            }
            else if (flightData.isPaused)
            {
                //critUnpaused();
                flightData.isPaused = false;
            }
        }


        {   //// ~~~~ calc new FUEL and USED FUEL
            if(flightData.FUELsimNew_lb < flightData.FUELsimCur_lb) //only if not refuelled in sim and tracking is running
            {
                double fuelUsed=flightData.FUELsimCur_lb-flightData.FUELsimNew_lb;

                //calc FTW fuel and fuel used
                flightData.FUELftwUsed_lb+=fuelUsed;

                flightData.FUELftwTotal_lb-=fuelUsed;
                //limit FTW fuel to positive values
                if(flightData.FUELftwTotal_lb<0)
                    flightData.FUELftwTotal_lb=0;
            }
            if(flightData.FUELsimNew_lb > flightData.FUELsimCur_lb+FUEL_CHECK_OFFSET_LBS)
                log("REFUELLED in SIM: "+QS2F(flightData.FUELsimCur_lb,3)+" lb => "+QS2F(flightData.FUELsimNew_lb,3)+" lb");

            //check for fuel warning
            if(flightData.FUELsimCur_lb>flightData.FUELftwTotal_lb+FUEL_CHECK_OFFSET_LBS)
            {
                if(time(nullptr)-refuelDelayTimer>refuelReportDelaySecs && !flightData.isPaused) //grace time expired?
                    flightData.isRefuelled=true; //only a hint now
            }
            else
            {
                flightData.isRefuelled=false;
                refuelDelayTimer=time(nullptr); //update time if fuel is ok
            }
        }


        {   //// ~~~~ isSuspended ???
            flightData.isSuspended=false; //will be set if fuel error or paused
            if(flightData.isAborted==true) //aborted is set in VA->FTWJSON
                flightData.isSuspended=true;
            if(flightData.isPaused==true)
                flightData.isSuspended=true;
            //if(flightData.isRefuelled==true) //refuel will trigger to notify user of the changes, but no alert message and no suspend!
            //    flightData.isSuspended=true;
            if(flightData.simRate>1)
            {
                flightData.isSuspended=true;
                wrn("SIMRATE: "+QS2F(flightData.simRate,1));
            }
        }

        if(!flightData.isSuspended)
        {
            bool updateWP=false;

            if(time(nullptr)-waypointTimer>wayPointTrackLowSec) //low time update ...
                updateWP=true;
            if(greatcircle(flightData.coordsCURR, lastCoords)<0.1) //but only if moved!
                updateWP=false;

            if(time(nullptr)-waypointTimer>wayPointTrackHighSec) //high time update ...
            {
                if(fabs(flightData.HDG-lastHDG)>5)               //only if nose has moved
                   updateWP=true;
            }

            if(updateWP)
            {
                dbg("adding waypoint ...");
                mapserver->addWaypoint(flightData.coordsCURR.first, flightData.coordsCURR.second);

                // ---
                lastCoords.first=flightData.coordsCURR.first;
                lastCoords.second=flightData.coordsCURR.second;
                lastHDG=flightData.HDG;
                waypointTimer=time(nullptr); //update time if fuel is ok
            }
        }

        {   //// ~~~~ Times accumulate
            if(flightData.isSuspended==true)
            {
                flightData.timePause_secs += (diff_msec/*ms*//(float)1000);
                dbg("pauseTime increased to "+QS2F(flightData.timePause_secs,2)+ "secs ...");
            }
            else
            {
                flightData.timeBlock_secs += (diff_msec/*ms*//(float)1000);
                if (flightData.AGL_ft > MIN_AGL_FT)
                    flightData.timeFlight_secs += (diff_msec/*ms*//(float)1000);
            }

            //Takeoff Time Limiter
            if(!mIsTimeSet(Takeoff) && flightData.timeBlock_secs>TIME_TO_TAKEOFF_SECS)
            {
                if(time(nullptr)-limitDelayTimer>limitReportDelaySecs)
                {
                    flightData.timeRemoved_secs+=flightData.timeBlock_secs-TIME_TO_TAKEOFF_SECS;
                    flightData.timeBlock_secs=TIME_TO_TAKEOFF_SECS;
                    wrn("Limited BlockTime(@Takeoff) to "+QS2F(flightData.timeBlock_secs/60,1)+" mins (removed "+QS2F(flightData.timeRemoved_secs/60,1)+" mins in total)");

                    limitDelayTimer=time(nullptr);
                }
            }
            if (!mIsTimeSet(Takeoff) && flightData.AGL_ft > MIN_AGL_FT)
                flightData.timeTakeoff_secs=flightData.timeBlock_secs;

            //Landing Time Limiter
            if (mIsTimeSet(Takeoff) && flightData.AGL_ft < MIN_AGL_FT)
            {
                if(!mIsTimeSet(Landing))
                    flightData.timeLanding_secs=flightData.timeBlock_secs;

                if(flightData.timeBlock_secs>flightData.timeLanding_secs+TIME_AFTER_LANDING_SECS)
                {
                    if(time(nullptr)-limitDelayTimer>limitReportDelaySecs)
                    {
                        flightData.timeRemoved_secs+=flightData.timeBlock_secs-(flightData.timeLanding_secs+TIME_AFTER_LANDING_SECS);
                        flightData.timeBlock_secs=flightData.timeLanding_secs+TIME_AFTER_LANDING_SECS;
                        wrn("Limited BlockTime(@Landing) to "+QS2F(flightData.timeBlock_secs/60,1)+" mins (removed "+QS2F(flightData.timeRemoved_secs/60,1)+" mins in total)");

                        limitDelayTimer=time(nullptr);
                    }
                }
            }
            else //reset landing time if taking off AGAIN after landing
                flightData.timeLanding_secs=-1;

        }


        {   //// ~~~~ Distances accumulate
            double nmPerSec=tickDist/diff_msec/1000;
            if(flightData.isSuspended==false)
            {
                //max: Concorde 1300kts => /60min/60secs => 0,36 nm/s
                if(nmPerSec>0.36 || diff_msec>20000 || tickDist>7.2)
                {
                    //wrn("WARP DETECTED: you moved "+QS2F(tickDist,4)+" nm with a speed of +"+QS2I(nmPerSec*60*60)+" kts ("+QS2F(nmPerSec,3)+" nm/s) within the last "+QS2I(diff_msec)+" msecs!");
                    wrn("WARP DETECTED: you moved "+QS2F(tickDist,4)+" nm within the last "+QS2I(diff_msec)+" msecs!");
                }
                else
                {
                    flightData.distance_nm += tickDist;
                    //dbg("Distance of "+QS2F(tickDist,4)+" nm for "+QS2I(diff_msec)+" ms accumulated: speed "+QS2F(nmPerSec*60*60,3)+" kts");
                    dbg("Distance of "+QS2F(tickDist,4)+" nm for "+QS2I(diff_msec)+" ms accumulated");
                }
            }
        }

    } //end only if flight tracking

    mapserver->setCurrent(&flightData);

    //set/remember new sim fuel
    flightData.FUELsimCur_lb=flightData.FUELsimNew_lb;
    //set/remember new coords
    flightData.coordsLAST.first=flightData.coordsCURR.first;
    flightData.coordsLAST.second=flightData.coordsCURR.second;

    if(flightData.GS_kts < 5.0 && flightData.AGL_ft < 10.0)
    {
        flightData.inhibitFlightNotStartedWarning=false;
        flightData.inhibitFlightNotStartedWarnSnd=false;
    }
}
