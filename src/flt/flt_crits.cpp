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
#define LOGGER_SECTION "CRIT"

//void MainWindow::critOverspeed()
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="You're trying to break the sound barrier, don't you?";
//    if (last < time(NULL) - 10) newEvent(message, true);
//    last = time(NULL);
//
//    mistakes.overspeed = 1;
//}
//
//void MainWindow::critStall()
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="Water U doing, mate? We're about to stall!";
//    if (last < time(NULL) - 10) newEvent(message, true);
//    last = time(NULL);
//
//    mistakes.stall = 1;
//}
//
//void MainWindow::critPaused()
//{
//    newEvent("PAUSED");
//    mistakes.pause = 1;
//    va_report("PAUSED");
//}
//void MainWindow::critUnpaused()
//{
//    newEvent("UNPAUSED");
//    va_report("UNPAUSED");
//}
//
//void MainWindow::critBeaconOff()
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="I've learned that the BEACON has to be ON before engine start, but I could be wrong.";
//    if (last < time(NULL) - 10) newEvent(message,true);
//    last = time(NULL);
//
//    mistakes.beaconOff = true;
//}
//
//void MainWindow::critIasBelow10k()
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="Wasn't there a rule to fly less than 250 kts below 10000 ft? But I can't recall exactly.";
//    //"("+QS2F(flightData.IAS_kts,0)+" kt at "+QS2F(flightData.ASL_ft,0)+" ft)")
//    if (last < time(NULL) - 10) newEvent(message, true);
//    last = time(NULL);
//
//    mistakes.iasLow = true;
//}
//
//void MainWindow::critLightsBelow10k()
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="Doesn't sound like a good idea to turn landing lights off below 10000 ft.";
//    if (last < time(NULL) - 10) newEvent(message, true);
//    last = time(NULL);
//
//    mistakes.lightsLow = true;
//}
//
//void MainWindow::critLightsAbove10k()
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="Do you want to leave the landing lights on forever? We're already way above 10000 ft!";
//    if (last < time(NULL) - 10) newEvent(message, true);
//    last = time(NULL);
//
//    mistakes.lightsHigh = true;
//}
//
//void MainWindow::critTaxiNoLights()
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="Could you explain to me, what this taxi light thingy is for? Do you ever use it?";
//    if (last < time(NULL) - 10) newEvent(message, true);
//    last = time(NULL);
//
//    mistakes.taxiLights = true;
//}
//
//void MainWindow::critTakeoffNoLights()
//{
//    QString message="";
//    message+="They are called landing lights, but I'm sure they are also good for takeoff!";
//    newEvent(message, true);
//
//    mistakes.takeoffLights = true;
//}
//
//void MainWindow::critLandingNoLights()
//{
//    QString message="";
//    message+="Well, let's hope you will use the landing lights at least for landings at night!";
//    newEvent(message, true);
//
//    mistakes.landingLights = true;
//}
//
//void MainWindow::critTaxiSpeed(double speed)
//{
//    static qint64 last = 0;
//    QString message="";
//    message+="Are you really sure you're allowed to drive "+QS2I(speed)+" kts on the taxiway?";
//    if (last < time(NULL) - 10) newEvent(message, true);
//    last = time(NULL);
//
//    mistakes.taxiSpeed = true;
//}
//
//void MainWindow::critQnhTakeoff()
//{
//    QString message="";
//    message+="Is your altimeter set to "+QS2F(flightData.qnhSet,2)+"? Didn't the tower told you "+QS2F(flightData.qnhReal,2)+" instead?";
//    newEvent(message, true);
//
//    mistakes.qnhTakeoff = true;
//}
//
//void MainWindow::critQnhLanding()
//{
//    QString message="";
//    message+="Is your altimeter set to "+QS2F(flightData.qnhSet,2)+"? Didn't the tower told you "+QS2F(flightData.qnhReal,2)+"?";
//    newEvent(message, true);
//
//    mistakes.qnhLanding = true;
//}
//
//void MainWindow::critCrashed(QString reason)
//{
//    if (flightState <= FS_PREFLIGHT || mistakes.crash) return;
//
//    QString message="";
//    message+="WTF? WE CRASHED, because of "+reason;
//    newEvent(message, true);
//
//    mistakes.crash = true;
//}
