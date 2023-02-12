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

#ifdef _WIN32
    #include <windef.h>
    #include "src/lib/uipc/FSUIPC_User.h"
#endif
//////////////////////////////////////////////////////////////////////////////////////
#define LOGGER_SECTION "FSUIPC"

void MainWindow::closeFSUIPC()
{
    flightData.connState=CS_UNKNOWN; //to update the gui button!
#ifdef _WIN32
    FSUIPC_Close();
#endif
    //ui->conSim->setStyleSheet("QPushButton { color: "+mRGB_red+"; }");
}

void MainWindow::initFSUIPC()
{
    closeFSUIPC();
    closeXPUDP();

#ifdef _WIN32
    DWORD pdwResult; //DWORD NOT DEFINED FOR LIN/MAC!

    if(FSUIPC_Open(SIM_ANY, &pdwResult)==true)
    {
        log("FSUIPC-Connect succeed");
        //ui->conSim->setStyleSheet("QPushButton { color: "+mRGB_grn+"; }");
    }
    else
#endif
    {
        err("FSUIPC-Connect failed!"); //with "+QString::number(pdwResult));
        //ui->conSim->setStyleSheet("QPushButton { color: "+mRGB_red+"; }");
    }
}

typedef signed char      sInt8;
typedef signed short     sInt16;
typedef signed long      sInt32;
typedef unsigned long    uInt32;
typedef signed long long sInt64;

#define FSUIPC_DEF(a,p) \
    FSUIPC_Read(a, sizeof(p), &p, &pdwResult)


void MainWindow::parseFSUIPC()
{
#ifdef _WIN32
    DWORD pdwResult; //DWORD NOT DEFINED FOR LIN/MAC!

    if (!isCommFSUIPC())
        return;

    //if(connState!=CS_CONNECTED)
    //    return;

    //dbg("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");

    /////////////////////////////////////////////////////////

    sInt16 F_pause;         FSUIPC_DEF(0x0264, F_pause);
    sInt16 F_simRate;       FSUIPC_DEF(0x0C1A, F_simRate);      // Simulation rate *256 (i.e. 256=1x)

    //sInt16 F_lights;        FSUIPC_DEF(0x0D0C, F_lights);
    sInt16 F_onGround;      FSUIPC_DEF(0x0366, F_onGround);     //Aircraft on ground flag (0=airborne, 1=on ground. Not updated in Slew mode)

    //sInt32 F_flaps;         FSUIPC_DEF(0x0BDC, F_flaps);        //0BDC  4  Flaps  control,  0=up,  16383=full.  The  “notches”  for  different  aircraft  are  spaced  equally  across  this  range:  calculate  the  increment by 16383/(number of positions-1), ignoring fractions.

    //sInt32 F_zfw;           FSUIPC_DEF(0x3BFC, F_zfw);
    //sInt32 F_EmptyWeight;   FSUIPC_DEF(0x1330, F_EmptyWeight);
    double F_payload;       FSUIPC_DEF(0x30C0, F_payload);      //Current  loaded  weight  in  lbs.  This  is  in  double  floating  point format (FLOAT64). [FS2000 and later]

    sInt64 F_lat;           FSUIPC_DEF(0x0560, F_lat);
    sInt64 F_lon;           FSUIPC_DEF(0x0568, F_lon);

    sInt32 F_altmsl_mtr;    FSUIPC_DEF(0x0574, F_altmsl_mtr);
    sInt16 F_altagl_mtr;    FSUIPC_DEF(0x0B4C, F_altagl_mtr);   //0B4C 2 Ground altitude (metres). See 0020 for more accuracy.

    //sInt16 F_qnh_real;      FSUIPC_DEF(0x0EC6, F_qnh_real);     //0EC6  2  Pressure (QNH) as millibars (hectoPascals) *16.
    //sInt16 F_qnh_set;       FSUIPC_DEF(0x0330, F_qnh_set);      //0330  2  Altimeter  pressure  setting  (“Kollsman”  window).  As  millibars (hectoPascals) * 16

    //double F_hdg_deg;       FSUIPC_DEF(0x02CC, F_hdg_deg);
    sInt32 F_pitch_deg;     FSUIPC_DEF(0x0578, F_pitch_deg);    //Pitch,   *360/(65536*65536)  for  degrees.  0=level,  –ve=pitch  up, +ve=pitch down
    sInt32 F_bank_deg;      FSUIPC_DEF(0x057C, F_bank_deg);     //Bank,    *360/(65536*65536) for degrees. 0=level, –ve=bank right, +ve=bank left
    sInt32 F_hdg_deg;       FSUIPC_DEF(0x0580, F_hdg_deg);      //Heading, *360/(65536*65536) for degrees TRUE.
    sInt16 F_oat_deg;       FSUIPC_DEF(0x0E8C, F_oat_deg);      //Outside Air Temperature (OAT), degrees C * 256

    //sInt16 F_g;             FSUIPC_DEF(0x11BA, F_g);            //TODO ---> 11B8 G Force: copy of 11BA on touchdown.
    sInt32 F_vs;            FSUIPC_DEF(0x02C8, F_vs);

    sInt32 F_ias_kts;       FSUIPC_DEF(0x02BC, F_ias_kts);
    sInt32 F_tas_kts;       FSUIPC_DEF(0x02B8, F_tas_kts);
    sInt32 F_gs_mps;        FSUIPC_DEF(0x02B4, F_gs_mps);

    double F_ff_pph[4];     FSUIPC_DEF(0x0918, F_ff_pph[0]);
                            FSUIPC_DEF(0x09B0, F_ff_pph[1]);
                            FSUIPC_DEF(0x0A48, F_ff_pph[2]);
                            FSUIPC_DEF(0x0AE0, F_ff_pph[3]);

    uInt32 F_fuel_lvls[11];
    FSUIPC_DEF(0x0B74, F_fuel_lvls[ 0]); // Fuel: centre tank level, % * 128 * 65536  Ok  Ok
    FSUIPC_DEF(0x0B7C, F_fuel_lvls[ 1]); // Fuel: left main tank level, % * 128 * 65536  Ok  Ok
    FSUIPC_DEF(0x0B84, F_fuel_lvls[ 2]); // Fuel: left aux tank level, % * 128 * 65536  Ok  Ok
    FSUIPC_DEF(0x0B8C, F_fuel_lvls[ 3]); // Fuel: left tip tank level, % * 128 * 65536  Ok  Ok
    FSUIPC_DEF(0x0B94, F_fuel_lvls[ 4]); // Fuel: right main tank level, % * 128 * 65536  Ok  Ok
    FSUIPC_DEF(0x0B9C, F_fuel_lvls[ 5]); // Fuel: right aux tank level, % * 128 * 65536
    FSUIPC_DEF(0x0BA4, F_fuel_lvls[ 6]); // Fuel: right tip tank level, % * 128 * 65536  Ok  Ok
    FSUIPC_DEF(0x1244, F_fuel_lvls[ 7]); // Fuel: centre 2 tank level, % * 128 * 65536 [FS2k/CFS2 only]  Ok  Ok
    FSUIPC_DEF(0x124C, F_fuel_lvls[ 8]); // Fuel: centre 3 tank level, % * 128 * 65536 [FS2k/CFS2 only]  Ok  Ok
    FSUIPC_DEF(0x1254, F_fuel_lvls[ 9]); // Fuel: external 1 tank level, % * 128 * 65536 [FS2k/CFS2 only]  Ok  Ok
    FSUIPC_DEF(0x125C, F_fuel_lvls[10]); // Fuel: external 2 tank level, % * 128 * 65536 [FS2k/CFS2 only]  Ok  Ok

    uInt32 F_fuel_gals[11];
    FSUIPC_DEF(0x0B78, F_fuel_gals[ 0]); // Fuel: centre  tank  capacity:  US  Gallons  (see  also  offsets  1244–
    FSUIPC_DEF(0x0B80, F_fuel_gals[ 1]); // Fuel: left main tank capacity: US Gallons  Ok  Ok
    FSUIPC_DEF(0x0B88, F_fuel_gals[ 2]); // Fuel: left aux tank capacity: US Gallons  Ok  Ok
    FSUIPC_DEF(0x0B90, F_fuel_gals[ 3]); // Fuel: left tip tank capacity: US Gallons  Ok  Ok
    FSUIPC_DEF(0x0B98, F_fuel_gals[ 4]); // Fuel: right main tank capacity: US Gallons  Ok  Ok 0B9C  4  Fuel: right aux tank level, % * 128 * 65536  Ok  Ok
    FSUIPC_DEF(0x0BA0, F_fuel_gals[ 5]); // Fuel: right aux tank capacity: US Gallons  Ok  Ok
    FSUIPC_DEF(0x0BA8, F_fuel_gals[ 6]); // Fuel: right tip tank capacity: US Gallons
    FSUIPC_DEF(0x1248, F_fuel_gals[ 7]); // Fuel: centre 2 tank capacity: US Gallons [FS2k/CFS2 only]  Ok  Ok
    FSUIPC_DEF(0x1250, F_fuel_gals[ 8]); // Fuel: centre 3 tank capacity: US Gallons [FS2k/CFS2 only]  Ok  Ok
    FSUIPC_DEF(0x1258, F_fuel_gals[ 9]); // Fuel: external 1 tank capacity: US Gallons [FS2k/CFS2 only]  Ok  Ok
    FSUIPC_DEF(0x1260, F_fuel_gals[10]); // Fuel: external 2 tank capacity: US Gallons [FS2k/CFS2 only]

    //FSUIPC_Read(0x313C, 12, acReg, &pdwResult);
    //FSUIPC_Read(0x3D00, 256, acTyp, &pdwResult);

    if(FSUIPC_Process(&pdwResult)==TRUE)
    {
        flightData.timeStamp=flightData.rref=time(NULL);

        //flightData.qnhReal=((double)F_qnh_real)*mulhPAmmHG/16;
        //flightData.qnhSet=((double)F_qnh_set)*mulhPAmmHG/16;

        flightData.simRate=F_simRate/256;
        if (F_pause>0)
            flightData.simRate=0; //pause

        flightData.coordsCURR.first =F_lat * 90.0/(10001750.0 * 65536.0 * 65536.0);
        flightData.coordsCURR.second=F_lon * 360.0/(65536.0 * 65536.0 * 65536.0 * 65536.0);

        flightData.ASL_ft=F_altmsl_mtr*mulMTR2FT;
        flightData.AGL_ft=flightData.ASL_ft-(F_altagl_mtr*mulMTR2FT);

        flightData.pitch=(double)F_pitch_deg*360/65536/65536;
        flightData.bank=(double)F_bank_deg*360/65536/65536;
        flightData.HDG=(double)F_hdg_deg*360/65536/65536;
        if(flightData.HDG<0) flightData.HDG+=360;
        //dbg("HDG: "+QS2I(flightData.HDG)+"° BANK:"+QS2I(flightData.bank)+"° PITCH:"+QS2I(flightData.pitch)+"°");
        flightData.oat=F_oat_deg/256;

        flightData.IAS_kts=F_ias_kts/128;
        flightData.TAS_kts=F_tas_kts/128;
        flightData.GS_kts=F_gs_mps*mulMPS2KTS/65536;

        //flightData.g=F_g/625;
        flightData.VS_fpm=F_vs*mulMPS2FPS/256; //VS per sec (in meters)

        //flightData.flaps=(double)F_flaps*100/16383;

        //flightData.nav=(F_lights& 1)!=0 ? true:false;
        //flightData.bea=(F_lights& 2)!=0 ? true:false;
        //flightData.ldn=(F_lights& 4)!=0 ? true:false;
        //flightData.txi=(F_lights& 8)!=0 ? true:false;
        //flightData.str=(F_lights&16)!=0 ? true:false;
        //if(flightData.nav) dbg("Light: Nav");
        //if(flightData.bea) dbg("Light: Beacon");
        //if(flightData.ldn) dbg("Light: Landing");
        //if(flightData.txi) dbg("Light: Taxi");
        //if(flightData.str) dbg("Light: Strobe");

        // --- FUEL
        double totalFuel=0, fuel, fuelCapa, fuelPerc;
        for(int i=0;i<=10;i++)
        {
            fuelPerc=(double)F_fuel_lvls[i] / 133.49 / 65536; //mit 133 (statt 128) kommt es ca hin, aber bei 100kg=>97 und bei 2500=>2506 ???
            fuelCapa=F_fuel_gals[i] * mulGAL2KG;
            fuel=fuelCapa*fuelPerc;
            totalFuel+=fuel;
            if(fuelCapa>0) dbg("Tank "+QS2I(i)+": CAP["+QS2I(F_fuel_gals[i])+" kg] * "+QS2I(fuelPerc*100)+"% = "+QS2I(fuel)+" kg");
        }
        flightData.FUELsimNew_lb=totalFuel * mulKG2LB;

        //flightData.ZFW_lb=F_zfw/256; //Zero Fuel Weight, lbs * 256

        flightData.PAYL_lb=F_payload;
        if(flightData.PAYL_lb<0) flightData.PAYL_lb=0;

        /////////////////////////////////////////////////////////////////////////
        // EVENTS
        /////////////////////////////////////////////////////////////////////////

        //if(acarsState==AS_TRACKING)
        //{
        //    // --- ENGINES
        //    for(int x=0;x<=3;x++)
        //    {
        //        //log("F_ff_pph["+QS2I(x)+"] "+QS2F(F_ff_pph[x],3));
        //        if (flightState >= FS_PREFLIGHT)
        //        {
        //            if (     (F_ff_pph[x] >  0.0) && !flightData.engON[x]) evtEngineStart(x);
        //            else if ((F_ff_pph[x] == 0.0) &&  flightData.engON[x]) evtEngineStop(x);
        //        }
        //    }
        //
        //    // --- LANDING
        //    if (flightState >= FS_CLIMB && flightState <= FS_DESCEND)
        //    {
        //        if (F_onGround)
        //            evtLanding();
        //    }
        //}

        if(hwTimer->elapsed()>HW_CYCLE_TIME) //trigger computation only each second
        {
            calcFlightData(hwTimer->elapsed());
            hwTimer->start();
        }

////////////////////////////////////////////////////////////////////////////////

        //not supported/implemented:
        flightData.winddeg=flightData.windknots=0;
        //double engN1[8];
        //double engN2[8];
        //flightData.onRwy=1;

        //critTaxiSpeed(flightData.gs); not working for FSUIPC, no onRwy flag

        //critOverspeed();
        //critStall();
        //critSlew();
    }
    else //probably the sim is busy or gone ...
    {
        closeFSUIPC();
    }
#endif
}
