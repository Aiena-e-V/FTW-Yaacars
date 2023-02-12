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

#include "version.h"
#include "revision.h"

#include "yaacars.h"
#include "ui_mainwindow.h"
#include "ipdialog.h"

#include <QCommandLineParser>
#include <QFileDialog>
#include <QTimer>

#include <QCloseEvent>
#include <QProcess>
#include <QDesktopServices>

//////////////////////////////////////////////////////////////////////////////////////
#define LOGGER_SECTION "GUI"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mapserver = new MapServer(1111, false);
    //QObject::connect(mapserver, &MapServer::closed, &a, &QCoreApplication::quit);

    //crcgen=new Crc32;

    infobar=new(InfoBar);
    //infobar->hide();
    //infobar->exec();

    mZeroFlightData;

    cpNames = new QStringList;
    cpNames->append("Whining William");
    cpNames->append("Picky Paul");
    cpNames->append("Anxious Amelie");
    cpNames->append("Frightened Fred");
    cpNames->append("Moaning Mona");

    checkCreateBasePath();
    uiLogger = new Logger(this, BASEDIR+"/yaacars.log", ui->logBox, ui->statusBar);
    //vaLogger = new Logger(this, BASEDIR+"/pireps.log", ui->txtPirepData);
    dbg("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=");

    airports.logger=uiLogger;

    ui->cbLoggerDbg->setChecked(false);
    uiLogger->dbg=false;
    ui->cbLoggerDbg->setEnabled(true);
    ui->cbLoggerLog->setEnabled(false);
    ui->cbLoggerWrn->setEnabled(false);
    ui->cbLoggerErr->setEnabled(false);

    //vaLogger->dbg=false;

    sRealTime = new QLabel(this);
    ui->statusBar->addPermanentWidget(sRealTime);
    sBlockTime = new QLabel(this);
    ui->statusBar->addPermanentWidget(sBlockTime);
    sFlightTime = new QLabel(this);
    ui->statusBar->addPermanentWidget(sFlightTime);

    build="V"+QString(VERSION)+" (Build "+QString(REV_BRANCH).toUpper()+"#"+QString::number(REV_CHECKINS)+")";
    //QString build="V"+QString(VERSION)+" (Build #"+QString::number(REV_CHECKINS)+")";
    QString appShort="YAACARS";
    QString appLong="Yet Another ACARS for 'Fly the World'";

    //UI tailoring for XACARS/FTW
    setWindowTitle(appShort+" :: "+build+" :: "+appLong);
    infobar->setWindowTitle("YAACARS");

    log("Logging to yaacars.log started for "+appShort+" :: "+build);
    //vaLoggerReport("INIT", "Logging to pireps.log started for "+appShort+" :: "+build);

    /////////////////////////////////////////////////////////////////////////////////////////

    log("Reading user settings ...");
    QSettings settings("YAACARS", "YAACARS", this);
    ui->username->setText(settings.value("callsign", "").toString());
    ui->password->setText(settings.value("password", "").toString());
    ui->cbCheckFuel->setChecked(settings.value("checkFuel", true).toBool());
    ui->cbCheckPayload->setChecked(settings.value("checkWeight", true).toBool());
    ui->cbVaComm->setCurrentIndex(settings.value("CommIdx", 0).toInt());
    ui->cbKmlPos->setChecked(settings.value("kmlPos", false).toBool());
    ui->cbKmlRte->setChecked(settings.value("kmlRte", false).toBool());
    ui->cbAirlineFlight->setChecked(settings.value("checkAirlineFlight", false).toBool());
    //ui->cbAutorunSB->setChecked(settings.value("autorunSimBrief", false).toBool());
    //ui->cbAutorunSV->setChecked(settings.value("autorunSkyVector", false).toBool());
    ui->cbDisableWarnSound->setChecked(settings.value("disableWarnSound", false).toBool());

    ui->cbDevServerScript->setChecked(settings.value("devServerScript", true).toBool());

    ui->cbVaProto->setCurrentIndex(settings.value("ProtoIdx", 0).toInt());
    ui->cbVaProto->setCurrentIndex(1); //fixed to JSON
    dbg("Reading user settings done");

    //convert hash from old password
    //if(ui->password->text()!="") //has old password -> convert to hash
    //{
    //    QByteArray str=ui->password->text().toUtf8();
    //    QByteArray hash = QCryptographicHash::hash(str, QCryptographicHash::Sha512);
    //    passwordHash=hash.toHex();
    //    settings.setValue("passhash", passwordHash);
    //    settings.setValue("password", "");
    //    ui->password->setText("");
    //    log("PWD: old password converted");
    //}

    //username
    if(ui->username->text()=="")
    {
        ui->tabWidget->setCurrentWidget(ui->settingsTab);
        ui->username->setStyleSheet("QLineEdit { background-color: orange; color: black; }");
    }

    //load hash
    passwordHash=settings.value("passhash", "").toString();
    passwordDate=settings.value("passdate", "").toString();
    if(passwordHash=="" || passwordDate=="")
    {
        on_btnLoginDataEdit_clicked();
        ui->tabWidget->setCurrentWidget(ui->settingsTab);
        if(passwordDate=="")
            wrn("PWD: old-style password set!");
        else
            wrn("PWD: no password set!");
    }
    else
    {
        ui->password->setVisible(false);
        ui->btnLoginDataEdit->setVisible(true);
        ui->btnLoginDataSave->setVisible(false);
        log("PWD: password is set");

    }

    if(ui->tabWidget->currentWidget()!=ui->settingsTab)
    {
        if(ui->username->text().toLower()=="teddii")
            ui->tabWidget->setCurrentWidget(ui->planTab);
        else
            ui->tabWidget->setCurrentWidget(ui->aboutTab);
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    setStateTxt("");
    setStateCol("");

    myip = strdup(settings.value("myip", "127.0.0.1").toByteArray().data());
    remoteip = strdup(settings.value("remoteip", "127.0.0.1").toByteArray().data());
    //log("myip="+settings.value("myip", "127.0.0.1")+", connecting to remoteip="+settings.value("remoteip", "127.0.0.1"));

    xpPortAvail=true;
    dbg("Binding UDP Socket for X-Plane ...");
    sock = new QUdpSocket(this);
    if (!sock->bind(32123))
    {
#ifdef _WIN32
        bool warn=true;
        if(QCoreApplication::arguments().count()>1)
            if(QCoreApplication::arguments().at(1)=="--dev")
                warn=false;
        if(warn==true)
            QMessageBox::warning(this, "YAACARS - Warning", "Could not bind to port 32123!\n(Is another copy of YAACARS already running?)\n\nYou're limited to use the FSUIPC connection only!", QMessageBox::Ok);
        err("Error: Could not bind to port 32123 - (Is another copy of YAACARS already running ?)");
        xpPortAvail=false;
        ui->cbVaComm->setCurrentIndex(FSUIPC);
#else
        QMessageBox::critical(this, "YAACARS - Error", "Error: Could not bind to port 32123!\n\nIs another copy of YAACARS already running?", QMessageBox::Ok);
        exit(1);
#endif
    }
    if(isCommXPUDP() && xpPortAvail)
    {
        connect(sock, SIGNAL(readyRead()), this, SLOT(parseXPUDP()));
        //setDRef(sock, "yaacars/connected", 0);
        //setDRef(sock, "yaacars/tracking", 0);
        //setDRef(sock, "yaacars/alive", 0);
        //setDRef(sock, "yaacars/fuel_kg", 0);
        //setDRef(sock, "yaacars/payload_kg", 0);
    }

    /////

    bool canSetPos=true;
    if(settings.value("mainWinX", false)==false || settings.value("mainWinY", false)==false)
        canSetPos=false;
    if(QCoreApplication::arguments().count()>1)
        if(QCoreApplication::arguments().at(1)=="--defaultpos")
            canSetPos=false;

    if(canSetPos)
    {
        this->move(settings.value("mainWinX", 200).toInt(), settings.value("mainWinY", 200).toInt());
        infobar->move(settings.value("infoBarX", 100).toInt(), settings.value("infoBarY", 100).toInt());
        if(settings.value("infoBarVis", true).toBool())
            on_btnStatusBarToggle_clicked();
    }
    else
    {
        infobar->move(this->x(), this->y());
    }
    /////

    dbg("Tailoring UI for FTW ...");
    va_uiState("INIT");
    ui->ImageAd->setStyleSheet("QLabel {border: 1px solid #000000;}");


    dbg("Init curl ...");
    va_curl_init();
    dbg("Connect to Sim ...");
    flightData.connState=CS_OFFLINE;
    connectToSim();

    connect(&_ui_InitTimer, SIGNAL(timeout()), this, SLOT(ui_InitTimer()));
    connect(&_ui_UpdTimer,  SIGNAL(timeout()), this, SLOT( ui_UpdTimer()));
    connect(&_ui_KmlTimer,  SIGNAL(timeout()), this, SLOT( ui_KmlTimer()));
    connect(&_va_UpdTimer,  SIGNAL(timeout()), this, SLOT( va_UpdTimer()));
    connect(&_nm_MesTimer,  SIGNAL(timeout()), this, SLOT( nm_MesTimer()));
    connect(&_comUpdTimer,  SIGNAL(timeout()), this, SLOT( comUpdTimer()));

    _ui_UpdTimer.start(ui_UpdTime);
    _ui_KmlTimer.start(ui_KmlTime);
    _va_UpdTimer.start(va_UpdTime);
    _nm_MesTimer.start(nm_MesTime);
    _comUpdTimer.start(comUpdTime);

    _ui_InitTimer.start(1);


    hwTimer=new QTime; //https://stackoverflow.com/questions/244646/get-elapsed-time-in-qt
    hwTimer->start();

    flightData.acarsState=AS_IDLE;
    createLuaLoad();
    log("Info: saving files to: "+BASEDIR);

    if(QFile::exists(BASEDIR+"/default.ysp"))
    {
        loadHtmlTemplate(BASEDIR+"/default.ysp");
        log("Loading custom default ScratchPad file: "+BASEDIR+"/default.ysp");
    }

    dbg("-------------------------------------- Init done --------------------------------------");
    log("===== Init done - ready ...");
    ui->labelUpdateInfo->setText("");

    //updateCheck
    uptodate=false;
    if(QCoreApplication::arguments().count()>1)
        if(QCoreApplication::arguments().at(1)=="--uptodate" || QCoreApplication::arguments().at(1)=="--dev")
            uptodate=true;
    if(!uptodate)
    {
        ui->labelUpdateInfo->setText("You already have the lastest 'release' version installed.");
        ui->labelUpdateInfo->setStyleSheet("");

        setDLbtn(btnDownloadRelease, false, "", "You already have the latest release version");
        setDLbtn(btnDownloadTesting, false, "", "There is currently no testing build available");
        update.fetchVersions();
    }
    else
    {
        ui->labelUpdateInfo->setText("Update check is disabled by command line!");
        ui->labelUpdateInfo->setStyleSheet("QLabel { background-color: yellow; color: black; }");

        setDLbtn(btnDownloadRelease, false, "", "Update check disabled");
        setDLbtn(btnDownloadTesting, false, "", "Update check disabled");
        wrn("Update check is disabled");
    }

    if(update.release!="")
    {
        if(build!=update.release)
        {
            ui->labelUpdateInfo->setText("There's an update available for download that NEEDs to be installed!");
            ui->labelUpdateInfo->setStyleSheet("QLabel { background-color: lime; color: black; }");

            setDLbtn(btnDownloadRelease, true, "QPushButton { color: black; background-color: "+mRGB_grn+"; }",
                     "Update to latest release "+update.release);
            log("There's an 'release' update available!");
        }
        if(update.release!=update.testing) //test build avail
        {
            if(build!=update.testing)
            {
                if(build!=update.release)
                    ui->labelUpdateInfo->setText("There's an update available for download that NEEDs to be installed!");
                else
                    ui->labelUpdateInfo->setText("There's an 'testing' build available (but you can also stay with the 'release' version)");
                ui->labelUpdateInfo->setStyleSheet("QLabel { background-color: lime; color: black; }");

                setDLbtn(btnDownloadTesting, true, "QPushButton { color: black; background-color: "+mRGB_yel+"; }",
                         "Update to current testing build "+update.testing);
                log("There's an 'testing' update available!");
            }
            else
            {
                ui->labelUpdateInfo->setText("Your testing build is up-to-date (or you can go back to the 'release' version again)");
                ui->labelUpdateInfo->setStyleSheet("");

                setDLbtn(btnDownloadTesting, false, "", "Your testing build is up-to-date");
                log("Your testing build is up-to-date");
            }
        }
    }
    else
    {
        if(!uptodate)
        {
            ui->labelUpdateInfo->setText("Update check failed, but you can still do FTW flights!");
            setDLbtn(btnDownloadRelease, false, "QPushButton { color: "+mRGB_red+"; }", "Update check failed");
            setDLbtn(btnDownloadTesting, false, "QPushButton { color: "+mRGB_red+"; }", "Update check failed");
            err("Update check failed");
        }
    }

    //notification
    //QString last=settings.value("lastVersion").toString();
    //if(build!=last)
    //{
    //    if(last!="") //first run
    //    {
    //        mOpenChangeLog();
    //        log("New version installed ... opening changelog in default browser ...");
    //    }
    //    settings.setValue("lastVersion", build);
    //}

    //airports
    airports.init(update.airport);
}


void MainWindow::ui_InitTimer()
{
    _ui_InitTimer.stop();

    if(isProtoXACARS())
    {
        on_loginButton_clicked();
    }
    else
    {
        //fake login
        va_uiState("LOGIN");
    }

    //QPair<double,double>* point;
    //point = airports.get("EKCH");
    //log("EKCH Lat="+QS2F(point->first,6)+" Lon="+QS2F(point->second,6));
}

MainWindow::~MainWindow()
{
    //if(isCommXPUDP() && xpPortAvail)
    //{
    //    setDRef(sock, "yaacars/connected", 0);
    //    setDRef(sock, "yaacars/tracking", 0);
    //    setDRef(sock, "yaacars/alive", 0);
    //    setDRef(sock, "yaacars/fuel_kg", 0);
    //    setDRef(sock, "yaacars/payload_kg", 0);
    //}

    delete mapserver;

    sock->close();
    delete sock;
    delete ui;
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("mainWinX", this->x());
    settings.setValue("mainWinY", this->y());
    settings.setValue("infoBarX", infobar->x());
    settings.setValue("infoBarY", infobar->y());
    settings.setValue("infoBarVis", infobar->isVisible());

    if(flightData.acarsState==AS_TRACKING)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "YAACARS",
                                                                    tr("Quit now? This will suspend you currently running flight ...\nAre you sure?"),
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            event->ignore();
        else
        {
            //
            infobar->close();
            //
            flightData.acarsState=AS_IDLE; //to reset lua state ...
            createLuaLoad();

            ///
            event->accept();
            log("---------- User quit while in flight");
            return;
        }
    }
    else
    {
        infobar->close();
        event->accept();
        log("---------- User quit");
        return;
    }
}

void MainWindow::on_loginButton_clicked()
{
    //qint32 ret;

    if(flightData.acarsState!=AS_IDLE)
        return;

    if(ui->username->text()=="" || ui->password->text()=="")
    {
        err("Please enter username and password first, to be able to login!");
        return;
    }

    log("Logging in ...");

    mRunVaCommand(va_login());

    va_uiState("LOGIN");
    log("Logged-in to FTW");
}

void MainWindow::on_getFlightplanButton_clicked()
{
    QSettings settings("YAACARS", "YAACARS", this);

    if(flightData.acarsState==AS_TRACKING) //IDLE or LOADED (to reload)
        return;

    if(isProtoXACARS())
    {
        //---try to paste FlightNo from Clipboard
        if(ui->flightNo->text()=="")
        {
            QClipboard *clipboard = QApplication::clipboard();
            QString clip = clipboard->text().trimmed();
            if(clip.length()==7)
                ui->flightNo->setText(clip);
        }
        ui->flightNo->setText(ui->flightNo->text().trimmed());
        ui->flightNo->setText(ui->flightNo->text().toUpper());

        //---Highlight FightNo field, if empty ...
        if(ui->flightNo->text()=="")
        {
            ui->flightNo->setFocus();
            if(ui->tabWidget->currentWidget()!=ui->planTab)
            {
                ui->tabWidget->setCurrentWidget(ui->planTab);
            }
            else
            {
                QMessageBox::warning(this, "YAACARS - Warning", "Please enter your Flight Number first!", QMessageBox::Ok);
            }
            return;
        }
    }

    //---Check YAACARS Version
    if(build!=update.release && update.release!="" && build!=update.testing && uptodate==false)
    {
        log("YAACARS version needs update!");
        QMessageBox::critical(this, "YAACARS",
            QString("You have to update to the latest version of YAACARS!"),
            QMessageBox::Ok);
        ui->tabWidget->setCurrentWidget(ui->aboutTab);
        return;
    }

    //---Check reusing last FlightNo
    if(isProtoXACARS() && ui->flightNo->text()==settings.value("lastFlightNumber", "FTWxxxx")) //flight "restarted" ... abort!
    {
        QMessageBox::critical(this, "YAACARS",
            QString("You already used this flight number ("+ui->flightNo->text()+") to start your last flight!\n\nYou will not be able to log the flight at the end in FTW!\n\nCancel and restart the flight in FTW to get a new fligt number!"),
            QMessageBox::Ok);
        return;
    }

    mRunVaCommand(va_getFlightplan());

    //---Check Airline FlightNo ...
    if(ui->flightNo->text().left(3)=="FTW" && ui->cbAirlineFlight->isChecked()==true)
    {
        qint32 ret = QMessageBox::information(this, "YAACARS",
                                          QString("You selected to do a flight for an airline, but your Flight Number shows a private FTW flight!"
                                                  "\n\nThis means, your flight is a private flight with a plane rented from the airline and it will be billed differently."
                                                  "\nNot all airlines allow you to use their plane privately, so please check back with the airline staff first!"
                                                  "\n\nAnother reason could be, that you loaded non-airline jobs with your flight!"
                                                  "\nThese jobs needs to be converted to airline jobs for you, so you can fly these jobs for your airline."),
                                        QMessageBox::Ok);
    }

    va_uiState("FLIGHTPLAN");
    log("Flightplan loaded");
}

void MainWindow::on_startButton_clicked()
{
//    qint64 tNow = time(nullptr);
    qint32 ret;
    QSettings settings("YAACARS", "YAACARS", this);

    if (flightData.connState!=CS_CONNECTED)
    {
        ret = QMessageBox::warning(this, "YAACARS",
                                          QString("Please connect properly to the sim first!"),
                                        QMessageBox::Ok);
        return;
    }

    if (!mIsResumed() && flightData.GS_kts > MIN_GS_KTS)
    {
        err("Flight cannot be started with "+QS2F(flightData.GS_kts,1)+" kts!");
        ret = QMessageBox::critical(this, "YAACARS",
                                          QString("Flight cannot be started ...\n\nYour groundspeed of "+QS2F(flightData.GS_kts,1)+" kts seems a little too high..."),
                                    QMessageBox::Ok);
        return;
    }

    if (!mIsResumed() && flightData.AGL_ft > MIN_AGL_FT)
    {
        err("Flight cannot be started with "+QS2F(flightData.AGL_ft,0)+" ft above the ground!");
        ret = QMessageBox::critical(this, "YAACARS",
                                          QString("Flight cannot be started ...\n\nYour AGL altitude of "+QS2F(flightData.AGL_ft,0)+" ft seems a little too high..."),
                                    QMessageBox::Ok);
        return;
    }

    //---Check depICAO field ...
    ui->arrIcao->setText(ui->arrIcao->text().toUpper());
    ui->depIcao->setText(ui->depIcao->text().toUpper());
    ui->altIcao->setText(ui->altIcao->text().toUpper());
    if(!mIsRescue())
    {
        if(ui->depIcao->text()=="" || ui->arrIcao->text()=="" || ui->altIcao->text()=="")
        {
            ret = QMessageBox::critical(this, "YAACARS",
                                  QString("The Departure, Arrival and Alternate Airport ICAOs must not be empty!\nPlease enter the ICAOs before starting the flight!"),
                                  QMessageBox::Ok);
            if (ret != QMessageBox::Yes) return;
        }
    }

    //---Find dep-ICAO coords ...
    //if(isProtoXACARS() && !mIsRescue()) //fuer FTWJSON haben wir die Coords bereits bekommen!
    //{
    //    QPair<double,double>* point;
    //    //try to ... find coords in airport list ...
    //    log("Looking up departure airport: "+ui->depIcao->text());
    //    point = airports.get(ui->depIcao->text());
    //    if (!point)
    //    {
    //        ret = QMessageBox::information(this, "YAACARS",
    //                              QString("Could not find the departure airport '"+ui->depIcao->text()+"' in the database.\n"
    //                                      "I can't check for your departure coorinates.\nMake sure you are at the right airport before continuing ..."),
    //                              QMessageBox::Yes, QMessageBox::No);
    //        if (ret != QMessageBox::Yes) return;
    //        coordsDEP.first=flightData.lat;
    //        coordsDEP.second=flightData.lon;
    //    }
    //    else
    //        coordsDEP = *point;
    //}

    //---Find arr-ICAO coords ...
    if(!mIsCoordsSet(ARR) && !mIsRescue()) //fuer FTWJSON haben wir die Coords bereits bekommen, solange nicht ICAO geändert!
    {
        QPair<double,double>* point;
        //try to ... find coords in airport list ...
        log("Looking up arrival airport: "+ui->arrIcao->text());
        point = airports.get(ui->arrIcao->text());
        if (!point)
        {
            log("Looking up arrival airport: "+ui->arrIcao->text());
            ret = QMessageBox::information(this, "YAACARS",
                                  QString("Could not find the arrival airport '"+ui->arrIcao->text()+"' in the database."),
                                  QMessageBox::Ok);
            return;
        }
        flightData.coordsARR = *point;
    }

    //---Find alt-ICAO coords ...
    if(ui->altIcao->text()==ui->arrIcao->text() || mIsRescue())
    {
        flightData.coordsALT.first=flightData.coordsARR.first;
        flightData.coordsALT.second=flightData.coordsARR.second;
    }
    else if(!mIsCoordsSet(ALT)) //fuer FTWJSON haben wir die Coords bereits bekommen, solange nicht ICAO geändert!
    {
        QPair<double,double>* point;
        //try to ... find coords in airport list ...
        log("Looking up alternate airport: "+ui->altIcao->text());
        point = airports.get(ui->altIcao->text());
        if (!point)
        {
            log("Looking up alternate airport: "+ui->altIcao->text());
            ret = QMessageBox::information(this, "YAACARS",
                                  QString("Could not find the alternate airport '"+ui->altIcao->text()+"' in the database."),
                                  QMessageBox::Ok);
            return;
        }
        flightData.coordsALT = *point;
    }

    //check coords existance
    if(!mIsRescue()) //wird serverseitig abgefangen!
    {
        if(!mIsCoordsSet(ARR) || !mIsCoordsSet(ALT))
        {
            ret = QMessageBox::information(this, "YAACARS",
                                  QString("Arrival or Alternate Airport coordinates are 0,0! Can't start the flight!"),
                                  QMessageBox::Ok);
            return;
        }
    }

    //---Check depCoords against current location
    double depDist;
    QString pos="your last known location";
    if(!mIsResumed())
    {
        depDist=greatcircle(flightData.coordsCURR, flightData.coordsDEP);
        pos="the departure airport "+ui->depIcao->text();
    }
    else
        depDist=greatcircle(flightData.coordsCURR, flightData.coordsLOADED);

    if (depDist > MAX_AIRPORT_DIST && !mIsRescue())
    {
        ret = QMessageBox::warning(this, "YAACARS",
                                          QString("Your distance of "+QS2F(depDist,0)+" nm from "+pos+" seems a little too high.\n"
                                                  "Are you sure you're at the right place?"),
                                    QMessageBox::Yes, QMessageBox::No); //for XACARS and JSON in case of resuming a flight
                                    //QMessageBox::Ok);
        if (ret != QMessageBox::Yes) return;
    }

    //---FUEL CHECK
    if (ui->cbCheckFuel->isChecked())
    {
        log("checkFuel is tuned on:");
        if(ui->fuel_lb->text()!="")
        {
            double fuelFTW = flightData.FUELftwTotal_lb;
            double fuelACF = flightData.FUELsimNew_lb;

            if(fuelACF < fuelFTW-fuelCheckDiff(fuelFTW) || fuelACF > fuelFTW+fuelCheckDiff(fuelFTW))
            {
                ret = QMessageBox::warning(this, "YAACARS",
                                                  QString("Fuel weight mismatch!\n\n"
                                                          "Sim: "+QS2F(fuelACF,0)+" lb / "+QS2F(fuelACF*mulLB2KG,0)+" kg\nFTW: "+QS2F(flightData.FUELftwTotal_lb,0)+" lb / "+QS2F(flightData.FUELftwTotal_lb*mulLB2KG,0)+" kg!"
                                                          //), QMessageBox::Ok);
                                                          "\n\nContinue anyway?"), QMessageBox::Yes, QMessageBox::No);
                if (ret != QMessageBox::Yes) return;
            }
            else
                log("   = Fuel weight ok!");
        }
        else
        {
            ret = QMessageBox::critical(this, "YAACARS",
                                              QString("No Fuel weight set to check against!\n"
                                                      "Please disable 'fuel checking' or enter the 'fuel weight' ..."),
                                       QMessageBox::Ok);
            if (ret != QMessageBox::Yes) return;
        }
    }
    else
        log("checkFuel is turned off");

    //---PAYLOAD CHECK
    if (ui->cbCheckPayload->isChecked())
    {
        log("checkPayload is tuned on:");
        if(ui->payload_lb->text()!="")
        {
            double payloadFTW = ui->payload_lb->text().toDouble();
            double payloadACF = flightData.PAYL_lb;

            if(payloadACF < payloadFTW-payloadCheckDiff(payloadFTW) || payloadACF > payloadFTW+payloadCheckDiff(payloadFTW))
            {
                ret = QMessageBox::warning(this, "YAACARS",
                                                  QString("Payload weight mismatch!\n\n"
                                                          "FTW: "+ui->payload_lb->text()+" lb / "+ui->payload_kg->text()+" kg\nSim: "+QS2F(flightData.PAYL_lb,0)+" lb / "+QS2F(flightData.PAYL_lb*mulLB2KG,0)+" kg!"
                                                       //), QMessageBox::Ok);
                                                       "\n\nContinue anyway?"), QMessageBox::Yes, QMessageBox::No);
                if (ret != QMessageBox::Yes) return;
            }
            else
                log("   = Payload weight ok!");
        }
        else
        {
            ret = QMessageBox::warning(this, "YAACARS",
                                              QString("No Payload weight set to check for!\n"
                                                      "Please disable payload checking or enter the PAX/Cargo weights ..."),
                                       QMessageBox::Ok);
            if (ret != QMessageBox::Yes) return;
        }
    }
    else
        log("checkPayload is turned off");

    ////////////////////////////////////////
    ////////////////////////////////////////

    mRunVaCommand(va_startFlight());

    settings.setValue("lastFlightNumber", ui->flightNo->text());

    va_uiState("STARTFLIGHT");

    createKmlRoute();

    //if(ui->cbAutorunSV->isChecked()==true)
    //    on_btnSkyVector_clicked();
    //if(ui->cbAutorunSB->isChecked()==true)
    //    on_btnSimBrief_clicked();
}

void MainWindow::on_endButton_clicked()
{
    qint32 ret;

    //---check for correct state (in case button has been activated already (in case of a 2nd landing))
    if (!mIsTimeSet(Takeoff) || !mIsTimeSet(Landing))
    {
        err("Flight cannot be ended - you have not landed yet!\n\n(Start or landing time is missing!)");
        ret = QMessageBox::critical(this, "YAACARS",
                                          QString("Flight cannot be ended - you have not landed yet ...!"),
                                    QMessageBox::Ok);
        return;
    }

    //---check for cooldown timer
    qint64 timeAfterLanding=time(nullptr)-flightData.timeLanding_secs;
    if (timeAfterLanding < TIME_TO_PIREP_SECS && !mIsRescue())
    {
        err("Flight cannot be ended just "+QS2I(timeAfterLanding)+" seconds after touching down!");
        ret = QMessageBox::critical(this, "YAACARS",
                                          QString("Flight cannot be ended just "+QS2I(timeAfterLanding)+" seconds after touchdown!\n\n(or get the pro version to get around this timeout :-) )"),
                                    QMessageBox::Ok);
        return;
    }

    //---check if standing still
    if(flightData.connState!=CS_CONNECTED) //if connection to sim is lost, then logging is also ok now
    if (flightData.GS_kts > MIN_GS_KTS)
    {
        err("Flight cannot be ended with "+QS2F(flightData.GS_kts,1)+" kts!");
        ret = QMessageBox::critical(this, "YAACARS",
                                          QString("Flight cannot be ended ...\n\nYour groundspeed of "+QS2F(flightData.GS_kts,1)+" kts seems a little too high..."),
                                    QMessageBox::Ok);
        return;
    }

    //---check if on ground
    if(flightData.connState!=CS_CONNECTED) //if connection to sim is lost, then logging is also ok now
    if (flightData.AGL_ft > MIN_AGL_FT)
    {
        err("Flight cannot be ended with "+QS2F(flightData.AGL_ft,0)+" ft above the ground!");
        ret = QMessageBox::critical(this, "YAACARS",
                                          QString("Flight cannot be ended ...\n\nYour AGL altitude of "+QS2F(flightData.AGL_ft,0)+" ft seems a little too high..."),
                                    QMessageBox::Ok);
        return;
    }

    //---Check arrCoords against current coords
    if(mIsCoordsSet(ARR) && !mIsRescue())
    {
        double arrDist = greatcircle(flightData.coordsCURR, flightData.coordsARR);
        if (arrDist > MAX_AIRPORT_DIST)
        {
            qint32 ret =
                QMessageBox::warning(this, "YAACARS",
                    QString("Your distance of "+QS2F(arrDist,0)+" nm from the arrival airport "+ui->arrIcao->text()+" seems a little too high.\n"
                            "Are you sure you're at the right place?\n\n(This is just a hint for you, not to log at the wrong airport.)"),
                    QMessageBox::Yes, QMessageBox::No);
            if (ret == QMessageBox::No) return;
        }
    }

    //---No fuel used
    if(flightData.FUELftwUsed_lb<=0)
    {
        qint32 ret =
            QMessageBox::warning(this, "YAACARS",
                QString("You didn't use any fuel!!\n"
                        "\nEnding your flight now will end up in a high risk of not getting paid for the flight!\n"
                        "You you still want to end your flight now?"),
                QMessageBox::Yes, QMessageBox::No);
        if (ret == QMessageBox::No) return;
    }

    //Not enough miles flown
    double distTotal, done;
    if(mIsCoordsSet(ARR)) // ARR is set
    {
        distTotal = greatcircle(flightData.coordsDEP, flightData.coordsARR);
        done = flightData.distance_nm / distTotal;
        if(done < 0.8)
        {
            qint32 ret =
                QMessageBox::warning(this, "YAACARS",
                    QString("Your flown distance ("+QS2F(flightData.distance_nm,0)+" nm) is less than the minimum distance between DEP and ARR airport ("+QS2F(flightData.distance_nm,0)+" nm)!\n"
                            "You you still want to end your flight now?"),
                    QMessageBox::Yes, QMessageBox::No);
            if (ret == QMessageBox::No) return;
        }
    }


    {   //// time sanity checks

        if(flightData.timeFlight_secs > MAX_FLIGHTTIME_SECS)
        {
            wrn("Flight time has been reduced to maximum flight time!");
            flightData.timeRemoved_secs = MAX_FLIGHTTIME_SECS - flightData.timeFlight_secs;
            flightData.timeFlight_secs=MAX_FLIGHTTIME_SECS;
        }

        if(mIsTimeSet(Takeoff) && mIsTimeSet(Landing))
        {
            if(flightData.timeBlock_secs > flightData.timeFlight_secs+TIME_TO_TAKEOFF_SECS+TIME_AFTER_LANDING_SECS)
            {
                wrn("Block time has been reduced to max takeoff+landing time!");
                flightData.timeRemoved_secs = flightData.timeFlight_secs+TIME_TO_TAKEOFF_SECS+TIME_AFTER_LANDING_SECS - flightData.timeBlock_secs;
                flightData.timeBlock_secs=flightData.timeFlight_secs;
            }
        }
        else if (mIsTimeSet(Takeoff))
        {
            if(flightData.timeBlock_secs > flightData.timeFlight_secs+TIME_TO_TAKEOFF_SECS)
            {
                wrn("Block time has been reduced to max takeoff time!");
                flightData.timeRemoved_secs = flightData.timeFlight_secs+TIME_TO_TAKEOFF_SECS - flightData.timeBlock_secs;
                flightData.timeBlock_secs=flightData.timeFlight_secs;
            }
        }
//        else
//        {
//            wrn("Block time has been reduced to flight time!");
//            flightData.timeBlock_secs=flightData.timeFlight_secs;
//        }
    }

    ret = QMessageBox::information(this, "YAACARS",
                             "\nReal\t\tTime:\t"+QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timeBlock_secs+flightData.timePause_secs+flightData.timeRemoved_secs), Qt::UTC).toString("hh:mm:ss")+
                             "\nBlock\t\tTime:\t"+QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timeBlock_secs), Qt::UTC).toString("hh:mm:ss")+
                             "\nRemoved\tTime:\t"+QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timeRemoved_secs), Qt::UTC).toString("hh:mm:ss")+
                             "\nFlight\t\tTime:\t"+QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timeFlight_secs), Qt::UTC).toString("hh:mm:ss")+
                             "\nPause\t\tTime:\t"+QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timePause_secs), Qt::UTC).toString("hh:mm:ss")+
                             "\nMiles\t\tflown:\t"+QS2I(flightData.distance_nm)+
                             "\n\nDo you want to end your flight now?"
                             , QMessageBox::Yes, QMessageBox::No);
    if (ret == QMessageBox::No) return;

    log("Submitting flight data ...");
    va_uiState("ENDFLIGHT");

    mRunVaCommand(va_endFlight());

    va_uiState("IDLE");

    QMessageBox::information(this, "YAACARS",
                             "Data sent successfully.\n\nPlease, end and bill your flight in FTW now!"
                             , QMessageBox::Ok);

    setStateTxt("");
    setStateCol("");
}

void MainWindow::on_resetButton_clicked()
{
    qint32 ret = QMessageBox::warning(this, "YAACARS",
                                      QString("Are you sure you want to cancel and reset your current flight now?\nYou can then only start the flight again at the departure location!\n\n(If your sim or computer crashed, you can also resume your current flight without the need to reset it ...)"),
                                      QMessageBox::Yes, QMessageBox::No);
    if (ret == QMessageBox::No) return;

    ret = QMessageBox::warning(this, "YAACARS",
                                      QString("you are going to reset the current flight and loose all progress - is that correct?"),
                                      QMessageBox::Yes, QMessageBox::No);
    if (ret == QMessageBox::No) return;

    mRunVaCommand(va_resetFlight());

    cancelFlight("Flight canceled by user input");
}

void MainWindow::on_suspendButton_clicked()
{
    if (flightData.acarsState==AS_TRACKING)
    {
        qint32 ret = QMessageBox::warning(this, "YAACARS",
                                          QString("Are you sure you want to suspend your current flight now?\n\nYou can then resume the flight later on ..."),
                                          QMessageBox::Yes, QMessageBox::No);
        if (ret == QMessageBox::No) return;
    }

    cancelFlight("Flight suspended by user input");
}

void MainWindow::cancelFlight(QString abortCond)
{
    if (flightData.acarsState==AS_TRACKING)
    {
       err("The flight tracking has been aborted for the following reason: "+abortCond+". You will not be able to log this flight at arrival! Sorry!");
       setStateTxt("Flight tracking aborted: "+abortCond);
       setStateCol("orange");

       //QMessageBox::critical(this, "YAACARS",
        //                           QString("The flight tracking has been aborted for the following reason:\n\n"+abortCond+"\n\nYou will not be able to log this flight at arrival!\n\nSorry!"),
        //                                  QMessageBox::Ok);
    }
    else
    {
        wrn("Current flightplanning was canceled!");
        va_uiState("STARTFLIGHT"); //to reset the edit boxes coloring only!
    }
    va_uiState("IDLE");

    if(isCommXPUDP() && xpPortAvail)
    {
        //setDRef(sock, "yaacars/connected", 1);
        //setDRef(sock, "yaacars/tracking", 0);
        //setDRef(sock, "yaacars/alive", 0);
        //setDRef(sock, "yaacars/fuel_kg", 0);
        //setDRef(sock, "yaacars/payload_kg", 0);
    }
}

static QTime conSimPressStart;
void MainWindow::on_conSim_pressed()
{
    conSimPressStart = QTime::currentTime();
}
void MainWindow::on_conSim_released()
{
    if (std::abs(QTime::currentTime().msecsTo(conSimPressStart)) > 2000)
    {
        IPDialog* ipd = new IPDialog(this);
        ipd->show();
    }
    else
    {
        connectToSim();
    }
}

void MainWindow::connectToSim()
{
    //log("Connecting to sim ...");

    if(isCommXPUDP())
        initXPUDP();
    else if(isCommFSUIPC())
        initFSUIPC();
    else
        wrn("no connectToSim() for "+ui->cbVaComm->currentText());
}

void MainWindow::comUpdTimer()
{
    if(isCommFSUIPC())
        parseFSUIPC();
}

void MainWindow::nm_MesTimer()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//double MainWindow::calcRealTime()
//{
//    return flightData.blockTime_secs+flightData.pauseTime_secs;
//}
//
//double MainWindow::calcBlockTime()
//{
//    return flightData.blockTime_secs;
//}
//
//double MainWindow::calcFlightTime()
//{
//    return flightData.flightTime_secs;
//}
//double MainWindow::calcPauseTime()
//{
//    return flightData.pauseTime_secs;
//}

void MainWindow::ui_UpdTimer()
{
    static uint16_t trackingCount = 0;
    qint64 tnow = time(nullptr);

    trackingCount++;

    if (flightData.timeStamp < tnow - 5 && flightData.rref < tnow - 5)
    {
        // Disconnected
        if (flightData.connState != CS_OFFLINE)
        {
            //log("[ConnectionState] (time&&rref) transition to CS_OFFLINE (from " +QS2I(flightData.connState)+ ")");
            //ui->conSim->setStyleSheet("QPushButton { color: "+mRGB_red+"; }");
            flightData.connState = CS_OFFLINE;
            ui->btnCopyLatLonToClip->setEnabled(false);

            connectToSim();
            return;
        }
    }
    else if (flightData.timeStamp < tnow - 5 || flightData.rref < tnow - 5)
    {
        // Partly connected
        if (flightData.connState != CS_OFFLINE)
        {
            //log("[ConnectionState] (time||rref) transition to CS_OFFLINE (from " +QS2I(flightData.connState)+ ")");
            //ui->conSim->setStyleSheet("QPushButton { color: "+mRGB_red+"; }");
            flightData.connState = CS_OFFLINE;
            ui->btnCopyLatLonToClip->setEnabled(false);

            connectToSim();
            return;
        }
    }
    else
    {
        // Connected
        if (flightData.connState != CS_CONNECTED)
        {
            //log("[ConnectionState] transition to CS_CONNECTED (from " +QS2I(flightData.connState)+ ")");
            //ui->conSim->setStyleSheet("QPushButton { color: "+mRGB_grn+"; }");
            flightData.connState = CS_CONNECTED;
            ui->btnCopyLatLonToClip->setEnabled(true);
        }
    }

    if (flightData.AGL_ft < MIN_AGL_FT && mIsTimeSet(Landing)) //when we was in the air already
    {
        qint64 timeAfterLanding=time(nullptr)-flightData.timeLanding_secs;
        if (timeAfterLanding < TIME_TO_PIREP_SECS && !mIsRescue())
        {
            ui->endButton->setText("End Flight in "+QS2I(TIME_TO_PIREP_SECS-timeAfterLanding)+" secs");
            ui->endButton->setEnabled(false);
        }
        else
        {
            ui->endButton->setText("End Flight");
            ui->endButton->setEnabled(true);
        }
    }
    else
    {
        ui->endButton->setText("End Flight");
        ui->endButton->setEnabled(false);
    }

    //if(flightData.acarsState==IDLE)
    //    trackingCount=0;

    //if(isCommXPUDP() && xpPortAvail)
    //{
    //    setDRef(sock, "yaacars/connected", flightData.connState==CS_CONNECTED?CS_CONNECTED:CS_OFFLINE);
    //    setDRef(sock, "yaacars/tracking", flightData.acarsState);
    //    if(flightData.acarsState==AS_TRACKING && flightData.isSuspended==true && flightData.isPaused==false) //nur wenn im tracking, keine Pause, aber suspended!
    //        setDRef(sock, "yaacars/alive", 0);
    //    else
    //        setDRef(sock, "yaacars/alive", trackingCount);
    //    setDRef(sock, "yaacars/fuel_kg", ui->fuel_kg->text().toInt());
    //    setDRef(sock, "yaacars/payload_kg", ui->payload_kg->text().toInt());
    //}

    if (flightData.acarsState==AS_TRACKING)
    {
        if (flightData.connState == CS_CONNECTED && !flightData.isSuspended)
        {
            setStateCol("lime");
            setStateTxt("Flight tracking is running ...");
            ui->inhibitWarnSoundButton->setVisible(false);
        }
        else
        {
            if((trackingCount%2)==0)
                setStateCol("yellow");
            else if(!flightData.isPaused) //blinken, ausser wenn pause
                setStateCol("orange");

            if (flightData.connState != CS_CONNECTED)
            {
                setStateTxt("Connection lost ...");
            }
            else if(flightData.isSuspended)
            {
                if(flightData.isAborted)
                    setStateTxt("Your flight might have been not started or aborted ... CHECK FTW TERMINAL ...");
                else if(flightData.isPaused)
                    setStateTxt("Flight tracking suspended due to pause ...");
                else if(flightData.simRate>1)
                    setStateTxt("Flight tracking suspended due to time compression ...");
                else if(flightData.isRefuelled)
                    ; //ignore here
                else
                    setStateTxt("Flight tracking suspended due to unknown reason ...");
            }
            else //not needed, but just to fill the else!
            {
                setStateTxt("Connection lost ...");
            }
        }
    } //not tracking flight
    else
    {
        //if(flightData.acarsState==AS_LOADED) //FP loaded, but not yet started the flight
        {
            if (flightData.connState != CS_CONNECTED)
            {
                setStateTxt("Connecting to sim ...");
                if((trackingCount%2)==0)
                    setStateCol("yellow");
                else
                    setStateCol("gold");
            }
            else
            {
                if(flightData.GS_kts>5)
                {
                    if(!ui->cbDisableWarnSound->isChecked() && !flightData.inhibitFlightNotStartedWarning && !flightData.inhibitFlightNotStartedWarnSnd)
                    {
                        bool isDev=false;
                        if(QCoreApplication::arguments().count()>1)
                            if(QCoreApplication::arguments().at(1)=="--dev")
                                isDev=true;

                        if(!isDev)
                            QApplication::beep();
                        ui->inhibitWarnSoundButton->setVisible(true);
                    }

                    setStateTxt("!!! Flight tracking not started !!!");
                    if(!flightData.inhibitFlightNotStartedWarning)
                    {
                        if(((trackingCount)%2)==0)
                        {
                            setStateCol("magenta");
                            ui->inhibitWarnSoundButton->setStyleSheet("QPushButton { font: bold; }");
                        }
                        else
                        {
                            setStateCol("orange");
                            ui->inhibitWarnSoundButton->setStyleSheet("");
                        }
                    }
                    else
                        setStateCol("yellow");
                }
                else
                {
                    setStateTxt("");
                    setStateCol("");
                    ui->inhibitWarnSoundButton->setVisible(false);
                }
            }
        }
        //else
        //{
        //    setStateTxt("");
        //    setStateCol("");
        //}
    }


    ////////////////////////////////////////////////////////

    if (flightData.connState == CS_CONNECTED && flightData.acarsState!=AS_TRACKING && ui->cbCheckFuel->isChecked() && ui->fuel_lb->text()!="")
    {
        double fuelFTW  = flightData.FUELftwTotal_lb;
        double fuelACF  = flightData.FUELsimCur_lb;
        double fuelDiff = fuelACF-fuelFTW;

        ui->lbPlaneFuelKg->setText(QS2F(fuelACF*mulLB2KG,0));
        ui->lbPlaneFuelLb->setText(QS2F(fuelACF,0));
        dbg("Fuel: ACF="+QS2F(fuelACF,1)+" FTW="+QS2F(fuelFTW,1)+ "Diff="+QS2F(fuelDiff,1));

        if(fuelACF < fuelFTW-fuelCheckDiff(fuelFTW) || fuelACF > fuelFTW+fuelCheckDiff(fuelFTW))
        {
            ui->lbPlaneFuelKg->setText(ui->lbPlaneFuelKg->text()+" (d="+QS2F(fuelDiff*mulLB2KG,0)+")");
            ui->lbPlaneFuelLb->setText(ui->lbPlaneFuelLb->text()+" (d="+QS2F(fuelDiff,0)+")");
            ui->lbPlaneFuelKg->setStyleSheet("QLabel { background-color: orange; color: black; }");
            ui->lbPlaneFuelLb->setStyleSheet("QLabel { background-color: orange; color: black; }");
        }
        else
        {
            ui->lbPlaneFuelKg->setStyleSheet("QLabel { background-color: "+mRGB_grn+"; color: black; }");
            ui->lbPlaneFuelLb->setStyleSheet("QLabel { background-color: "+mRGB_grn+"; color: black; }");
        }
    }
    else
    {
        ui->lbPlaneFuelKg->setText("");
        ui->lbPlaneFuelLb->setText("");
        ui->lbPlaneFuelKg->setStyleSheet("");
        ui->lbPlaneFuelLb->setStyleSheet("");
    }

    if(flightData.connState == CS_CONNECTED && flightData.acarsState!=AS_TRACKING && ui->cbCheckPayload->isChecked() && ui->payload_lb->text()!="")
    {
        double payloadFTW = ui->payload_lb->text().toDouble();
        double payloadACF = flightData.PAYL_lb;
        double payloadDiff = payloadACF-payloadFTW;

        ui->lbPlanePayloadKg->setText(QS2F(flightData.PAYL_lb*mulLB2KG,0));
        ui->lbPlanePayloadLb->setText(QS2F(flightData.PAYL_lb,0));
        dbg("Payload: ACF="+QS2F(payloadACF,1)+" FTW="+QS2F(payloadFTW,1)+ "Diff="+QS2F(payloadDiff,1));

        if(payloadACF < payloadFTW-payloadCheckDiff(payloadFTW) || payloadACF > payloadFTW+payloadCheckDiff(payloadFTW))
        {
            ui->lbPlanePayloadKg->setText(ui->lbPlanePayloadKg->text()+" (d="+QS2F(payloadDiff*mulLB2KG,0)+")");
            ui->lbPlanePayloadLb->setText(ui->lbPlanePayloadLb->text()+" (d="+QS2F(payloadDiff,0)+")");
            ui->lbPlanePayloadKg->setStyleSheet("QLabel { background-color: orange; color: black; }");
            ui->lbPlanePayloadLb->setStyleSheet("QLabel { background-color: orange; color: black; }");
        }
        else
        {
            ui->lbPlanePayloadKg->setStyleSheet("QLabel { background-color: "+mRGB_grn+"; color: black; }");
            ui->lbPlanePayloadLb->setStyleSheet("QLabel { background-color: "+mRGB_grn+"; color: black; }");
        }
    }
    else
    {
        ui->lbPlanePayloadKg->setText("");
        ui->lbPlanePayloadLb->setText("");
        ui->lbPlanePayloadKg->setStyleSheet("");
        ui->lbPlanePayloadLb->setStyleSheet("");
    }

    ////////////////////////////////////////////////////////
    if (flightData.acarsState==AS_TRACKING)
    {
        double distDEP = greatcircle(flightData.coordsCURR, flightData.coordsDEP);
        double distARR = greatcircle(flightData.coordsCURR, flightData.coordsARR);
//        double distALT = greatcircle(flightData.coordsCURR, flightData.coordsALT);
        double distTotal, done;

        if(mIsCoordsSet(ARR)) // ARR is set
            distTotal = greatcircle(flightData.coordsDEP, flightData.coordsARR);
        else
            distTotal=0;

        if(distTotal!=0 && mIsCoordsSet(ARR) && !mIsRescue()) //!div0 and DST is set
        {
            done = distARR / distTotal;
            ui->percDoneTarget->setText(QS2I(fabs(1.0 - done) * 100));
        }
        else
            ui->percDoneTarget->setText(TEXT_NO_DATA);

        if(distTotal!=0 && mIsCoordsSet(ARR) && !mIsRescue()) //!div0 and DST is set
        {
            done = flightData.distance_nm / distTotal;
            if(done < 0.9)
                setDistPercCol("red");
            else if(done < 0.97)
                setDistPercCol("orange");
            else if(done < 1.0)
                setDistPercCol("yellow");
            else
                setDistPercCol("green");

            setDistPercTxt(QS2I((done) * 100));
        }
        else
        {
            ui->percDoneReal->setStyleSheet("");
            ui->percDoneReal->setText(TEXT_NO_DATA);
        }

        mShowDistance(DEP);
        mShowDistance(ARR);
        //mShowDistance(ALT);

        if(!mIsRescue())
            setDistNmTxt(QS2I(flightData.distance_nm),"|>"+QS2I(flightData.distance_nm)+" >|"+QS2I(distTotal-flightData.distance_nm));
        else
            setDistNmTxt("","");


        ////////////////////////////
        QString sR=QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timeBlock_secs+flightData.timePause_secs+flightData.timeRemoved_secs), Qt::UTC).toString("hh:mm:ss");
        sRealTime->setText("Real:"+sR);
        QString sB=QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timeBlock_secs), Qt::UTC).toString("hh:mm:ss");
        sBlockTime->setText("Blk:"+sB);
        QString sF=QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timeFlight_secs), Qt::UTC).toString("hh:mm:ss");
        sFlightTime->setText("Flt:"+sF);
        QString sP=QDateTime::fromTime_t(static_cast<unsigned int>(flightData.timePause_secs), Qt::UTC).toString("hh:mm:ss");

        if((trackingCount%60)==0)
            dbg("Real "+sR+" // Block "+sB+" // Flight "+sF+" // Pause "+sP);
    }
    else
    {
        setDistPercCol("");
        setDistPercTxt(TEXT_NO_DATA);

        setDistNmTxt(TEXT_NO_DATA,TEXT_NO_DATA);

        ui->percDoneTarget->setText(TEXT_NO_DATA);
        ui->distDEP->setText(TEXT_NO_DATA);
        ui->distARR->setText(TEXT_NO_DATA);
        //ui->distALT->setText(TEXT_NO_DATA);
    }

    // ~~~~ FTW
    //ui->zfw->setText(QS2F(flightData.ZFW_lb, 0)+" lb / "+QS2F(flightData.ZFW_lb*mulLB2KG, 0)+" kg");
    // ~~~~ FOB

    //if(flightData.isRefuelled && flightData.acarsState!=AS_IDLE)
    //{
        //if(((trackingCount)%2)==0)
        //{
            setFuelOBTxt(QS2F(flightData.FUELsimNew_lb, 0)+" lb / "+QS2F(flightData.FUELsimNew_lb*mulLB2KG, 0)+" kg",
                         QS2F(flightData.FUELftwTotal_lb, 0)+" lb / "+QS2F(flightData.FUELftwTotal_lb*mulLB2KG, 0)+" kg");
        //    setFuelOBCol("lime");
        //}
        //else
        //{
        //    setFuelOBTxt(QS2F(flightData.FUELsimNew_lb, 0)+" lb / "+QS2F(flightData.FUELsimNew_lb*mulLB2KG, 0)+" kg");
        //    setFuelOBCol("lightblue");
        //}
    //}
    //else
    //{
    //    setFuelOBTxt(QS2F(flightData.FUELftwTotal_lb, 0)+" lb / "+QS2F(flightData.FUELftwTotal_lb*mulLB2KG, 0)+" kg");
    //    setFuelOBCol("");
    //}

    // ~~~~ FU
    if (flightData.acarsState==AS_TRACKING)
    {
        setFuelUTxt(QS2F(flightData.FUELftwUsed_lb, 0)+" lb / "+QS2F(flightData.FUELftwUsed_lb*mulLB2KG, 0)+" kg");
    }
    else
        setFuelUTxt("---");


    ui->altitude->setText(QS2F(flightData.AGL_ft, 0)+" ft");

    ui->simRate->setText(QS2F(flightData.simRate,0));
    if(flightData.simRate!=1)
        ui->simRate->setStyleSheet("QLineEdit { background-color: orange; color: black; }");
    else
        ui->simRate->setStyleSheet("");

    //ui->ias->setText(QS2F(flightData.IAS_kts,1)+" kts");
    ui->gs->setText( QS2F(flightData.GS_kts, 1)+" kts");
    //7ui->vs->setText( QS2F(flightData.VS_fpm, 0)+" fpm");

    //ui->flaps->setText(QS2F(flightData.flaps, 0));

    ui->lat->setText(QS2F(flightData.coordsCURR.first, 6)+"°");
    ui->lon->setText(QS2F(flightData.coordsCURR.second, 6)+"°");
}

void MainWindow::ui_KmlTimer()
{
    //dbg("KML Timer");
    if(ui->cbKmlPos->isChecked()==false)
        return;
    if(flightData.timeStamp < time(nullptr) -5) //we got recent data from sim
        return;

    QString filename = BASEDIR+"/currentPosition.kml";
    dbg("Creating KML file '"+filename +"' ...");
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
        stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">" << endl;
        stream << "<Document>" << endl;
        stream << "	<Style id=\"sn_icon56\">" << endl;
        stream << "		<IconStyle>" << endl;
        stream << "			<scale>1.5</scale>" << endl;
        stream << "			<Icon>" << endl;
        stream << "				<href>http://maps.google.com/mapfiles/kml/pal2/icon56.png</href>" << endl;
        stream << "			</Icon>" << endl;
        stream << "		</IconStyle>" << endl;
        stream << "	</Style>" << endl;
        stream << "	<Camera>" << endl;
        stream << "		<longitude>" << QS2F(flightData.coordsCURR.second,6) << "</longitude>" << endl;
        stream << "		<latitude>" << QS2F(flightData.coordsCURR.first,6) << "</latitude>" << endl;
        stream << "		<altitude>15000</altitude>" << endl;
        stream << "	</Camera>" << endl;
        stream << "	<Placemark>" << endl;
        stream << "		<Style>" << endl;
        stream << "			<IconStyle>" << endl;
        stream << "				<color>ff0000ff</color>" << endl;
        stream << "				<heading>" << QS2I(flightData.HDG) << "</heading>" << endl;
        stream << "			</IconStyle>" << endl;
        stream << "		</Style>" << endl;
        stream << "		<styleUrl>#sn_icon56</styleUrl>" << endl;
        stream << "		<Point>" << endl;
        stream << "			<coordinates>" << QS2F(flightData.coordsCURR.second,6) <<  "," << QS2F(flightData.coordsCURR.first,6) << ",0</coordinates>" << endl;
        stream << "		</Point>" << endl;
        stream << "	</Placemark>" << endl;
        stream << "</Document>" << endl;
        stream << "</kml>" << endl;
        file.close();
    }
    //else
    //    err("Can't open KML file for writing ...");
}

void MainWindow::createKmlRoute()
{
    if (!mIsCoordsSet(DEP) && !mIsCoordsSet(ARR))
    {
        wrn("KML-Route not generated. DEP or ARR coords not found.");
        return;
    }

    QString filename = BASEDIR+"/currentRoute.kml";
    dbg("Creating KML file '"+filename +"' ...");
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
        stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">" << endl;
        stream << "<Document>" << endl;

        stream << "	<Style id=\"sn_iconDep\">" << endl;
        stream << "		<IconStyle>" << endl;
        stream << "			<scale>1.5</scale>" << endl;
        stream << "			<Icon>" << endl;
        stream << "				<href>http://maps.google.com/mapfiles/kml/paddle/D.png</href>" << endl;
        stream << "			</Icon>" << endl;
        stream << "		</IconStyle>" << endl;
        stream << "	</Style>" << endl;

        stream << "	<Style id=\"sn_iconArr\">" << endl;
        stream << "		<IconStyle>" << endl;
        stream << "			<scale>1.5</scale>" << endl;
        stream << "			<Icon>" << endl;
        stream << "				<href>http://maps.google.com/mapfiles/kml/paddle/A.png</href>" << endl;
        stream << "			</Icon>" << endl;
        stream << "		</IconStyle>" << endl;
        stream << "	</Style>" << endl;

        //stream << "	<Camera>" << endl;
        //stream << "		<longitude>" << QS2F(flightData.lon,6) << "</longitude>" << endl;
        //stream << "		<latitude>" << QS2F(flightData.lat,6) << "</latitude>" << endl;
        //stream << "		<altitude>15000</altitude>" << endl;
        //stream << "	</Camera>" << endl;

        stream << "	<Placemark>" << endl;
        stream << "		<styleUrl>#sn_iconDep</styleUrl>" << endl;
        stream << "		<Point>" << endl;
        stream << "			<coordinates>" << QS2F(flightData.coordsDEP.second,6) <<  "," << QS2F(flightData.coordsDEP.first,6) << ",0</coordinates>" << endl;
        stream << "		</Point>" << endl;
        stream << "	</Placemark>" << endl;

        stream << "	<Placemark>" << endl;
        stream << "		<styleUrl>#sn_iconArr</styleUrl>" << endl;
        stream << "		<Point>" << endl;
        stream << "			<coordinates>" << QS2F(flightData.coordsARR.second,6) <<  "," << QS2F(flightData.coordsARR.first,6) << ",0</coordinates>" << endl;
        stream << "		</Point>" << endl;
        stream << "	</Placemark>" << endl;

        stream << "	<Placemark>" << endl;
        stream << "     <LineString>" << endl;
        stream << "         <tessellate>1</tessellate>" << endl;
        stream << "			<coordinates>" << QS2F(flightData.coordsDEP.second,6) <<  "," << QS2F(flightData.coordsDEP.first,6) << ",0 " << QS2F(flightData.coordsARR.second,6) <<  "," << QS2F(flightData.coordsARR.first,6) << ",0 </coordinates>" << endl;
        stream << "     </LineString>" << endl;
        stream << "     <Style>" << endl;
        stream << "         <LineStyle>" << endl;
        stream << "             <color>#8000FF00</color>" << endl;
        stream << "             <width>5</width>" << endl;
        stream << "         </LineStyle>" << endl;
        stream << "     </Style>" << endl;
        stream << "	</Placemark>" << endl;

        stream << "</Document>" << endl;
        stream << "</kml>" << endl;
        file.close();
    }
    //else
    //    err("Can't open KML file for writing ...");
}

void MainWindow::createLuaLoad()
{
    QString filename = BASEDIR+"/yaacars.lua";
    dbg("Creating LUA file '"+filename +"' ...");
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "YAACARS = {}" << endl;
        stream << "YAACARS['tracking']="+QS2I(flightData.acarsState==2?1:0) << endl;
        stream << "YAACARS['plane_name']=\""+ui->acName->text() << "\"" << endl;
        stream << "YAACARS['fuel_kg']="+QS2I(ui->fuel_kg->text().toInt()) << endl;
        stream << "YAACARS['payload_kg']="+QS2I(ui->payload_kg->text().toInt()) << endl;
        file.close();
        dbg("yaacars.lua updated!");
    }
}

////////////////////////////////////////////////////////////////////////////
void MainWindow::setStateTxt(QString txt)
{
    ui->labelState->setText(txt);
    infobar->setStateTxt(txt);
}
void MainWindow::setStateCol(QString txt)
{
    ui->labelState->setStyleSheet(txt==""?"":"QLabel { background-color: "+txt+"; color: black; }");
    infobar->setStateCol(txt==""?"":"QLabel { background-color: "+txt+"; color: black; }");
}

/////////////////////////////////////////////

void MainWindow::setDistNmTxt(QString txt1, QString txt2)
{
    ui->distFlown->setText(txt1);
    infobar->setDistNmTxt(txt2);
}
void MainWindow::setDistNmCol(QString txt)
{
    ui->distFlown->setStyleSheet(txt==""?"":"QLineEdit { background-color: "+txt+"; color: black; }");
    infobar->setDistNmCol(txt==""?"":"QLabel { background-color: "+txt+"; color: black; }");
}

void MainWindow::setDistPercTxt(QString txt)
{
    ui->percDoneReal->setText(txt);
    infobar->setDistPercTxt(txt);
}
void MainWindow::setDistPercCol(QString txt)
{
    ui->percDoneReal->setStyleSheet(txt==""?"":"QLineEdit { background-color: "+txt+"; color: black; }");
    infobar->setDistPercCol(txt==""?"":"QLabel { background-color: "+txt+"; color: black; }");
}

/////////////////////////////////////////////

void MainWindow::setFuelOBTxt(QString sim,QString ftw)
{
    ui->fobSIM->setText(sim);
    ui->fobFTW->setText(ftw);
    infobar->setFuelOBTxt(ftw);
}
//void MainWindow::setFuelOBCol(QString txt)
//{
//    ui->fob->setStyleSheet(txt==""?"":"QLineEdit { background-color: "+txt+"; color: black; }");
//    infobar->setFuelOBCol(txt==""?"":"QLabel { background-color: "+txt+"; color: black; }");
//}

void MainWindow::setFuelUTxt(QString txt)
{
    ui->fused->setText(txt);
    infobar->setFuelUTxt(txt);
}
void MainWindow::setFuelUCol(QString txt)
{
    ui->fused->setStyleSheet(txt==""?"":"QLineEdit { background-color: "+txt+"; color: black; }");
    infobar->setFuelUCol(txt==""?"":"QLabel { background-color: "+txt+"; color: black; }");
}

////////////////////////////////////////////////////////////////////////////
void MainWindow::updatePayload()
{
    if(ui->pax_lb->text()=="" || ui->cargo_lb->text()=="")
        ui->payload_lb->setText("");
    else
        ui->payload_lb->setText(QS2F(ui->pax_lb->text().toDouble()+ui->cargo_lb->text().toDouble(),0));

    if(ui->pax_kg->text()=="" || ui->cargo_kg->text()=="")
        ui->payload_kg->setText("");
    else
        ui->payload_kg->setText(QS2F((ui->pax_kg->text().toDouble()+ui->cargo_kg->text().toDouble()),0));
}

void MainWindow::on_pax_lb_textChanged(const QString &arg1)   { UNUSED(arg1); updatePayload(); }
void MainWindow::on_pax_kg_textChanged(const QString &arg1)   { UNUSED(arg1); updatePayload(); }
void MainWindow::on_cargo_lb_textChanged(const QString &arg1) { UNUSED(arg1); updatePayload(); }
void MainWindow::on_cargo_kg_textChanged(const QString &arg1) { UNUSED(arg1); updatePayload(); }

////////////////////////////////////////////////////////////////////////////
void MainWindow::on_fuel_kg_editingFinished()
{
    //bei Fuel auch format "1.467 kg" erlauben
    ui->fuel_kg->setText(ui->fuel_kg->text().replace(" ","").replace(".","").replace("kg",""));

    if(ui->fuel_kg->text()=="")
        ui->fuel_lb->setText("");
    else
        ui->fuel_lb->setText(QS2F(ui->fuel_kg->text().toDouble()*mulKG2LB,0));

    createLuaLoad();
}
void MainWindow::on_fuel_lb_editingFinished()
{
    //bei Fuel auch format "1.467 lb" erlauben
    ui->fuel_lb->setText(ui->fuel_lb->text().replace(" ","").replace(".","").replace("lb",""));

    if(ui->fuel_lb->text()=="")
        ui->fuel_kg->setText("");
    else
        ui->fuel_kg->setText(QS2F(ui->fuel_lb->text().toDouble()*mulLB2KG,0));

    createLuaLoad();
}

void MainWindow::on_pax_kg_editingFinished()
{
    if(ui->pax_kg->text()=="")
        ui->pax_lb->setText("");
    else
        ui->pax_lb->setText(QS2F(ui->pax_kg->text().toDouble()*mulKG2LB,0));

    createLuaLoad();
}
void MainWindow::on_pax_lb_editingFinished()
{
    if(ui->pax_lb->text()=="")
        ui->pax_kg->setText("");
    else
        ui->pax_kg->setText(QS2F(ui->pax_lb->text().toDouble()*mulLB2KG,0));

    createLuaLoad();
}

void MainWindow::on_flightNo_returnPressed()
{
    on_getFlightplanButton_clicked();
}

////////////////////////////////////////////////////////////////////////////
void MainWindow::on_cbLoggerDbg_stateChanged(int arg1)
{
    uiLogger->dbg=arg1;
}
void MainWindow::on_cbLoggerLog_stateChanged(int arg1)
{
    uiLogger->log=arg1;
}
void MainWindow::on_cbLoggerWrn_stateChanged(int arg1)
{
    uiLogger->wrn=arg1;
}
void MainWindow::on_cbLoggerErr_stateChanged(int arg1)
{
    uiLogger->err=arg1;
}

void MainWindow::on_cbDevServerScript_stateChanged(int arg1)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("devServerScript", arg1);
}

////////////////////////////////////////////////////////////////////////////
void MainWindow::on_username_textChanged(const QString &arg1)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("callsign", arg1);

    if(arg1=="" || arg1!=arg1.trimmed())
        ui->username->setStyleSheet("QLineEdit { background-color: orange; color: black; }");
    else
        ui->username->setStyleSheet("");

    mapserver->username=ui->username->text();
}

void MainWindow::on_password_textChanged(const QString &arg1)
{
    //QSettings settings("YAACARS", "YAACARS", this);
    //settings.setValue("password", arg1);
    //this is done by the "save" button now only!
}

void MainWindow::on_cbKmlPos_stateChanged(int arg1)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("kmlPos", (bool)arg1);
    ui_KmlTimer(); //trigger once to update immediately
}
void MainWindow::on_cbKmlRte_stateChanged(int arg1)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("kmlRte", (bool)arg1);
}

void MainWindow::on_cbCheckFuel_stateChanged(int arg1)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("checkFuel", (bool)arg1);
}

void MainWindow::on_cbCheckPayload_stateChanged(int arg1)
{
    if(xpPortAvail==true)
    {
        QSettings settings("YAACARS", "YAACARS", this);
        settings.setValue("checkWeight", (bool)arg1);
    }
}

void MainWindow::on_cbAirlineFlight_stateChanged(int arg1)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("checkAirlineFlight", (bool)arg1);
}


//void MainWindow::on_cbAutorunSV_stateChanged(int arg1)
//{
//    QSettings settings("YAACARS", "YAACARS", this);
//    settings.setValue("autorunSkyVector", (bool)arg1);
//}
//
//void MainWindow::on_cbAutorunSB_stateChanged(int arg1)
//{
//    QSettings settings("YAACARS", "YAACARS", this);
//    settings.setValue("autorunSimBrief", (bool)arg1);
//}

void MainWindow::on_cbDisableWarnSound_stateChanged(int arg1)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("disableWarnSound", (bool)arg1);
}

void MainWindow::on_btnLoginDataSave_clicked()
{
    QSettings settings("YAACARS", "YAACARS", this);
    QByteArray str=ui->password->text().toUtf8();
    QByteArray hash = QCryptographicHash::hash(str, QCryptographicHash::Sha512);
    passwordHash=hash.toHex();
    passwordDate=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    settings.setValue("passhash", passwordHash);
    settings.setValue("passdate", passwordDate);

    ui->password->setText("");
    ui->password->setVisible(false);
    ui->btnLoginDataEdit->setVisible(true);
    ui->btnLoginDataSave->setVisible(false);

    ui->password->setStyleSheet("");
}

void MainWindow::on_btnLoginDataEdit_clicked()
{
    ui->password->setVisible(true);
    ui->btnLoginDataEdit->setVisible(false);
    ui->btnLoginDataSave->setVisible(true);

    ui->password->setStyleSheet("QLineEdit { background-color: orange; color: black; }");
}

////////////////////////////////////////////////////////////////////////////
void MainWindow::on_cbVaComm_currentIndexChanged(int index)
{
#ifdef _WIN32
    if(index==XPUDP && !xpPortAvail)
    {
        ui->cbVaComm->setCurrentIndex(FSUIPC);
        return;
    }

    if(xpPortAvail==true)
    {
        QSettings settings("YAACARS", "YAACARS", this);
        settings.setValue("CommIdx", index);
    }

    //ui->conSim->setStyleSheet("QPushButton { color: "+mRGB_red+"; }");
    //flightData.connState = CS_UNKNOWN;

    va_uiState("INIT");
    connectToSim();
#else
    ui->cbVaComm->setCurrentIndex(XPUDP);
#endif
}

void MainWindow::on_cbVaProto_currentIndexChanged(int index)
{
    QSettings settings("YAACARS", "YAACARS", this);
    settings.setValue("ProtoIdx", index);
    va_uiState("INIT");
}

////////////////////////////////////////////////////////////////////////////

void MainWindow::on_btnDownloadRelease_clicked()
{
#ifdef _WIN32
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "YAACARS",
                                                                tr("Do you want me to try to autoupdate?\n"
                                                                   "This will download and install the latest version.\n"
                                                                   "Autoupdate needs the original pathes 'YAACARS\\win32' to work!\n"
                                                                   "\n"
                                                                   "Otherwise the manual download will be issued by the default browser."),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        QString file="\""+QDir::toNativeSeparators(QCoreApplication::applicationDirPath())+"\\YAACARS-update-release.bat\"";
        QProcess exec;
        exec.startDetached(file);
        //system(QString(file).toLatin1());
    }
    else
#endif
    {
        QUrl url;
        url.setUrl(URL_DOWNLOAD);
        QDesktopServices::openUrl(url);

        //QMessageBox::information(this, "YAACARS", "The download link has been sent to your browser.\n\nI will quit now ...", QMessageBox::Ok);
    }
    exit(100);
}

void MainWindow::on_btnDownloadTesting_clicked()
{
#ifdef _WIN32
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "YAACARS",
                                                                tr("Do you want me to try to autoupdate?\n"
                                                                   "This will download and install the latest version.\n"
                                                                   "Autoupdate needs the original pathes 'YAACARS\\win32' to work!\n"
                                                                   "\n"
                                                                   "Otherwise the manual download will be issued by the default browser."),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        QString file="\""+QDir::toNativeSeparators(QCoreApplication::applicationDirPath())+"\\YAACARS-update-testing.bat\"";
        QProcess exec;
        exec.startDetached(file);
        //system(QString(file).toLatin1());
    }
    else
#endif
    {
        QUrl url;
        url.setUrl(URL_DOWNLOAD);
        QDesktopServices::openUrl(url);

        //QMessageBox::information(this, "YAACARS", "The download link has been sent to your browser.\n\nI will quit now ...", QMessageBox::Ok);
    }

    exit(100);
}

void MainWindow::on_changeLogButton_clicked()
{
    mOpenChangeLog();
}

//void MainWindow::on_btnMetar_clicked()
//{
//    QString dep, arr, alt;
//    dep=ui->depIcao->text();
//    arr=ui->arrIcao->text();
//    alt=ui->altIcao->text();
//
//    if(dep!="")
//    {
//        QUrl url;
//        QString icaos=dep;
//        if(arr!="")
//            icaos+="+"+arr;
//        if(alt!="")
//            icaos+="+"+alt;
//        url.setUrl("https://www.aviationweather.gov/metar/data");
//        url.setQuery("ids="+icaos+"&format=raw&date=&hours=0");
//
//        QDesktopServices::openUrl(url);
//    }
//}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void MainWindow::on_arrIcao_editingFinished()
{
    ui->arrIcao->setText(ui->arrIcao->text().toUpper());
    if(ui->altIcao->text()=="")
        ui->altIcao->setText(ui->arrIcao->text());

    flightData.coordsARR.first=0; //find coords in apdb when starting flight
    flightData.coordsARR.second=0;
}
void MainWindow::on_altIcao_editingFinished()
{
    ui->altIcao->setText(ui->altIcao->text().toUpper());

    flightData.coordsALT.first=0; //find coords in apdb when starting flight
    flightData.coordsALT.second=0;
}

void MainWindow::on_tailNumber_editingFinished()
{
    ui->tailNumber->setText(ui->tailNumber->text().toUpper());
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void MainWindow::on_btnFpVATSIM_clicked()
{
    if(ui->depIcao->text()!="" && ui->arrIcao->text()!="") //when dep/arr is set
    {
        //https://cert.vatsim.net/fp/file.php?
        // 1=I
        // 2=
        // 3=T/B734/F
        // 4=430
        // 5=EPBY
        // 6=2100
        // 7=FL100
        // 8=DCT%2520SL%2520Z95%2520GRU%2520N133%2520LODNI
        // 9=EPGD
        // 10a=00
        // 10b=22
        // 11=
        // 12a=01
        // 12b=46
        // 13=EPPO
        // 14=
        // 15=
        // voice=/V/

        QUrl url;
        QString query;
        query= "1=";
        if(ui->flightType->currentIndex()==0)
            query+="I";
        else
            query+="V";

        query+="&2="+ui->edtVatCallsign->text();

        query+="&5="+ui->depIcao->text();
        query+="&9="+ui->arrIcao->text();
        query+="&13="+ui->altIcao->text();

        query+="&8="+ui->edtVatRoute->text().replace(" ", "%20");

        query+="&3="+ui->edtVatAcType->text();
        query+="&4="+ui->edtVatCrzTas->text();
        query+="&6="+ui->edtVatDepTime->text();
        query+="&7="+ui->edtVatFlightLevel->text();

        query+="&10a="+ui->edtVatEnrouteHrs->text();
        query+="&10b="+ui->edtVatEnrouteMins->text();

        query+="&12a="+ui->edtVatFobHrs->text();
        query+="&12b="+ui->edtVatFobMins->text();

        query+="&voice="+ui->cbVatVoice->currentText();

        url.setUrl("https://cert.vatsim.net/fp/file.php");
        url.setQuery(query);

        QDesktopServices::openUrl(url);
    }
}



void MainWindow::on_btnStatusBarToggle_clicked()
{
    //infobar->setWindowFlags(Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint|Qt::WindowStaysOnTopHint);
    //infobar->setWindowFlags(Qt::X11BypassWindowManagerHint|Qt::WindowStaysOnTopHint);
    infobar->setWindowFlags(Qt::WindowStaysOnTopHint);

    //int x=infobar.width()*0.7;
    //int y=infobar.height()*0.7;

    infobar->show();
   //infobar->exec();
}

//inhibit des gesamten Warnsystems
void MainWindow::on_btnInhibit_clicked()
{
    flightData.inhibitFlightNotStartedWarning=true;
}

//inhibit des Warn-Sounds, z.B. bei resume des flights
void MainWindow::on_inhibitWarnSoundButton_clicked()
{
    flightData.inhibitFlightNotStartedWarnSnd=true;
    ui->inhibitWarnSoundButton->setVisible(false);
}

void MainWindow::on_btnCopyLatLonToClip_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(QS2F(flightData.coordsCURR.first,6)+" "+QS2F(flightData.coordsCURR.second,6));
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void MainWindow::loadHtmlTemplate(QString fname)
{
    QFile file(fname);

    file.open(QFile::ReadOnly | QFile::Text);

    QTextStream ReadFile(&file);
    ui->textEdit->setText(ReadFile.readAll());

    file.close();
}

void MainWindow::saveHtmlTemplate(QString fname)
{
    QFile file(fname);

    file.open(QFile::WriteOnly | QFile::Text);

    QTextStream WriteFile(&file);
    WriteFile << ui->textEdit->toHtml();

    file.close();
}

void MainWindow::on_btnLoadDefaultHtmlTemplate_clicked()
{
    if(QFile::exists(BASEDIR+"/default.ysp"))
    {
        loadHtmlTemplate(BASEDIR+"/default.ysp");
        log("Loading custom default ScratchPad file: "+BASEDIR+"/default.ysp");
    }
    else
        err("Default ScratchPad file: "+BASEDIR+"/default.ysp not found!");
}

void MainWindow::on_btnLoadHtmlTemplate_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Load Scratch Pad from ..."), BASEDIR,
            tr("Scratch Pad (*.ysp);;All Files (*)"));
    //QFileDialog::getOpenFileName(fileName);

    if(fileName!="")
    {
        loadHtmlTemplate(fileName);
    }
}

void MainWindow::on_btnSaveHtmlTemplate_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Scratch Pad to ..."), BASEDIR,
            tr("Scratch Pad (*.ysp);;All Files (*)"));

    if(fileName!="")
    {
        saveHtmlTemplate(fileName);
    }
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if(ui->btnLoginDataSave->isVisible()==true)
    {
        QMessageBox::warning(this, "YAACARS - Warning", "You have to save your password first!", QMessageBox::Ok);
    }
}

void MainWindow::on_ImageAd_linkActivated(const QString &link)
{

}

void MainWindow::on_btnShowLatLon_clicked()
{
    if(ui->lat->text()!="" && ui->lon->text()!="")
    {
        //https://skyvector.com/?fpl=LFQQ%20D272C%20D131O%20CIV%20SISGA%20BUPAL%20+51.054_+006.373%20+51.485_+008.973%20WRB%20UMKUK%20D210F%20DLE%20EDDV

        QUrl url;
        QString query;
        query= "fpl=+"+ui->lat->text().replace("°","")+"_+00"+ui->lon->text().replace("°","");
        url.setUrl("https://skyvector.com/");
        url.setQuery(query);

        QDesktopServices::openUrl(url);
    }
}

void MainWindow::on_btnLiveMap_clicked()
{
    mOpenLiveMap();
}

void MainWindow::on_btnHelp_clicked()
{
    mOpenHelp();
}
