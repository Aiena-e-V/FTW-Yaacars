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
#define LOGGER_SECTION "XPUDP"

void MainWindow::closeXPUDP()
{
    flightData.connState=CS_UNKNOWN; //to update the gui button!
    //sock->close(); //close any prev connections
}
void MainWindow::initXPUDP()
{
    closeXPUDP();
    closeFSUIPC();

    //if (!sock->bind(32123))
    //{
    //    QMessageBox::critical(this, "YAACARS - Error", "Error: Could not bind to port 32123.", QMessageBox::Ok);
    //    return;
    //}

    char buffer[1024];
    char* pos = buffer;
    memcat(&pos, "DSEL", 5);
    //memcat(&pos, 1); // times
    memcat(&pos, 3); // speeds
    memcat(&pos, 4); // Mach, VVI, G-load
    memcat(&pos, 5); // atmosphere: weather
    memcat(&pos, 6); // atmosphere: aircraft
    memcat(&pos, 7); // system pressures
    memcat(&pos, 13); // trim/flap/slat/s-brakes
    memcat(&pos, 14); // gear/brakes
    memcat(&pos, 17); // pitch, roll, headings
    memcat(&pos, 20); // lat, lon, altitude
    memcat(&pos, 21); // loc, vel, dist traveled
    memcat(&pos, 41); // N1
    memcat(&pos, 42); // N2
    memcat(&pos, 45); // FF
    memcat(&pos, 63); // payload weights and CG
    memcat(&pos, 66); // landing gear vert force
    memcat(&pos, 106); // switches 1: electrical
    memcat(&pos, 114); // annunciators: general #2
    memcat(&pos, 127); // warning status
    sock->writeDatagram(buffer, (pos-buffer), remoteip, 49000);

    char addr[16];
    memset(addr, 0, 16);
    strncpy(addr, myip.toString().toUtf8().data(), 16);
    memset(buffer, 0, 1024);
    pos = buffer;
    memcat(&pos, "ISET", 5);
    memcat(&pos, 64);
    memcat(&pos, addr, 16);
    memcat(&pos, "32123", 6); pos += 2;
    memcat(&pos, 1);
    sock->writeDatagram(buffer, (pos-buffer), remoteip, 49000);

    sendDRef(sock, "sim/flightmodel/forces/fnrml_gear", 1);
    sendDRef(sock, "sim/time/sim_speed", 2);
    sendDRef(sock, "sim/time/ground_speed", 3);
}

void MainWindow::parseXPUDP()
{
    static bool posUpdate=false;
    //static double prevOverspeed = 0.0, prevStall = 0.0;
    static unsigned int simRateSim=0, SimRateGnd=0;

    //static double realTimeNow=0, realTimeLast=0;

    qint32 len;
    char buffer[2048]; // I think the max UDP datagram size is 924 or so, so this should be plenty.
    char* ptr;
    char msg[5] = "    ";
    int type;
    float val[8];

    while (sock->hasPendingDatagrams())
    {
        len = sock->readDatagram(buffer, 2048);

        if(!isCommXPUDP()) //read buffer, but don't parse messages, if not XPUDP
            continue;

        memcpy(msg, buffer, 4);
        if (!memcmp(msg, "DATA", 4))
        {
            flightData.timeStamp = time(NULL);
            for (ptr = buffer+5; ptr < buffer+len; ptr += 36)
            {
                memcpy(&type, ptr, 4);
                memcpy(val, ptr+4, 8*sizeof(float));

                switch (type)
                {
                        //         0     1     2     3     4     5     6     7
                case 1: // times:  real  totl  missn timer ----- zulu  local hobbs
                    //realTimeNow = float2double(val[0]);
                    break;

                case 3: // speeds: kias  keas  ktas  ktgs  ----- mph   mphas mphgs
                    flightData.IAS_kts = float2double(val[0]);
                    flightData.TAS_kts = float2double(val[2]);
                    flightData.GS_kts = float2double(val[3]);
                    break;

                case 4: //         mach  ----- fpm   ----- Gnorm Gaxil Gside -----
                    flightData.VS_fpm = float2double(val[2]);
                    //flightData.g = fabs(float2double(val[4])) + fabs(float2double(val[5])) + fabs(float2double(val[6]));
                    break;

                case 5: //        SLprs SLtmp ----- wnspd wndir trb   prec  hail
                    //flightData.qnhReal = float2double(val[0]);
                    flightData.windknots = float2double(val[3]);
                    flightData.winddeg = float2double(val[4]);
                    break;

                case 6: //        AMprs AMtmp LEtmp densr Aktas Qpsf  ----- gravi
                    flightData.oat = float2double(val[1]);
                    break;

                case 7: //        baro  edens vacum vacum elec  elec  ahrs  ahrs
                    //flightData.qnhSet = float2double(val[0]);
                    break;

                case 13: //        telev talrn trudr fhndl fposn srtio sbhdl sbpos
                    //flightData.flaps = float2double(val[4])*100.0;
                    break;

                case 14: //        gear  wbrak lbrak rbrak
                    //flightData.gear = float2double(val[0]) > 0.5;
                    break;

                case 17: //        pitch roll  hdt   hdm
                    flightData.pitch = float2double(val[0]);
                    flightData.bank = float2double(val[1]);
                    flightData.HDG = float2double(val[3]);
                    break;

                case 20: //        latd  lond  altsl altgl runwy aind lats  lonw
                    // Travelling 0.1 nmi in 1/20th of a second would be about mach 10, so fairly safe call it a slew.
                    //if (greatcircle(flightData.lat, flightData.lon, float2double(val[0], float2double(val[1]) > 0.1 && flightState >= FS_PREFLIGHT && flightData.lat != 0.0 && flightData.lon != 0.0)
                    //    critSlew(greatcircle(flightData.lat, flightData.lon, float2double(val[0], float2double(val[1]));
                    flightData.coordsCURR.first = float2double(val[0]);
                    flightData.coordsCURR.second= float2double(val[1]);
                    flightData.ASL_ft           = float2double(val[2]);
                    flightData.AGL_ft           = float2double(val[3]);
                    //flightData.onRwy = (float2double(val[4]) != 0.0);

                    if(hwTimer->elapsed()>HW_CYCLE_TIME) //trigger computation only each second
                        posUpdate=true;
                    break;

                case 21: //        x     y     z     vX    vY    vZ   dstft dstnm
                    //NOW USED FOR ACCUMULATING DISTANCE - DON'T USE IT HERE AYNMORE! flightData.distance = float2double(val[7];
                    break;

                case 34: // engine power
                    break;

                case 41: //N1
                    //for (int x = 0; x < 8; x++)
                    //    flightData.engN1[x]=float2double(val[x]);
                    break;
                case 42: //N2
                    //for (int x = 0; x < 8; x++)
                    //    flightData.engN2[x]=float2double(val[x]);
                    break;

                case 45: // FF
                    //for (int x = 0; x < 8; x++)
                    //{
                    //    //update cur.engine values for inFlight events, if out of FS_PREFLIGHT
                    //    if (acarsState==AS_TRACKING && flightState >= FS_PREFLIGHT)
                    //    {
                    //        if ((float2double(val[x]) > 0.0) && !flightData.engON[x]) evtEngineStart(x);
                    //        else if ((float2double(val[x]) == 0.0) && flightData.engON[x]) evtEngineStop(x);
                    //    }
                    //}
                    break;

                case 63: // payload weights and CG
                    //0:empty | 1:payload | 2:fuel | 3:jetti | 4:curnt | 5:maxim | 6:- | 7:cg
                    //flightData.ZFW_lb = float2double(val[0])+float2double(val[1]);
                    flightData.PAYL_lb = float2double(val[1]);
                    flightData.FUELsimNew_lb = float2double(val[2]);
                    dbg("Received Fuel(lb): "+QS2F(flightData.FUELsimNew_lb,3));
                    break;

                case 66: // landing gear vert force
                    //if (acarsState==AS_TRACKING && flightState >= FS_CLIMB && flightState <= FS_DESCEND)
                    //{
                    //    bool found = false;
                    //    for (int x = 0; x < 8; x++)
                    //    {
                    //        if (float2double(val[x]) > 0.0) found = true;
                    //    }
                    //    if (found) evtLanding();
                    //}
                    break;

                case 106: // switches 1: electrical
                    //flightData.nav = float2bool(val[1]);
                    //flightData.bea = float2bool(val[2]);
                    //flightData.str = float2bool(val[3]);
                    //flightData.ldn = float2bool(val[4]);
                    //flightData.txi = float2bool(val[5]);
                    break;

                case 114: // annunciators: general #2
                    //if (acarsState==AS_TRACKING && float2double(val[7]) > prevOverspeed)
                    //{
                    //    //critOverspeed();
                    //    //prevOverspeed = float2double(val[7]);
                    //}
                    break;

                case 127: //
                    //if (acarsState==AS_TRACKING && float2double(val[6]) > prevStall)
                    //{
                    //    //critStall();
                    //    //prevStall = float2double(val[6]);
                    //}
                    break;

                default:
                    //Got packet with unexpected type, ignoring!
                    break;
                }
            }
        }
        else if (!memcmp(msg, "RREF", 4))
        {
            for (ptr = buffer+5; ptr < buffer+len; ptr += 8)
            {
                memcpy(&type, ptr, 4);
                memcpy(val, ptr+4, 4);

                switch (type)
                {
                case 1: // sim/flightmodel/forces/fnrml_gear
                    //if (acarsState==AS_TRACKING && flightState >= FS_CLIMB && flightState <= FS_DESCEND && float2double(val[0]) > 0.0)
                    //    evtLanding();
                    flightData.rref = time(NULL); // Keep this in the most-recently-added packet type.
                    break;

                case 2: // sim/time/sim_speed
                    simRateSim=float2double(val[0]);
                    flightData.simRate=simRateSim*SimRateGnd;
                    break;

                case 3: // sim/time/ground_speed
                    SimRateGnd=float2double(val[0]);
                    flightData.simRate=simRateSim*SimRateGnd;
                    break;

                default:
                    qInfo("Unexpected RREF type %d", type);
                    sendDRef(sock, "", type, 0);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////
        // EVENTS
        /////////////////////////////////////////////////////////////////////////

        //if(acarsState==AS_TRACKING)
        //{
        //    //not working for FSUIPC!
        //    if ((flightState == FS_TAXITORWY || flightState == FS_TAXITOGATE) && flightData.GS_kts > 25.0 && !flightData.onRwy)
        //        critTaxiSpeed(flightData.GS_kts);
        //}

        if(posUpdate==true)
        {
            calcFlightData(hwTimer->elapsed());
            posUpdate=false;
            //realTimeLast=realTimeNow;
            hwTimer->start();
        }

    } //endWhile
}
