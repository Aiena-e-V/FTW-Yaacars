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
#define LOGGER_SECTION "VAcm"

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void MainWindow::va_uiState(QString state)
{
    if(state=="INIT") //============================================================================
    {
        closeFSUIPC();
        closeXPUDP();

        //Init some VA internal stuff
        if(isProtoFTWJSON())
        {
            //YAACARS #161+
            baseUrl="http://www.ftw-sim.de/yaacars/server/v1.01/";
            //YAACARS #163+
            baseUrl="http://www.ftw-sim.de/yaacars/server/v1.02/";
            //YAACARS #172+
            baseUrl="http://www.ftw-sim.de/yaacars/server/v1.03/";
            //YAACARS #181+
            baseUrl="http://www.ftw-sim.de/yaacars/server/v1.04/";
            //YAACARS #189+
            baseUrl="http://www.ftw-sim.de/yaacars/server/v1.05/";
        }
        if(isProtoXACARS())
        {
            baseUrl="http://www.ftw-sim.de/acars/";
        }

        //Init UI
        ui->password->setEchoMode(QLineEdit::Password);

        //if(ui->username->text().toLower()!="teddii"
        //&& ui->username->text().toLower()!="stefan.klees"
        //&& ui->username->text().toLower()!="toffi"
        //&& ui->username->text().toLower()!="docbrown"
        //&& ui->username->text().toLower()!="orangetiger"
        //
        {
            ui->cbDevServerScript->setVisible(false);
            ui->cbDevServerScript->setChecked(false);
        }


        //ui->btnMetar->setVisible(false);
        //ui->btnSimBrief->setVisible(false);
        //ui->btnSkyVector->setVisible(false);
        ui->acIcao->setVisible(false);
        ui->acIcaoLabel->setVisible(false);
        if(isProtoXACARS())
        {
            ui->loginButton->setText("Connect to FTW");
            ui->loginButton->setVisible(true);
        }
        else {
            ui->loginButton->setVisible(false);
            ui->lbPassword->setText("FTW-Password");
        }

        if(isProtoXACARS())
        {
            ui->lbPassword->setText("XACARS-Passw.");
        }

        ui->conSim->setVisible(false);

        va_uiState("IDLE");
//        flightState = FS_OFFLINE;
    }
    else if(state=="IDLE") //============================================================================
    {
        flightData.acarsState = AS_IDLE;
        createLuaLoad(); //update for tracking!
//        flightState = FS_CONNECTED;

        mZeroFlightData;
        mapserver->clearWaypoints();

        if(ui->tabWidget->currentWidget()!=ui->settingsTab && ui->tabWidget->currentWidget()!=ui->aboutTab)
            ui->tabWidget->setCurrentWidget(ui->planTab);

        ui->getFlightplanButton->setEnabled(true);
        ui->inhibitWarnSoundButton->setVisible(false);

        //if(ui->username->text().toLower()!="teddii" && ui->username->text().toLower()!="docbrown" && ui->username->text().toLower()!="toffi" && ui->username->text().toLower()!="stefan.klees")
        ui->cbVaProto->setEnabled(false);

        ui->startButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
        ui->suspendButton->setEnabled(false);
        ui->endButton->setEnabled(false);
        //ui->loginButton->setStyleSheet("QPushButton { color: "+mRGB_red+"; }");

        ui->username->setReadOnly(false);
        ui->password->setReadOnly(false);
        ui->btnLoginDataEdit->setEnabled(true);

        if(isCommFSUIPC())
        {
            ui->cbCheckPayload->setEnabled(false); //doesn't work for FSUIPC!
            ui->cbCheckPayload->setChecked(false); //doesn't work for FSUIPC!
        }
        else
            ui->cbCheckPayload->setEnabled(true);

        if(isProtoXACARS())
        {
            //allow input of FlightNo and Fuel
            ui->flightNo->setReadOnly(false);
            ui->flightNo->setStyleSheet("QLineEdit { background: orange; color: black; }");
        }
        else
        {
            ui->flightNo->setReadOnly(true);
            ui->flightNo->setStyleSheet("");
        }

        ui->acName->setText("");
        ui->acIcao->setText("");
        ui->tailNumber->setText("");

        ui->flightNo->setText("");

        ui->depIcao->setText("");
        ui->arrIcao->setText("");
        ui->altIcao->setText("");

        ui->pax_count->setText("");
        ui->pax_lb->setText("");
        ui->pax_kg->setText("");

        ui->fuel_lb->setText("");
        ui->fuel_kg->setText("");

        ui->cargo_lb->setText("");
        ui->cargo_kg->setText("");
        ui->payload_lb->setText("");
        ui->payload_kg->setText("");

        //VATSIM
        ui->edtVatRoute->setText("");
        ui->edtVatAcType->setText("");
        ui->edtVatFlightLevel->setText("");
        ui->edtVatCrzTas->setText("");
        ui->edtVatDepTime->setText("");
        ui->edtVatEnrouteHrs->setText("");
        ui->edtVatEnrouteMins->setText("");
        ui->edtVatCallsign->setText("");

        //ui->cbVatFlightRules->setCurrentIndex(0);
        ui->cbVatVoice->setEnabled(false);
        ui->flightType->setEnabled(false);
    }

    else if(state=="LOGIN") //=====================================================================
    {
//        flightState = FS_CONNECTED;

        ui->loginButton->setStyleSheet("QPushButton { color: "+mRGB_grn+"; }");
        //ui->tabWidget->setCurrentWidget(ui->planTab);

        //if(ui->username->text().toLower()=="teddii") //easyTesting(tm)
        //    ui->tabWidget->setCurrentWidget(ui->planTab);
   }

    else if(state=="FLIGHTPLAN") //=====================================================================
    {
        flightData.acarsState = AS_LOADED;
        createLuaLoad(); //update for tracking!

        ui->resetButton->setEnabled(true);
        //if(flightData.isResumedFlightFTW)
        //    ui->suspendButton->setEnabled(true);

        ui->startButton->setEnabled(true);
        ui->endButton->setEnabled(false);

        if(isProtoXACARS())
        {
            ui->flightNo->setReadOnly(true);
            ui->flightNo->setStyleSheet("");

            ui->fuel_lb->setReadOnly(false);
            ui->fuel_lb->setStyleSheet("QLineEdit { background: lightblue; color: black; }");
            ui->fuel_kg->setReadOnly(false);
            ui->fuel_kg->setStyleSheet("QLineEdit { background: lightblue; color: black; }");
            ui->pax_lb->setReadOnly(false);
            ui->pax_lb->setStyleSheet("QLineEdit { background: lightblue; color: black; }");
            ui->pax_kg->setReadOnly(false);
            ui->pax_kg->setStyleSheet("QLineEdit { background: lightblue; color: black; }");
            ui->arrIcao->setReadOnly(false);
            ui->arrIcao->setStyleSheet("QLineEdit { background: lightblue; color: black; }");

            ui->altIcao->setReadOnly(false);
            ui->altIcao->setStyleSheet("QLineEdit { background: DarkSeaGreen; color: black; }");
            ui->tailNumber->setReadOnly(false);
            ui->tailNumber->setStyleSheet("QLineEdit { background: DarkSeaGreen; color: black; }");
        }
        if(isProtoFTWJSON())
        {
            if(!mIsRescue())
            {
                ui->arrIcao->setReadOnly(false);
                ui->arrIcao->setStyleSheet("QLineEdit { background: lightblue; color: black; }");
                ui->altIcao->setReadOnly(false);
                ui->altIcao->setStyleSheet("QLineEdit { background: DarkSeaGreen; color: black; }");
            }
        }

        ui->username->setReadOnly(true);
        ui->password->setReadOnly(true);
        ui->btnLoginDataEdit->setEnabled(false);

        //VATSIM
        ui->edtVatRoute->setReadOnly(false);
        ui->edtVatAcType->setReadOnly(false);
        ui->edtVatFlightLevel->setReadOnly(false);
        ui->edtVatCrzTas->setReadOnly(false);
        ui->edtVatDepTime->setReadOnly(false);
        ui->edtVatEnrouteHrs->setReadOnly(false);
        ui->edtVatEnrouteMins->setReadOnly(false);
        ui->edtVatCallsign->setReadOnly(false);

        ui->flightType->setEnabled(true);
        ui->cbVatVoice->setEnabled(true);
    }

    else if(state=="STARTFLIGHT") //=====================================================================
    {
        flightData.acarsState = AS_TRACKING;
        createLuaLoad(); //update for tracking!

        log("+++++++++++++++++++++++++++++");
        log("Starting flight...");
        log("... for "+ui->username->text());
        if(isCommFSUIPC())
            log("... using FSUIPC connection");
        else
            log("... using XP-UDP connection");
        log("+++++++++++++++++++++++++++++");

        ui->getFlightplanButton->setEnabled(false);

        ui->startButton->setEnabled(false);
        //#ifdef DEBUG
        //    //ui->endButton->setEnabled(true); //we allow to end the flight right after start for debug purposes!
        //#endif
        ui->resetButton->setEnabled(true);
        ui->suspendButton->setEnabled(true);

        ui->cbVaProto->setEnabled(false);
        ui->cbVaComm->setEnabled(false);

        ui->arrIcao->setReadOnly(true); //enabled for XACARS, disable now
        ui->arrIcao->setStyleSheet("");
        ui->altIcao->setReadOnly(true); //enabled for XACARS, disable now
        ui->altIcao->setStyleSheet("");
        ui->fuel_lb->setReadOnly(true); //enabled for XACARS, disable now
        ui->fuel_lb->setStyleSheet("");
        ui->fuel_kg->setReadOnly(true); //enabled for XACARS, disable now
        ui->fuel_kg->setStyleSheet("");
        ui->pax_lb->setReadOnly(true); //enabled for XACARS, disable now
        ui->pax_lb->setStyleSheet("");
        ui->pax_kg->setReadOnly(true); //enabled for XACARS, disable now
        ui->pax_kg->setStyleSheet("");
        ui->tailNumber->setReadOnly(true); //enabled for XACARS, disable now
        ui->tailNumber->setStyleSheet("");

        ui->tabWidget->setCurrentWidget(ui->dataTab);

        ui->textHtmlInfo->clear();
        //ui->critEvents->clear();
        //ui->flightEvents->clear();
        log("FLIGHT STARTED");
        //cpName=cpNames->value(QRandomGenerator::global()->bounded(cpNames->count()));
        //newEvent("Hi! I'm your copilot for today! How are you doing?", true);
    }
    else if(state=="LANDING") //=======================================================================
    {
//        flightState = FS_TAXITOGATE;
        //$$ flightDataOnLanding = flightData;
        ui->endButton->setEnabled(true);
        log("LANDED");
    }
    else if(state=="ENDFLIGHT") //=======================================================================
    {
        log("FLIGHT ENDED");
        //$$ if(flightDataOnLanding.time==0) //ended flight without proper landing?
        //$$ {
        //$$     wrn("Time for Landing wasn't set properly! Please report this bug!");
        //$$     flightDataOnLanding = flightData;
        //$$ }

        //$$ if(flightDataOnEngOff.time==0) //no engines shutdown, then update endtime here!
        //$$     flightDataOnEngOff = flightData;
        //$$
        //$$ flightDataOnEnd = flightData;

        //check for maximum time to takeoff
        //$$ if( flightDataOnEngOff.time-flightDataOnLanding.time>TIME_AFTER_LANDING_SECS)
        //$$ {
        //$$     wrn("Time from Landing to EngineOff has been reduced from "+QS2I((flightDataOnEngOff.time-flightDataOnLanding.time)/60)+"min to "+QS2I(TIME_AFTER_LANDING_SECS/60)+"min");
        //$$     flightDataOnEngOff.time=flightDataOnLanding.time+TIME_AFTER_LANDING_SECS;
        //$$ }
        //$$ if( flightDataOnEnd.time   -flightDataOnLanding.time>TIME_AFTER_LANDING_SECS)
        //$$ {
        //$$     wrn("Time from Landing to EndFlight has been reduced from "+QS2I((flightDataOnEnd.time-flightDataOnLanding.time)/60)+"min to "+QS2I(TIME_AFTER_LANDING_SECS/60)+"min");
        //$$     flightDataOnEnd.time   =flightDataOnLanding.time+TIME_AFTER_LANDING_SECS;
        //$$ }

        //ui->tabWidget->setCurrentWidget(ui->summaryTab);
    }
    else
        err("Unknown UI state: '"+state+"'");
}

void MainWindow::va_UpdTimer()
{
    static long callingCount = 0;

    if(flightData.acarsState!=AS_TRACKING)
        return;

    callingCount++;

    QString va_ret=va_report("POSITION");
    if(va_ret == "") //no error?
    {
        dbg("Update sent.");

        if(mIsRescue())
            _va_UpdTimer.setInterval(va_UpdTimeRsq);
         else
            _va_UpdTimer.setInterval(va_UpdTime);


    }
    else
    {
        err("UPDATE failed: "+va_ret);
        _va_UpdTimer.setInterval(va_UpdTime);
        //_va_UpdTimer.setInterval(va_UpdOfflineTime);
        //wrn("Connection seems to be down - sleeping for "+QS2I(va_UpdOfflineTime/1000)+" secs ...");
    }
}

void MainWindow::vaLoggerDebug(QString pre, QString txt)
{
    if(ui->password->text()!="")
        txt=txt.replace(ui->password->text(),"*****");
    txt=txt.replace("|"," ");
    txt=txt.replace("~"," ");
    vaLogger->write_dbg(pre, txt);
}
void MainWindow::vaLoggerReport(QString pre, QString txt)
{
    if(ui->password->text()!="")
        txt=txt.replace(ui->password->text(),"*****");
    txt=txt.replace("|"," ");
    txt=txt.replace("~"," ");
    vaLogger->write_log(pre, txt);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

QString MainWindow::va_login()
{
    if(isProtoXACARS())
        return va_ftw_xacars_login();
    else if(isProtoFTWJSON())
        return va_ftw_json_login();
    else
    {
        wrn("no va_login() for "+ui->cbVaProto->currentText());
        return "FAIL";
    }
}

QString MainWindow::va_getFlightplan()
{
    if(isProtoXACARS())
        return va_ftw_xacars_getFlightplan();
    else if(isProtoFTWJSON())
        return va_ftw_json_getFlightplan();
    else
    {
        wrn("no va_getFlightplan() for "+ui->cbVaProto->currentText());
        return "FAIL";
    }
}

QString MainWindow::va_startFlight()
{
    if(isProtoXACARS())
        return va_ftw_xacars_startFlight();
    else if(isProtoFTWJSON())
        return va_ftw_json_startFlight();
    else
    {
        wrn("no va_startFlight() for "+ui->cbVaProto->currentText());
        return "FAIL";
    }
}

QString MainWindow::va_endFlight()
{
    //QString ret="FAIL";
    //if(isProtoXACARS())
    //    ret=va_ftw_xacars_login();
    //else if(isProtoFTWJSON())
    //    ret=va_ftw_json_login();
    //
    //QMessageBox::StandardButton resBtn = QMessageBox::question( this, "YAACARS",
    //                                                            "Connection to the server: "+ret+
    //                                                            "\n\nDo you want to end your flight now?",
    //                                                            QMessageBox::No | QMessageBox::Yes,
    //                                                            QMessageBox::Yes);
    //if (resBtn == QMessageBox::Yes)
    //{
    //    log("Sending data ...");
        if(isProtoXACARS())
            return va_ftw_xacars_endFlight();
        else if(isProtoFTWJSON())
            return va_ftw_json_endFlight();
        else
        {
            wrn("no va_endFlight() for "+ui->cbVaProto->currentText());
            return "FAIL";
        }
    //}
    //log("Ending the flight cancelled by user");
    //return "CANCEL";
}

QString MainWindow::va_report(QString eventType)
{
    if(isProtoXACARS())
        return va_ftw_xacars_report(eventType);
    else if(isProtoFTWJSON())
        return va_ftw_json_report(eventType);
    else
    {
        wrn("no va_report() for "+ui->cbVaProto->currentText());
        return "FAIL";
    }
}

QString MainWindow::va_resetFlight()
{
    if(isProtoXACARS())
        return "FAIL";
    else if(isProtoFTWJSON())
        return va_ftw_json_resetFlight();
    else
    {
        wrn("no va_resetFlight() for "+ui->cbVaProto->currentText());
        return "FAIL";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::va_curl_init()
{
    if(isProtoXACARS())
        va_ftw_xacars_curl_init();
    else if(isProtoFTWJSON())
        va_ftw_json_curl_init();
    else
    {
        wrn("no va_curl_init() for "+ui->cbVaProto->currentText());
        return;
    }
}
