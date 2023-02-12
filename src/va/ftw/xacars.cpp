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

#define acarsIdVer QString("XACARS|2.0")

#define curl_fetch va_ftw_xacars_curl_fetch

//////////////////////////////////////////////////////////////////////////////////////
#define LOGGER_SECTION "VAxa"

QString MainWindow::va_ftw_xacars_login()
{
    //QString url = "liveacars.php";
    //QString qry = "DATA1="+acarsIdVer+"&DATA2=TEST&DATA3="+ui->username->text();
    //
    //vaLoggerReport("Login", qry);
    //
    // if(curl_fetch(url, qry, CURL_TIMEOUT_GENERAL_SECS)=="1")
    //     return "OK";
    // else
    //     return "FAIL";
}

QString MainWindow::va_ftw_xacars_getFlightplan()
{
    //QString url = "flightdata.php";
    //QString qry = "DATA1="+acarsIdVer+"&DATA2="+ui->flightNo->text()+"&DATA3=flightplan&DATA4="+ui->username->text()+"&DATA5="+ui->password->text();
    //
    //vaLoggerReport("GetFP", qry);
    //
    //QString data=curl_fetch(url, qry, CURL_TIMEOUT_GENERAL_SECS);
    //
    //QStringList query = data.split('\n');
    //QJsonObject json;
    //if(query[0] == "1|flightplan" && query.count()>=9)
    //{
    //    //departure   = query[1];
    //    //arrival     = query[2];
    //    //alternative = query[3];
    //    //route       = query[4];
    //    //pax   		= query[5];
    //    //cargo 		= query[6];
    //    //aircraft 	= query[8];
    //
    //    //int yn = QMessageBox::question(this, "YAACARS", "A flight plan was found. Load it?", QMessageBox::Yes, QMessageBox::No);
    //    //if (yn == QMessageBox::Yes)
    //    {
    //        ui->depIcao->setText(query[1]);
    //        ui->arrIcao->setText(query[2]);
    //        ui->altIcao->setText(query[3]);
    //        //ui->edtVatRoute->setText(query[4]);
    //        ui->pax_count->setText(query[5]);
    //        ui->pax_kg->setText(QS2F(ui->pax_count->text().toDouble()*kgPerPax,0));
    //        ui->pax_lb->setText(QS2F(ui->pax_kg->text().toDouble()*mulKG2LB,0));
    //        ui->cargo_kg->setText(query[6]);
    //        ui->cargo_lb->setText(QS2F(ui->cargo_kg->text().toDouble()*mulKG2LB,0));
    //        ui->acName->setText(query[8]);
    //        //not avail:
    //        //ui->flightNo->setText(data["callsign"].toString());
    //        //ui->depTime->setText(data["etd"].toString());
    //        //ui->acIcao->setText(data["plane_icao"].toString());
    //        //ui->eet->setText(data["duration"].toString());
    //        ui->tailNumber->setText("N-A"+QS2I(QRandomGenerator::global()->bounded(1111, 9999))); //pilot can override the data for own purposes
    //    }
    //    return "OK";
    //}
    //return "FAIL";
}

QString MainWindow::va_ftw_xacars_startFlight()
{
    //log("Sending StartFlight...");
    //
    //QString url = "liveacars.php";
    //QString qry;
    //
    //QString lat=latToXACARS(flightData.lat);
    //QString lon=lonToXACARS(flightData.lon);
    //
    //QString alt=QS2F(flightData.ASL_ft, 0);
    //QString fob=QS2F(flightData.FUEL_lb, 0);
    //
    //QString oat=QS2F(flightData.oat, 0);
    //QString hdg=QS2F(flightData.HDG, 0);
    //QString trk=hdg;
    //
    //// BEGINFLIGHT ////////////////////////////////////////////////////////////////////////////////
    ////http://www.xacars.net/index.php?Client-Server-Protocol
    //// - Username
    //// - [empty]
    //// - Flightnumber
    //// - Aircraft Type
    //// - [empty]
    //// - Flightplan (spaces are sent as "~")
    //// - Position (e.g. "N48 7.21916 E16 33.4283")
    //// - Altitude
    //// - [empty]
    //// - [empty]
    //// - [empty]
    //// - Fuel on board
    //// - Heading
    //// - Wind (e.g. "31012" for 310°, 12kts)
    //// - OAT
    //// - Flightrules (IFR or VFR)
    //// - Distance flown (0 at this time)
    //// - Password for the user
    //
    //
    //qry="DATA1="+acarsIdVer+"&DATA2=BEGINFLIGHT"+
    //"&DATA3="+ui->username->text()+"||"+ui->flightNo->text()+"|"+ui->acName->text()+"||"+ui->depIcao->text()+"~~"+ui->arrIcao->text()+
    //"|"+lat+" "+lon+"|"+alt+"||||"+fob+"|"+hdg+"|00000|"+oat+"|"+ui->flightType->currentText()+"|0|"+ui->password->text()+"|";
    //
    ////!!! BEGINFLIGHT returns the session ID (flightIdSrv) for this flight!
    //
    //vaLoggerReport("Start", qry);
    //
    //QString data=curl_fetch(url, qry, CURL_TIMEOUT_GENERAL_SECS);
    //
    //QStringList query = data.split('|');
    //trackingId="";
    //if(query.count()==2)
    //    if(query[0]=="1")
    //        trackingId=query[1];
    //if(query[0]=="0")
    //    return query[1];
    //
    //if(trackingId=="")
    //{
    //    err("StartFlight FAILED! '"+data+"'");
    //    return data;
    //}
    //
    //log("StartFlight sent. Tracking Number: "+trackingId);
    //
    //return "OK";
}

QString MainWindow::va_ftw_xacars_pause(int mode)
{
    //if(acarsState != AS_TRACKING)
    //    return "";
    //
    //
    //QString url = "liveacars.php";
    //QString qry;
    //
    //qry="DATA1="+acarsIdVer+"&DATA2=PAUSEFLIGHT&DATA3="+trackingId+"&DATA4="+QString::number(mode);
    //
    //vaLoggerReport("Pause", qry);
    //
    //if(trackingId!="")
    //    return curl_fetch(url, qry, CURL_TIMEOUT_LIVEUPDATE_SECS);
    //else
    //    return "";
}

QString MainWindow::va_ftw_xacars_report(QString eventType)
{
    //if(acarsState != AS_TRACKING)
    //    return true;
    //
    //if(eventType=="PAUSED")
    //{
    //    va_ftw_xacars_pause(1);
    //    return true;
    //}
    //if(eventType=="UNPAUSED")
    //{
    //    va_ftw_xacars_pause(0);
    //    return true;
    //}
    ////if(flightState<TAXITORWY || flightState>TAXITOGATE) //AR messages only in the air
    ////    if(eventType=="CRUISE"
    ////    || eventType=="DESCEND"
    ////    || eventType=="CLIMB")
    ////        return;
    //
    //dbg("Sending "+eventType+" update ... ");
    //
    //QString url = "liveacars.php";
    //QString qry;
    //
    ////--------------------------------------------------
    ////http://www.xacars.net/index.php?Messagetypes
    //
    //QString lat=latToXACARS(flightData.lat);
    //QString lon=lonToXACARS(flightData.lon);
    //
    //QString alt=QS2F(flightData.ASL_ft, 0);
    //QString fob=QS2F(flightData.FUEL_lb, 0);
    //
    //QString hdg=QS2F(flightData.HDG, 0);
    //QString oat=QS2F(flightData.oat, 0);
    //
    //QString wnd=QS2F(flightData.winddeg, 0).rightJustified(3, '0')+QS2F(flightData.windknots, 0).rightJustified(2, '0'); //dir and speed: dddss
    //
    //QDateTime dt=QDateTime::currentDateTime();
    //
    //QString zuluDate=dt.toUTC().toString("MM/dd/yyyy"); //"mm/dd/yyyy";
    //QString zuluTime=dt.toUTC().toString("hh:mm")+"Z";  //"xx:xxZ";
    //
    //QString ias=QS2F(fabs(flightData.IAS_kts), 0);
    //QString tas=QS2F(fabs(flightData.TAS_kts), 0);
    //QString weight=QS2F(flightData.ZFW_lb+flightData.FUEL_lb, 0);
    //QString zfw=QS2F(flightData.ZFW_lb, 0);
    //QString trk=hdg;
    //QString dst=QS2F(fabs(flightData.distance), 0);
    //
    //QString blockType=""; //"QA","QB","QC","QD","QM","AR","PR"
    //
    //if(eventType=="STARTUP") //engines running
    //    blockType="QA";
    //else if(eventType=="TAKEOFF") //takeoff
    //    blockType="QB";
    //else if(eventType=="LANDING") //touchdown
    //    blockType="QC";
    //else if(eventType=="SHUTDOWN") //engines off
    //    blockType="QD";
    //else if(eventType=="REPORT") //engines off
    //    blockType="QM";
    //else if(eventType=="CRUISE"
    //     || eventType=="DESCEND"
    //     || eventType=="CLIMB")
    //    blockType="AR";
    //else if(eventType=="POSITION") //Position report
    //    blockType="PR";
    //else
    //{
    //    err("--- Unknown EventType '"+eventType+"' in sendUpdate()! ---");
    //    return false;
    //}
    //
    //qry="DATA1="+acarsIdVer+"&DATA2=MESSAGE&DATA3="+trackingId+
    //"&DATA4=["+zuluDate+" "+zuluTime+"]\n"+
    //"ACARS Mode: 2 Aircraft Reg: ."+ui->tailNumber->text()+"\n"+
    //"Msg Label: "+blockType+" Block ID: 01 Msg No: M"+QString::number(blockNumber++).rightJustified(2, '0')+"A\n"+
    //"Flight ID: "+ui->flightNo->text()+"\n"+
    //"Message:\n";
    //
    //if(blockType=="QM")
    //{
    //    double bt=calcBlockTime();
    //    double ft=calcFlightTime();
    //    if(ft>bt) bt=ft;
    //    QString blockTime=QDateTime::fromTime_t(static_cast<unsigned int>(bt), Qt::UTC).toString("HH:mm");
    //    QString flightTime=QDateTime::fromTime_t(static_cast<unsigned int>(ft), Qt::UTC).toString("HH:mm");
    //    //QString blockTime=QDateTime::fromTime_t(flightDataOnEngOff.time-flightDataOnStart.time, Qt::UTC).toString("HH:mm");
    //    //QString flightTime=QDateTime::fromTime_t(flightDataOnLanding.time-flightDataOnTakeoff.time, Qt::UTC).toString("HH:mm");
    //
    //    QString blockFuel=QS2F(flightDataOnStart.FUEL_lb-flightDataOnEngOff.FUEL_lb, 0);
    //    QString flightFuel=QS2F(flightDataOnTakeoff.FUEL_lb-flightDataOnLanding.FUEL_lb, 0);
    //
    //    qry+="BLOCK TIME "+blockTime+" /FUEL "+blockFuel+"\n"+
    //         "FLIGHT TIME "+flightTime+" /FUEL "+flightFuel+"\n"+
    //         "FLIGHT DISTANCE "+dst+"\n";
    //}
    //else
    //{
    //
    //    if(blockType=="QA")
    //    {
    //        qry+="OUT "+zuluTime+" /ZFW "+zfw+" /FOB "+fob+" /TAW "+weight+"\n"+
    //                  "/AP "+ui->depIcao->text()+"\n/";
    //    }
    //    else if(blockType=="QC")
    //    {
    //        qry+="ON "+zuluTime+" /FOB "+fob+" /LAW "+weight+"\n"+
    //                  "/AP "+ui->arrIcao->text()+"\n/";
    //    }
    //    else if(blockType=="QB")
    //    {
    //        qry+="OFF "+zuluTime+" /FOB "+fob+" /TOW "+weight+"\n/";
    //    }
    //    else if(blockType=="QD")
    //    {
    //        qry+="IN "+zuluTime+" /FOB "+fob+" /RAW "+weight+"\n/";
    //    }
    //    else if(blockType=="AR")
    //    {
    //        if(eventType=="CLIMB")
    //            qry+="ALT "+alt+" CHG CLIMB\n/";
    //        else if(eventType=="DESCEND")
    //            qry+="ALT "+alt+" CHG DESC\n/";
    //        else
    //            qry+="ALT "+alt+" LEVEL\n/";
    //    }
    //
    //    qry+="POS "+lat+" "+lon+"\n";
    //
    //    if(blockType!="AR")
    //        qry+="/ALT "+alt+"\n";
    //
    //    qry+="/HDG "+hdg+"\n"+
    //         "/HDT "+trk+"\n"+
    //         "/IAS "+ias;
    //
    //    if(blockType=="AR" || blockType=="PR")
    //        qry+=" /TAS "+tas;
    //    qry+="\n";
    //
    //    qry+="/WND "+wnd+" /OAT "+oat+" /TAT 0\n";
    //
    //    if(blockType=="AR" || blockType=="PR")
    //    {
    //        qry+="/FOB "+fob+"\n"+
    //             "/DST "+dst+" - 0\n";
    //    }
    //
    //    if(blockType=="QB" || blockType=="QC")
    //    {
    //        for (int x = 0; x < 8; x++)
    //        {
    //            if(flightData.engON[x]!=0)
    //            {
    //                if(x>0)
    //                    qry+=" ";
    //                qry+="/E"+QString::number(x+1)+"N1 "+QS2F(flightData.engN1[x],0)+" /E"+QString::number(x+1)+"N2 "+QS2F(flightData.engN2[x],0);
    //            }
    //        }
    //        qry+="\n";
    //    }
    //} //all except for QM
    //
    //qry+="\n";
    //
    //vaLoggerReport("Report", qry);
    //
    //if(curl_fetch(url, qry, CURL_TIMEOUT_LIVEUPDATE_SECS)=="FAIL")
    //{
    //    wrn("Sending the Update failed ...");
    //    return false;
    //}
    //else
    //{
    //    dbg("Update sent.");
    //    return true;
    //}
}

QString MainWindow::va_ftw_xacars_endLiveFlight()
{
    //if(acarsState != AS_TRACKING || trackingId!="")
    //    return "";
    //
    //QString url = "liveacars.php";
    //QString qry;
    //
    //qry="DATA1="+acarsIdVer+"&DATA2=ENDFLIGHT&DATA3="+trackingId;
    //
    //vaLoggerReport("EndLive", qry);
    //
    //return curl_fetch(url, qry, CURL_TIMEOUT_GENERAL_SECS);
}

QString MainWindow::va_ftw_xacars_endFlight()
{
    //if(acarsState != AS_TRACKING)
    //    return "FAIL";
    //
    //if(va_ftw_xacars_endLiveFlight()=="FAIL")
    //    return "FAIL";
    //
    //QString url = "pirep.php";
    //QString qry;
    //
    //// PIREP ////////////////////////////////////////////////////////////////////////////////
    ////http://www.xacars.net/index.php?Client-Server-Protocol
    ////
    //// DATA2 includes all the data necessary for the PIREP as fields separated by "~". In the following listing each field is a new line, but see the data example below how it really looks like:
    ////
    //// - Username
    //// - Password
    //// - Flightnumber
    //// - Aircraft REG (also used for ICAO)
    //// - Altitude/Flightlevel
    //// - Flightrules (IFR or VFR)
    //// - Departure ICAO
    //// - Destination ICAO
    //// - Alternate ICAO
    //// - Departuretime (dd.mm.yyyy hh:mm)
    //// - Blocktime (hh:mm)
    //// - Flighttime (hh:mm)
    //// - Blockfuel
    //// - Flightfuel
    //// - Pax
    //// - Cargo
    //// - Online (VATSIM|ICAO|[other])
    //// - OUT (UNIX timestamp) - time of engine start
    //// - OFF (UNIX timestamp) - time of takeoff
    //// - ON (UNIX timestamp) - time of landing
    //// - IN (UNIX timestamp) - time of engine stop
    //// - ZFW - zero fuel weight in lbs
    //// - TOW - take off weight in lbs
    //// - LAW - landing weight in lbs
    //// - OUT latitude (N/Sxx xx.xxxx)
    //// - OUT longitude (E/Wxx xx.xxxx)
    //// - OUT altitude in ft
    //// - IN latitude (N/Sxx xx.xxxx)
    //// - IN longitude (E/Wxx xx.xxxx)
    //// - IN altitude in ft
    //// - max CLIMB in ft/min
    //// - max DESCEND in ft/min
    //// - max IAS in kt
    //// - max GS in kt
    //
    //// DATA1:'YACARS|2.0'
    //// DATA2:'Teddii~Maey1234~FTW1514~Piper~22032~VFR~HSKA~HESN~HESN~10.11.2018 15:23~02:17~02:15~810~799~0~0~OFFLINE~
    ////          1541863408~1541863504~1541871610~1541871650~
    ////          4770~5931~5131~
    ////          15.3883~36.3292~1674~
    ////          23.9531~32.822~652~
    ////          3209~3149~193~244~'
    //// DATA3:''
    //// DATA4:''
    //// DATA5:''
    //
    //// Teddii~Maey1234~FTW1514~Piper~22032~VFR~
    //// HSKA~HESN~HESN~
    //// 10.11.2018 15:23~02:17~02:15~810~799~0~0~~1541863408~1541863504~1541871610~1541871650~4770~5931~5131~15.3883~36.3292~1674~23.9531~32.822~652~3209~3149~193~244~'
    //
    //log("+++ RAW BLOCK TIME +++");
    //log("+ OnEnd:   "+QDateTime::fromTime_t(flightDataOnEnd.time, Qt::UTC).toString("HH:mm:ss"));
    //log("+ OnStart: "+QDateTime::fromTime_t(flightDataOnStart.time, Qt::UTC).toString("HH:mm:ss"));
    //log("= Block:   "+QDateTime::fromTime_t(flightDataOnEnd.time-flightDataOnStart.time, Qt::UTC).toString("HH:mm:ss"));
    //log("- Pause:   "+QDateTime::fromTime_t(flightDataOnEnd.pauseTime, Qt::UTC).toString("HH:mm:ss"));
    //log("= Total:   "+QDateTime::fromTime_t(flightDataOnEnd.time-flightDataOnStart.time-flightDataOnEnd.pauseTime, Qt::UTC).toString("HH:mm:ss"));
    //log("+++ RAW FLIGHT TIME +++");
    //log("+ OnEnd:   "+QDateTime::fromTime_t(flightDataOnLanding.time, Qt::UTC).toString("HH:mm:ss"));
    //log("+ OnStart: "+QDateTime::fromTime_t(flightDataOnTakeoff.time, Qt::UTC).toString("HH:mm:ss"));
    //log("= Flight:  "+QDateTime::fromTime_t(flightDataOnLanding.time-flightDataOnTakeoff.time, Qt::UTC).toString("HH:mm:ss"));
    //log("- Pause:   "+QDateTime::fromTime_t(flightDataOnLanding.pauseTime-flightDataOnTakeoff.pauseTime, Qt::UTC).toString("HH:mm:ss"));
    //log("= Total:   "+QDateTime::fromTime_t(flightDataOnLanding.time-flightDataOnTakeoff.time-(flightDataOnLanding.pauseTime-flightDataOnTakeoff.pauseTime), Qt::UTC).toString("HH:mm:ss"));
    //log("+++++++++++++++++++++++");
    //
    //QString maxAlt=QS2F(maximum.asl, 0);
    //QString depDateTime=QDateTime::fromTime_t(flightDataOnStart.time, Qt::UTC).toString("dd.MM.yyyy HH:mm"); //"10.11.2018 15:23";
    //
    //double bt=calcBlockTime();
    //double ft=calcFlightTime();
    //if(ft>bt) bt=ft;
    //QString blockTime=QDateTime::fromTime_t(static_cast<unsigned int>(bt), Qt::UTC).toString("HH:mm");
    //QString flightTime=QDateTime::fromTime_t(static_cast<unsigned int>(ft), Qt::UTC).toString("HH:mm");
    //log("+++ CALC'd TIMES ++++++");
    //log("= Block  + Pause: "+blockTime+" + "+QDateTime::fromTime_t(static_cast<unsigned int>(calcBlockPauseTime()), Qt::UTC).toString("HH:mm"));
    //log("= Flight + Pause: "+flightTime+" + "+QDateTime::fromTime_t(static_cast<unsigned int>(calcFlightPauseTime()), Qt::UTC).toString("HH:mm"));
    //log("+++++++++++++++++++++++");
    //
    //QString blockFuel=QS2F(flightDataOnStart.FUEL_lb-flightDataOnEnd.FUEL_lb, 0);
    //QString flightFuel=QS2F(flightDataOnTakeoff.FUEL_lb-flightDataOnLanding.FUEL_lb, 0);
    //
    //QString mode="OFFLINE";
    //QString tsOut=QS2F(flightDataOnStart.time, 0);
    //QString tsOff=QS2F(flightDataOnTakeoff.time, 0);
    //QString tsOn =QS2F(flightDataOnLanding.time, 0);
    //QString tsIn =QS2F(flightDataOnEngOff.time, 0);
    //
    //QString zfw=QS2F(flightDataOnTakeoff.ZFW_lb, 0);
    //
    //QString tow=QS2F(flightDataOnTakeoff.ZFW_lb+flightDataOnTakeoff.FUEL_lb, 0);
    //QString law=QS2F(flightDataOnLanding.ZFW_lb+flightDataOnLanding.FUEL_lb, 0);
    //
    //QString latOut=QS2F(flightDataOnStart.lat, 4);
    //QString lonOut=QS2F(flightDataOnStart.lon, 4);
    //
    //QString latIn =QS2F(flightDataOnEngOff.lat, 4);
    //QString lonIn =QS2F(flightDataOnEngOff.lon, 4);
    //
    //QString altOut=QS2F(flightDataOnStart.ASL_ft, 0);
    //QString altIn =QS2F(flightDataOnEngOff.ASL_ft, 0);
    //
    //QString maxClimb=QS2F(maximum.climb, 0);
    //QString maxDesc =QS2F(fabs(maximum.descend), 0);
    //QString maxIAS  =QS2F(maximum.ias, 0);
    //QString maxGS   =QS2F(maximum.gs, 0);
    //
    //qry="DATA1="+acarsIdVer+
    //"&DATA2="+ui->username->text()+"~"+ui->password->text()+"~"+ui->flightNo->text()+"~"+ui->acName->text()+"~"+maxAlt+"~"+ui->flightType->currentText()+"~"+
    //ui->depIcao->text()+"~"+ui->arrIcao->text()+"~"+ui->altIcao->text()+"~"+
    //depDateTime+"~"+blockTime+"~"+flightTime+"~"+
    //blockFuel+"~"+flightFuel+"~"+
    //ui->pax_count->text()+"~"+ui->cargo_lb->text()+"~"+mode+"~"+
    //tsOut+"~"+tsOff+"~"+tsOn+"~"+tsIn+"~"+
    //zfw+"~"+tow+"~"+law+"~"+
    //latOut+"~"+lonOut+"~"+altOut+"~"+latIn+"~"+lonIn+"~"+altIn+"~"+
    //maxClimb+"~"+maxDesc+"~"+maxIAS+"~"+maxGS+"~";
    //
    //vaLoggerReport("EndFlight", qry);
    //
    //if(curl_fetch(url, qry, CURL_TIMEOUT_GENERAL_SECS)=="FAIL")
    //    return "FAIL";
    //
    //return "OK";
}

//void MainWindow::va_ftw_xacars_newEvent(QJsonObject& e)
//{
//    events.append(e);
//}
//QString MainWindow::va_ftw_xacars_sendEvents()
//{
//}
//
//void MainWindow::va_ftw_xacars_newTrack(QJsonObject& t)
//{
//    tracks.append(t);
//}
//QString MainWindow::va_ftw_xacars_sendTracks() {
//}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

static size_t curl_ftw_xacars_write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    QByteArray* gCurlData = static_cast<QByteArray*>(userp);

    gCurlData->append(static_cast<const char*>(buffer), size*nmemb);

    return size * nmemb;
}

void MainWindow::va_ftw_xacars_curl_init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    gCurl = curl_easy_init();

    curl_easy_setopt(gCurl, CURLOPT_WRITEFUNCTION, curl_ftw_xacars_write_data);
    curl_easy_setopt(gCurl, CURLOPT_WRITEDATA, &gCurlData);

    //curl_slist *headers = NULL;
    //headers = curl_slist_append(headers, "Content-Type: application/text");
    //curl_easy_setopt(gCurl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(gCurl, CURLOPT_HEADER, 0L);
}

QString MainWindow::va_ftw_xacars_curl_fetch(QString url, QString query, long timeout)
{
//    char errbuf[CURL_ERROR_SIZE];

    if(baseUrl=="")
    {
        err("baseUrl not set for "+QString(__FUNCTION__));
        return "FAIL";
    }

    QUrl UrlObj;
    UrlObj.setUrl(QString(baseUrl)+QString(url));
    UrlObj.setQuery(query);

    vaLoggerDebug("curl", "+++++ "+url);
    vaLoggerDebug("curl", "  "+UrlObj.query());

    QByteArray fetch = UrlObj.toEncoded();

    curl_easy_setopt(gCurl, CURLOPT_URL, fetch.data());
    curl_easy_setopt(gCurl, CURLOPT_TIMEOUT, timeout);

//    //https://curl.haxx.se/libcurl/c/CURLOPT_VERBOSE.html
//    /* ask libcurl to show us the verbose output */
//    curl_easy_setopt(gCurl, CURLOPT_VERBOSE, 1L);
//    //https://curl.haxx.se/libcurl/c/CURLOPT_ERRORBUFFER.html
//    /* provide a buffer to store errors in */
//    curl_easy_setopt(gCurl, CURLOPT_ERRORBUFFER, errbuf);
//    /* set the error buffer as empty before performing a request */
//    errbuf[0] = 0;

    gCurlData.clear();
    qint32 ret = curl_easy_perform(gCurl);
    if(ret != CURLE_OK)
    {
        vaLoggerDebug("curl", "cURL-fetch: failed "+QString::number(ret));
        err("cURL-fetch: failed "+QString::number(ret));

//        size_t len = strlen(errbuf);
//
//        QString errStr="";
//        errStr+="cURL-fetch: failed "+QString::number(ret);
//        //errStr+=" => "+curl_easy_strerror(ret);
//        if(len)
//        {
//            errStr+=" => ";
//            errStr+=errbuf;
//        }
//
//        //vaLoggerDebug("curl", errStr);
//        vaLoggerReport("curl", errStr);

        gCurlData="FAIL";
    }
    else
    {
        //dbg("cURL-fetch: received "+QString(gCurlData.data()));
        vaLoggerDebug("curl", QString("........... .............................."));
        vaLoggerDebug("curl", "cURL-fetch: "+QString(fetch.data()));
        vaLoggerDebug("curl", ".......ret: "+QString::number(ret));
        vaLoggerDebug("curl", ".......len: "+QString::number(gCurlData.length()));
        vaLoggerDebug("curl", ".......dat: "+QString(gCurlData.data()));
        vaLoggerDebug("curl", QString("........... .............................."));
    }

    return gCurlData;
}
