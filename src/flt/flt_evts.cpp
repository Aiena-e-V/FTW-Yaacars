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
#define LOGGER_SECTION "EVTs"

//void MainWindow::newEvent(QString desc, bool critical)
//{
//    if(acarsState!=AS_TRACKING)
//        return;
//
//    if (critical)
//    {
//        critEvents++;
//        ui->critEvents->append(cpName+" said at "+QTime::currentTime().toString("hh:mm:ss")+": "+desc);
//        log("COCO: "+desc);
//    }
//    else
//    {
//        ui->flightEvents->append(QTime::currentTime().toString("hh:mm:ss")+": "+desc);
//        log("Event: "+desc);
//    }
//
//    //va_newEvent(e);
//}

//void MainWindow::evtStartTaxi()
//{
//    //$$ if (flightDataOnEngOn.time == 0) flightDataOnEngOn = flightData;
//
//    flightState = FS_TAXITORWY;
//    newEvent("TAXI TO RWY");
//
//    if (!flightData.txi) critTaxiNoLights();
//}
//
//void MainWindow::evtTakeoff()
//{
//    //if not already set, set them now!
//    //$$ if (flightDataOnEngOn.time == 0) flightDataOnEngOn = flightData;
//    //$$ if (flightDataOnTakeoff.time == 0) flightDataOnTakeoff = flightData;
//
//    //check for maximum time to takeoff
//    //$$ if( flightDataOnTakeoff.time-flightDataOnStart.time>TIME_TO_TAKEOFF_SECS)
//    //$$ {
//    //$$     wrn("Time from Starting to Takeoff has been reduced from "+QS2I((flightDataOnTakeoff.time-flightDataOnStart.time)/60)+"min to "+QS2I(TIME_TO_TAKEOFF_SECS/60)+"min");
//    //$$     flightDataOnStart.time=flightDataOnTakeoff.time-TIME_TO_TAKEOFF_SECS;
//    //$$ }
//    //$$ if( flightDataOnTakeoff.time-flightDataOnEngOn.time>TIME_TO_TAKEOFF_SECS)
//    //$$ {
//    //$$     wrn("Time from EnginesOn to Takeoff has been reduced from "+QS2I((flightDataOnTakeoff.time-flightDataOnEngOn.time)/60)+"min to "+QS2I(TIME_TO_TAKEOFF_SECS/60)+"min");
//    //$$     flightDataOnEngOn.time=flightDataOnTakeoff.time-TIME_TO_TAKEOFF_SECS;
//    //$$ }
//    //$$
//    //$$ //reset landing values on takeoff (in case of re-takeoff)
//    //$$ if (flightDataOnLanding.time != 0) //has already been landed and takeoff again (event after crash?)
//    //$$     flightDataOnLanding.time=0; //reset the landing data to count blocktime again
//    //$$ if (flightDataOnEngOff.time != 0) //has already been landed and takeoff again (event after crash?)
//    //$$     flightDataOnEngOff.time=0; //reset the engOff data to count blocktime again
//    //$$ if (flightDataOnEnd.time != 0) //has already been landed and takeoff again (event after crash?)
//    //$$     flightDataOnEnd.time=0; //reset the end data to count blocktime again
//
//    flightState = FS_CRUISE;
//    newEvent("TAKEOFF");
//
//    if (!flightData.ldn) critTakeoffNoLights();
//    if (fabs(flightData.qnhReal - flightData.qnhSet) > 0.07) critQnhTakeoff();
//
//    va_report("TAKEOFF");
//}
//
//void MainWindow::evtStartClimb()
//{
//    flightState = FS_CLIMB;
//    newEvent("CLIMB");
//
//    va_report("CLIMB");
//}
//
//void MainWindow::evtCruise()
//{
//    flightState = FS_CRUISE;
//    newEvent("CRUISE");
//
//    va_report("CRUISE");
//}
//
//void MainWindow::evtStartDescend()
//{
//    flightState = FS_DESCEND;
//    newEvent("DESCEND");
//
//    va_report("DESCEND");
//}
//
//void MainWindow::evtLanding()
//{
//    if (flightData.VS_fpm < -650.0) critCrashed(QString("IMPACT AT "+QS2F(flightData.VS_fpm,0)+" FPM"));
//    else if (flightData.g > 15.0) critCrashed(QString("IMPACT AT "+QS2F(flightData.g,1)+" G"));
//    else if (flightData.onRwy==0) critCrashed("NOT ON RUNWAY");
//    else newEvent("LANDING");
//
//    if (!flightData.ldn) critLandingNoLights();
//    if (fabs(flightData.qnhReal - flightData.qnhSet) > 0.05) critQnhLanding();
//
//    va_uiState("LANDING");
//
//    float bfuel=flightData.FUELftwUsed_lb;
//    float ffuel=flightData.FUELftwUsed_lb;
//
//    ui->fsDepIcao->setText(ui->depIcao->text());
//    ui->fsArrIcao->setText(ui->arrIcao->text());
//    ui->fsDistance->setText(QS2F(flightData.distance_nm, 1)+" nmi");
//    ui->fsZFW->setText(ui->zfw->text()); //+" lb"
//    ui->fsCritEvents->setText(QString::number(critEvents));
//    QString times=  /*"Real "+QDateTime::fromTime_t(calcRealTime(), Qt::UTC).toString("hh:mm")+" // "+*/
//                    "Blk "+QDateTime::fromTime_t(calcBlockTime(), Qt::UTC).toString("hh:mm")+" / "+
//                    "Flt "+QDateTime::fromTime_t(calcFlightTime(), Qt::UTC).toString("hh:mm")+ " / "+
//                    "P +"+QDateTime::fromTime_t(calcPauseTime(), Qt::UTC).toString("hh:mm");
//    ui->fsDuration->setText(times);
//    ui->fsBFuel->setText(QS2F(bfuel,0)+" lb / "+QS2F(bfuel*mulLB2KG,0)+" kg");
//    ui->fsFFuel->setText(QS2F(ffuel,0)+" lb / "+QS2F(ffuel*mulLB2KG,0)+" kg");
//    ui->fsG->setText(QS2F(maximum.g,1)+" G");
//
//    //$$ ui->fsPitch->setText(QS2F(flightDataOnLanding.pitch,0)+"°");
//    //$$ ui->fsIAS->setText(QS2F(flightDataOnLanding.IAS_kts,1)+" kts");
//    //$$ ui->fsBank->setText(QS2F(flightDataOnLanding.bank,1)+"°");
//    //$$ ui->fsFlaps->setText(QS2F(flightDataOnLanding.flaps,0)+"%");
//    //$$ ui->fsVS->setText(QS2F(flightDataOnLanding.VS_fpm,0)+" fpm");
//    ui->fsPitch->setText("N/A");
//    ui->fsIAS->setText("N/A");
//    ui->fsBank->setText("N/A");
//    ui->fsFlaps->setText("N/A");
//    ui->fsVS->setText("N/A");
//
//    va_report("LANDING");
//}
//
//void MainWindow::evtShutdown()
//{
//    flightState = FS_POSTFLIGHT;
//    newEvent("DEBOARDING");
//    //$$ flightDataOnEngOff = flightData;
//
//    va_report("SHUTDOWN");
//    va_report("REPORT");
//}
//
//void MainWindow::evtEngineStart(int e)
//{
//    //newEvent(QString("STARTING ENGINE %1").arg(e+1));
//    //flightData.engON[e] = 1;
//    //
//    //if (flightState < FS_CLIMB && flightDataOnEngOn.time == 0)
//    //{
//    //    if (!flightData.bea) critBeaconOff();
//    //
//    //    int onCount = 0;
//    //    for (int x = 0; x < 8; x++)
//    //        if (flightData.engON[x] == 1) onCount++;
//    //    if (onCount == 1)
//    //    {
//    //        //$$ flightDataOnEngOn = flightData;
//    //        va_report("STARTUP");
//    //    }
//    //}
//}
//
//void MainWindow::evtEngineStop(int e)
//{
//    newEvent(QString("STOPPING ENGINE %1").arg(e+1));
//    flightData.engON[e] = 0;
//
//    if (flightState == FS_TAXITOGATE)
//    {
//        int onCount = 0;
//        for (int x = 0; x < 8; x++)
//            if (flightData.engON[x] == 1)
//                onCount++;
//        if (onCount == 0)
//            evtShutdown();
//    }
//}

