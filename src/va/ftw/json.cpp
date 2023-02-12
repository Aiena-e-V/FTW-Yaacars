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

#define acarsIdVer QString("YAACARS|1.0")

#define curl_fetch va_ftw_json_curl_fetch

//////////////////////////////////////////////////////////////////////////////////////
#define LOGGER_SECTION "VAjs"

#define mShowHtmlInfo() \
{ \
    if(jRet.find("html_info")!=jRet.end()) \
        ui->textHtmlInfo->setText(jRet["html_info"].toString()); \
}

#define addUserData() \
{ \
    jObj["pilot"] = ui->username->text(); \
}

#define addDataHash() \
{ \
    jObj["tracking_id"] = flightData.trackingIdFTW; \
    QByteArray data; \
    data.append("//BEGIN/"); \
    foreach(const QString& key, jObj.keys()) \
    { \
        if(key!="hash2" && key!="hash1" && key!="date1") \
        { \
            data.append(jObj.value(key).toString()); \
            data.append("<="); \
            data.append(key); \
            data.append("/"); \
        } \
    } \
    data.append("END//"); \
    jObj["hash2"] = QString(QCryptographicHash::hash(data,QCryptographicHash::Md5).toHex()); \
}

#define checkDataHash(JOBJ) \
{ \
    QByteArray data; \
    data.append("//BEGIN/"); \
    foreach(const QString& key, JOBJ.keys()) \
    { \
        if(key!="hash2" && key!="hash1" && key!="date1") \
        { \
            data.append(JOBJ.value(key).toString()); \
            data.append("<="); \
            data.append(key); \
            data.append("/"); \
        } \
    } \
    data.append("END//"); \
    if(JOBJ["hash2"].toString() != QString(QCryptographicHash::hash(data,QCryptographicHash::Md5).toHex())) \
    { \
        JOBJ["res_code"] = "loc:dataIntegrityBroken"; \
        JOBJ["res_text"] = "local: data integrity broken!\n\nPLEASE REPORT THIS BUG!"; \
        JOBJ["res_text"] = "local: data integrity broken!\n"+JOBJ["hash2"].toString()+"\n"+QString(QCryptographicHash::hash(data,QCryptographicHash::Md5).toHex()); \
    } \
}

#define dumpJsonObj(JOBJ) \
{ \
    log("JSON: === SOD ============================================"); \
    foreach(const QString& key, JOBJ.keys()) \
        if(key!="hash2" && key!="hash1" && key!="date1") \
            log("JSON: "+key+" => "+JOBJ.value(key).toString()); \
    log("JSON: === EOD ============================================"); \
}

//#define mClientData jObj["client_data"]="REV=1 toT="+QS2I(flightData.timeTakeoff_secs)+" ldgT="+QS2I(flightData.timeLanding_secs)
#define mClientData jObj["client_data"]="{\"REV\":\"1\", \"toT\":\""+QS2I(flightData.timeTakeoff_secs)+"\", \"ldgT\":\""+QS2I(flightData.timeLanding_secs)+"\"}"

#define addTrackingData() \
{ \
    jObj["flight_number"] = ui->flightNo->text(); \
 \
    jObj["lat"]=QS2F(flightData.coordsCURR.first, 6); \
    jObj["lon"]=QS2F(flightData.coordsCURR.second, 6); \
 \
    jObj["flt_mins"]=QS2F(flightData.timeFlight_secs/60,0); \
    jObj["blk_mins"]=QS2F(flightData.timeBlock_secs/60,0); \
 \
    mClientData; \
 \
    jObj["fuel_kg"]=QS2F(flightData.FUELftwTotal_lb*mulLB2KG, 1); \
    jObj["fuelUsed_kg"]=QS2F((flightData.FUELftwUsed_lb)*mulLB2KG, 1); \
 \
    jObj["dist_nm"]=QS2F(fabs(flightData.distance_nm),2); \
 \
    jObj["asl_ft"]=QS2F(flightData.ASL_ft, 1); \
    jObj["agl_ft"]=QS2F(flightData.AGL_ft, 1); \
 \
    jObj["oat"]=QS2F(flightData.oat,2); \
    jObj["wnd_dir"]=QS2F(flightData.winddeg, 0); \
    jObj["wnd_spd"]=QS2F(flightData.windknots, 1); \
 \
    jObj["hdg"]=QS2F(flightData.HDG,0); \
    jObj["ias_kts"]=QS2F(fabs(flightData.IAS_kts), 0); \
    jObj["tas_kts"]=QS2F(fabs(flightData.TAS_kts), 0); \
    jObj["gs_kts"]=QS2F(fabs(flightData.GS_kts), 0); \
    jObj["vs_fpm"]=QS2F(fabs(flightData.VS_fpm), 0); \
}

QString MainWindow::va_ftw_json_login()
{
    return "";
}

QString MainWindow::va_ftw_json_getFlightplan()
{
    QString qry = "proto="+acarsIdVer+"&action=getflight";

    QJsonObject jObj, jRet;
    addUserData();
    QByteArray data="STUFFBITS>>";
    data.append(passwordDate);
    data.append("<<STIBFFUTS>>");
    data.append(passwordHash);
    data.append("<<STIBFFUTS>>");
    data.append(passwordDate);
    data.append("<<STUFFBITS");

                                    //login with hash! ONLY HERE ONCE - so that PW change in FTW after flight doesn't matter!
    jObj["hash1"] = QString(QCryptographicHash::hash(data,QCryptographicHash::Md5).toHex());
    jObj["date1"] = passwordDate;   //login with hash! ONLY HERE ONCE - so that PW change in FTW after flight doesn't matter!
    jObj["client"] = build;

    addDataHash();
    jRet=curl_fetch(qry, jObj, CURL_TIMEOUT_GENERAL_SECS);

    mShowHtmlInfo();

    //if(ui->username->text().toLower()=="teddii")
    //    dumpJsonObj(jRet);

    if(jRet["res_code"].toString()=="ok")
    {
        flightData.trackingIdFTW=jRet["tracking_id"].toString();

        flightData.missionTypeFTW=jRet["mission_type"].toString().toInt();

        ui->depIcao->setText(jRet["dep_icao"].toString());
        flightData.coordsDEP.first     =jRet["dep_lat"].toString().toDouble();
        flightData.coordsDEP.second    =jRet["dep_lon"].toString().toDouble();

        ui->arrIcao->setText(jRet["arr_icao"].toString());
        flightData.coordsARR.first    =jRet["arr_lat"].toString().toDouble();
        flightData.coordsARR.second   =jRet["arr_lon"].toString().toDouble();

        ui->altIcao->setText(jRet["alt_icao"].toString());
        flightData.coordsALT.first     =jRet["alt_lat"].toString().toDouble();
        flightData.coordsALT.second    =jRet["alt_lon"].toString().toDouble();

        //ui->edtVatRoute->setText(jRet["route"].toString());
        //ui->acIcao->setText(jRet["plane_icao"].toString());
        ui->pax_count->setText( jRet["pax_count"].toString());
        ui->flightNo->setText(  jRet["flight_number"].toString());
        ui->acName->setText(    jRet["plane_name"].toString());
        ui->tailNumber->setText(jRet["plane_reg"].toString());

        ui->pax_kg->setText(             jRet["pax_kg"].toString());
        ui->pax_lb->setText(  QS2F(floor(jRet["pax_kg"].toString().toDouble()*mulKG2LB),0));
        ui->cargo_kg->setText(           jRet["cargo_kg"].toString());
        ui->cargo_lb->setText(QS2F(floor(jRet["cargo_kg"].toString().toDouble()*mulKG2LB),0));
        ui->fuel_kg->setText(            jRet["fuel_kg"].toString());
        ui->fuel_lb->setText( QS2F(floor(jRet["fuel_kg"].toString().toDouble()*mulKG2LB),0));

        ///////////////////////////////////////////////////////////////
        flightData.isResumedFlightFTW   =jRet["flight_state"].toString().toInt();
        flightData.FUELftwUsed_lb       =jRet["fuel_used"].toString().toDouble()*mulKG2LB;
        flightData.FUELftwTotal_lb      =jRet["fuel_kg"].toString().toDouble()*mulKG2LB;
        flightData.distance_nm          =jRet["miles_flown"].toString().toDouble();
        flightData.timeBlock_secs       =jRet["block_time"].toString().toDouble()*60;
        flightData.timeFlight_secs      =jRet["flight_time"].toString().toDouble()*60;

        flightData.coordsLOADED.first   =jRet["cur_lat"].toString().toDouble();
        flightData.coordsLOADED.second  =jRet["cur_lon"].toString().toDouble();



        if(jRet["flight_state"].toString()=="1")
        {
            int rev=0;

            QJsonParseError parseErr;
            QString data=jRet["client_data"].toString();
            data=data.replace("&quot;","\"");
            QJsonDocument jClientDoc = QJsonDocument::fromJson(data.toUtf8(), &parseErr);
            QJsonObject jClientData = jClientDoc.object();

            if(parseErr.error==parseErr.NoError && jClientData["REV"].toString()=="1")
            {
                rev=jClientData["REV"].toString().toInt();
                flightData.timeTakeoff_secs=jClientData["toT"].toString().toDouble();
                flightData.timeLanding_secs=jClientData["ldgT"].toString().toDouble();
            }
            else
            {
                rev=0;
                //QMessageBox::information(this, "YAACARS",
                //    QString("Parser-Error:\n"+parseErr.errorString()+"\n\n"+jRet["client_data"].toString()),
                //    QMessageBox::Ok);
            }

            if(rev!=1)
            {
                flightData.timeTakeoff_secs=0;  //started immediately
                flightData.timeLanding_secs=-1; //not landed yet - will start counting now if on ground
                QMessageBox::information(this, "YAACARS",
                    QString("Please note, that some in-flight data could not be recovered ...\nPreflight/Postflight times have been reset to zero,\nbut you can still end the flight at your arrival airport!\n\nPLEASE REPORT THIS BUG!"),
                    QMessageBox::Ok);
            }
            QMessageBox::information(this, "YAACARS",
                QString("You are resuming a suspended flight!<br><br><a href='"+URL_RESUME_FAQ+"'>How does it work?</a><br><br>Enjoy - and good luck! :-)")
                /*+QString("\nFUELused=")+QS2I(fdFUELftwUsd)*/,
                QMessageBox::Ok);
        }

        ///////////////////////////////////////////////////////////////
        if(jRet["info_message"]!="")
            QMessageBox::information(this, "YAACARS - Server Message",
                                 QString(jRet["info_message"].toString()),
                                 QMessageBox::Ok);

        return "";
    }

    //retun the error
    return jRet["res_text"].toString();
}

QString MainWindow::va_ftw_json_startFlight()
{
    QString qry = "proto="+acarsIdVer+"&action=startflight";

    QJsonObject jObj, jRet;
    addUserData();

    jObj["arr_icao"]        = ui->arrIcao->text();
    jObj["arr_lat"]         = QS2F(flightData.coordsARR.first, 6);
    jObj["arr_lon"]         = QS2F(flightData.coordsARR.second,6);

    jObj["alt_icao"]        = ui->altIcao->text();
    jObj["alt_lat"]         = QS2F(flightData.coordsALT.first, 6);
    jObj["alt_lon"]         = QS2F(flightData.coordsALT.second,6);

    jObj["fuelInit_kg"]     = ui->fuel_kg->text();

    addTrackingData();

    addDataHash();
    jRet=curl_fetch(qry, jObj, CURL_TIMEOUT_GENERAL_SECS);

    mShowHtmlInfo();

    return jRet["res_text"].toString();
}

QString MainWindow::va_ftw_json_report(QString eventType)
{
    if(flightData.acarsState != AS_TRACKING)
        return "";

    dbg("Sending "+eventType+" update ... ");

    QString qry = "proto="+acarsIdVer+"&action=tracking";

    QJsonObject jObj, jRet, jLive;
    addUserData();
    jObj["event"] = eventType;
    jObj["count"] = QS2I(flightData.trackingCount++);
    addTrackingData();

    addDataHash();
    jRet=curl_fetch(qry, jObj, CURL_TIMEOUT_LIVEUPDATE_SECS);

    mShowHtmlInfo();

    if(jRet.find("live")!=jRet.end())
    {
        mapserver->setFlights(jRet["live"].toString());
        dbg("live data received");
        //log(jRet["live"].toString());
    }
    //else
    //    log("no live data");

    if(jRet["res_code"].toString()=="ok")
    {
        flightData.isAborted=false; //in case of server fetches correct data again
    }
    else if(jRet["res_code"].toString()=="aborted")
    {
        //wird ein Deck weiter oben ausgegeben err(jRet["res_text"].toString());
        flightData.isAborted=true;
    }
    else
    {
        //wird ein Deck weiter oben ausgegeben err(jRet["res_text"].toString());
        //err("SENDING UPDATE FAILED!");
        if(jRet["res_text"].toString()=="")
        {
            jRet["res_code"]="loc:failed";
            jRet["res_text"]="local:Update failed";
        }
    }

    return jRet["res_text"].toString();
}

QString MainWindow::va_ftw_json_endFlight()
{
    if(flightData.acarsState != AS_TRACKING)
        return "You are not on ground!";

    QString qry = "proto="+acarsIdVer+"&action=endflight";

    QJsonObject jObj, jRet;
    addUserData();
    addTrackingData();

    addDataHash();
    jRet=curl_fetch(qry, jObj, CURL_TIMEOUT_GENERAL_SECS);

    mShowHtmlInfo();

    return jRet["res_text"].toString();
}

QString MainWindow::va_ftw_json_resetFlight()
{
    QString qry = "proto="+acarsIdVer+"&action=resetflight";

    QJsonObject jObj, jRet;
    addUserData();

    addDataHash();
    jRet=curl_fetch(qry, jObj, CURL_TIMEOUT_GENERAL_SECS);

    mShowHtmlInfo();

    return jRet["res_text"].toString();
}

//void MainWindow::va_ftw_json_newEvent(QJsonObject& e)
//{
//    events.append(e);
//}
//QString MainWindow::va_ftw_json_sendEvents()
//{
//    return "";
//}
//
//void MainWindow::va_ftw_json_newTrack(QJsonObject& t)
//{
//    tracks.append(t);
//}
//QString MainWindow::va_ftw_json_sendTracks() {
//    return "";
//}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

static size_t curl_ftw_json_write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    QByteArray* gCurlData = static_cast<QByteArray*>(userp);

    gCurlData->append(static_cast<const char*>(buffer), size*nmemb);

    return size * nmemb;
}

void MainWindow::va_ftw_json_curl_init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    gCurl = curl_easy_init();

    curl_easy_setopt(gCurl, CURLOPT_WRITEFUNCTION, curl_ftw_json_write_data);
    curl_easy_setopt(gCurl, CURLOPT_WRITEDATA, &gCurlData);

    curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(gCurl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(gCurl, CURLOPT_HEADER, 0L);
}

QJsonObject MainWindow::va_ftw_json_curl_fetch(QString query, QJsonObject jObj, long timeout)
{
    if(baseUrl=="")
    {
        err("baseUrl not set for "+QString(__FUNCTION__));
        QJsonObject jEmpty;
        return jEmpty;
    }

    QString url="yaacars.php";
    if(ui->cbDevServerScript->isChecked())
        url="yaacars_dev.php";

    QUrl UrlObj;
    UrlObj.setUrl(QString(baseUrl)+QString(url));
    UrlObj.setQuery(query);

    QByteArray fetch = UrlObj.toEncoded();
    curl_easy_setopt(gCurl, CURLOPT_URL, fetch.data());
    curl_easy_setopt(gCurl, CURLOPT_TIMEOUT, timeout);

    QJsonDocument jDoc(jObj);
    QByteArray payload = jDoc.toJson();
    curl_easy_setopt(gCurl, CURLOPT_POST, 1);
    curl_easy_setopt(gCurl, CURLOPT_POSTFIELDS, payload.data());

    //log("========================== "+UrlObj.query());
    //dbg(payload);

    gCurlData.clear();
    qint32 ret = curl_easy_perform(gCurl);
    if(ret != CURLE_OK)
    {
        err("cURL-fetch failed: ("+QString::number(ret)+")");
        gCurlData="";
    }

    QJsonParseError parseErr;
    jDoc = QJsonDocument::fromJson(gCurlData, &parseErr);
    QJsonObject jServer = jDoc.object();

    //log("Server Response: "+gCurlData);

    if(parseErr.error==parseErr.NoError)
    {
        if(jServer.find("res_code")!=jServer.end()) //received result
        {
            //if(debug)
            //{
            //    log("==> result: "+jServer["res_code"].toString());
            //    dbg("--- JSON Data Dump ---");
            //    foreach (QString key, jServer.keys())
            //        dbg("--> ["+key+"]<"+jServer[key].toString()+">");
            //
            //    log("---");
            //}
            checkDataHash(jServer);

            return jServer;
        }
        else
        {
            err("JSON ERROR - no result code received from server!");

            QJsonObject jEmpty;
            jEmpty["res_code"] = "loc:empty";
            jEmpty["res_text"] = "local: no data received";
            return jEmpty;
        }
    }
    else
    {
        QJsonObject jEmpty;
        jEmpty["res_code"] = "loc:parse";
        if(gCurlData=="")
        {
            if(ret==28)
            {
                //err("Connection to host timed out!");
                jEmpty["res_text"] = "local: connection to host timed out";
            }
            else
            {
                //err("JSON NO DATA RECEIVED");
                jEmpty["res_text"] = "local: no data received";
            }
        }
        else
        {
            //err("JSON PARSER ERROR: "+parseErr.errorString());
            //err(gCurlData);
            jEmpty["res_text"] = "local: parsing failed ("+parseErr.errorString()+")";
        }

        return jEmpty;
    }
}
