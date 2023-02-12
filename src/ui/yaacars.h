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

//#define DEBUG

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <QLabel>
#include <math.h>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QChar>
#include <QClipboard>
#include <QCryptographicHash>

#include <QDir>
#include <QStandardPaths>
//#include "src/lib/crc32/crc32.h"

#include "src/ui/infobar.h"

#include "src/va/airports.h"
#include "src/va/update.h"
#include "src/sim/data.h"
#include "src/ui/logger.h"

#include "src/lib/curl/curl.h"
#include "src/ui/mapserver.h"

#define BASEDIR QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/YAACARS"
//#define BASEDIR QStandardPaths::writableLocation(QStandardPaths::DataLocation)

#define mRunVaCommand(cmd) \
{ \
    QString va_ret = cmd; \
    if (va_ret != "") \
    { \
        err(va_ret); \
        QMessageBox::critical(this, "YAACARS - Server Message", va_ret, QMessageBox::Ok); \
        return; \
    } \
}

#define URL_RESUME_FAQ  ((QString)"http://www.ftw-sim.de/wiki/doku.php?id=yaacars:resume_faq")
#define URL_DOWNLOAD    ((QString)"http://www.ftw-sim.de/yaacars/download/index.php")

#define checkCreateBasePath() \
{ \
    QDir dir(BASEDIR); \
    if (!dir.exists())  \
        dir.mkpath("."); \
}

#define mOpenChangeLog() \
{ \
    QUrl url; \
    url.setUrl("http://www.ftw-sim.de/wiki/doku.php"); \
    url.setQuery("id=yaacars:changelog"); \
    QDesktopServices::openUrl(url); \
}

#define mOpenLiveMap() \
{ \
    QUrl url; \
    url.setUrl("http://www.ftw-sim.de/yaacars/livemap/"); \
    QDesktopServices::openUrl(url); \
}

#define mOpenHelp() \
{ \
    QUrl url; \
    url.setUrl("http://www.ftw-sim.de/wiki/doku.php"); \
    url.setQuery("id=yaacars:start"); \
    QDesktopServices::openUrl(url); \
}

//#define mIsRescue() (ui->flightNo->text().mid(0,3)=="RET")
//#define mIsRescue()     (flightData.missionTypeFTW==3/*Rescue*/)
#define mIsRescue()     (flightData.missionTypeFTW==3/*Rescue*/ || flightData.missionTypeFTW==4/*Mission*/)

#define mIsResumed()    (flightData.isResumedFlightFTW==1/*resumed*/)

#define mIsCoordsSet(TYPE) (flightData.coords##TYPE.first!=0 && flightData.coords##TYPE.second!=0)

#define mIsTimeSet(TYPE) (flightData.time##TYPE##_secs>=0)

//--- DEFINITIONs
#define fuelCheckDiff(a) a*0.03 /*3%*/
#define payloadCheckDiff(a) a*0.03 /*3%*/

#define mulLB2KG 0.453592
#define mulKG2LB (1/mulLB2KG)
#define mulGAL2KG (3.2256)
#define mulGAL2LB (mulGAL2KG*mulKG2LB)
#define mulMTR2FT (3.28084)
#define mulMPS2FPS (mulMTR2FT*60) //sec->min
#define mulMPS2KTS (1.94384449)
#define mulmmHG2hPA (33.863886666667)
#define mulhPAmmHG (1/mulmmHG2hPA)

#define MAX_AIRPORT_DIST 2.0 //was 10.0 in 1.x

#define MAX_FLIGHTTIME_SECS (20/*hrs*/*60/*mins*/*60)

#define TIME_TO_TAKEOFF_SECS    (30/*mins*/*60)   //BLOCK TIME LIMITER
#define TIME_AFTER_LANDING_SECS (15/*mins*/*60)   //BLOCK TIME LIMITER
//#define TIME_TO_TAKEOFF_SECS    (3/*mins*/*60)   //BLOCK TIME LIMITER
//#define TIME_AFTER_LANDING_SECS (3/*mins*/*60)   //BLOCK TIME LIMITER

#define CURL_TIMEOUT_GENERAL_SECS (15L)     //ALSO REDEFINED IN UPDATE.CPP!
#define CURL_TIMEOUT_DOWNLOAD_SECS (30L)    //ALSO REDEFINED IN AIRPORTS.CPP!
#define CURL_TIMEOUT_LIVEUPDATE_SECS (10L)

#define FUEL_CHECK_OFFSET_LBS 5 //FF7x7 :-( und wohl auch die A320 :-((

#define TIME_TO_PIREP_SECS 60
#define MIN_GS_KTS 6.0
#define MIN_AGL_FT 20.0

#define kgPerPax 70.0

#define ui_UpdTime 1000  /*in millisecs! */
#define ui_KmlTime 5000 /*in millisecs! */
#define va_UpdTime 15000 /*in millisecs! was 60000, for reporting to live server */
#define va_UpdTimeRsq 5000 /* 5secs for RETT and MIS */
#define va_UpdOfflineTime 3600000 /*in millisecs! 3600 secs => 5min */
#define nm_MesTime 500  /*in millisecs! for NOT USED ATM! */
#define comUpdTime 1000  /*in millisecs! for FSUIPC polling */

#define HW_CYCLE_TIME 1000 /*ms for HW-Timer*/

#define refuelReportDelaySecs (3 * comUpdTime/1000) //in secs! (at least the FSUIPC poll time!)
#define limitReportDelaySecs (60)                   //in secs!

#define wayPointTrackLowSec (60)
#define wayPointTrackHighSec (5)

#define mRGB_grn QString("lime")
#define mRGB_yel QString("yellow")

//#define mRGB_red QString("red")
//#define mRGB_red QString("OrangeRed")
#define mRGB_red QString("Tomato")

#define TEXT_NO_DATA "---"

//--- MACROs
#undef err
#undef dbg
#undef log
#undef wrn
#define err(txt) uiLogger->write_err(QString(LOGGER_SECTION),txt)
#define dbg(txt) uiLogger->write_dbg(QString(LOGGER_SECTION),txt)
#define log(txt) uiLogger->write_log(QString(LOGGER_SECTION),txt)
#define wrn(txt) uiLogger->write_wrn(QString(LOGGER_SECTION),txt)

#define QS2F(a,b) QString::number(a, 'f', b)
#define QS2I(a) QString::number(a, 'f', 0)

#define isProtoXACARS() (static_cast<eProto>(ui->cbVaProto->currentIndex())==XACARS)
#define isProtoFTWJSON() (static_cast<eProto>(ui->cbVaProto->currentIndex())==FTWJSON)

#define isCommXPUDP() (static_cast<eComm>(ui->cbVaComm->currentIndex())==XPUDP)
#define isCommFSUIPC() (static_cast<eComm>(ui->cbVaComm->currentIndex())==FSUIPC)

#define setDLbtn(btn,en,lay,txt) ui->btn->setEnabled(en), ui->btn->setStyleSheet(lay), ui->btn->setText(txt)

#define float2double(var) static_cast<double>(var)
#define float2bool(var) static_cast<bool>(var)

#define UNUSED(x) (void)(x)

#define mZeroFlightData \
{ \
    flightData.coordsDEP.first=0;                              \
    flightData.coordsDEP.second=0;                             \
    flightData.coordsARR.first=0;                              \
    flightData.coordsARR.second=0;                             \
    flightData.coordsALT.first=0;                              \
    flightData.coordsALT.second=0;                             \
    flightData.coordsCURR.first=0;                             \
    flightData.coordsCURR.second=0;                            \
    flightData.coordsLAST.first=0;                             \
    flightData.coordsLAST.second=0;                            \
    flightData.coordsLOADED.first=0;                           \
    flightData.coordsLOADED.second=0;                          \
    flightData.acarsState=AS_IDLE;                             \
    flightData.connState=CS_UNKNOWN;                           \
    flightData.inhibitFlightNotStartedWarning=false;           \
    flightData.inhibitFlightNotStartedWarnSnd=false;           \
    flightData.isSuspended=false;                              \
    flightData.isPaused=false;                                 \
    flightData.isAborted=false;                                \
    flightData.isRefuelled=false;                              \
    flightData.isResumedFlightFTW=0;                           \
    flightData.missionTypeFTW=-1;                              \
    flightData.trackingIdFTW="";                               \
    flightData.trackingCount=0;                               \
    flightData.timeStamp=0;                                    \
    flightData.rref=0;                                         \
    flightData.simRate=0;                                      \
    flightData.pitch=0;                                        \
    flightData.bank=0;                                         \
    flightData.HDG=0;                                          \
    flightData.VS_fpm=0;                                       \
    flightData.IAS_kts=0;                                      \
    flightData.TAS_kts=0;                                      \
    flightData.GS_kts=0;                                       \
    flightData.ASL_ft=0;                                       \
    flightData.AGL_ft=0;                                       \
    flightData.PAYL_lb=0;                                      \
    flightData.FUELftwTotal_lb=0;                              \
    flightData.FUELftwUsed_lb=0;                               \
    flightData.FUELsimCur_lb=0;                                \
    flightData.FUELsimNew_lb=0;                                \
    flightData.distance_nm=0;                                  \
    flightData.timeBlock_secs=0;                               \
    flightData.timeFlight_secs=0;                              \
    flightData.timePause_secs=0;                               \
    flightData.timeTakeoff_secs=-1;                            \
    flightData.timeLanding_secs=-1;                            \
    flightData.timeRemoved_secs=0;                             \
    flightData.winddeg=0;                                      \
    flightData.windknots=0;                                    \
    flightData.oat=0;                                          \
}

#define mShowDistance(t) \
{ \
    if(flightData.coords##t.first!=0 && flightData.coords##t.second!=0) \
        ui->dist##t->setText(QS2F(dist##t, 1)+" nm"); \
    else \
        ui->dist##t->setText(TEXT_NO_DATA); \
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Logger *uiLogger, *vaLogger;
    QHostAddress myip, remoteip;
    MapServer *mapserver;

    //Crc32 *crcgen;

    InfoBar *infobar;

    void setStateTxt(QString);
    void setStateCol(QString);
    //////////////////////////
    void setDistNmTxt(QString,QString);
    void setDistNmCol(QString);

    void setDistPercTxt(QString);
    void setDistPercCol(QString);
    //////////////////////////
    void setFuelOBTxt(QString,QString);
    //void setFuelOBCol(QString);

    void setFuelUTxt(QString);
    void setFuelUCol(QString);

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool uptodate;
    bool xpPortAvail;

    QStringList *cpNames;
    QString cpName;

    QTime *hwTimer;

    void connectToSim();

private slots:
    void closeEvent (QCloseEvent *event);
    void parseXPUDP();

    void ui_InitTimer();
    void ui_UpdTimer();
    void ui_KmlTimer();
    void va_UpdTimer();
    void nm_MesTimer();
    void comUpdTimer();

    void on_startButton_clicked();
    void on_endButton_clicked();

    void on_username_textChanged(const QString &arg1);
    void on_password_textChanged(const QString &arg1);

    void on_conSim_pressed();
    void on_conSim_released();

    //void on_loadJsonButton_clicked();

    void on_cbLoggerDbg_stateChanged(int arg1);
    void on_cbLoggerLog_stateChanged(int arg1);
    void on_cbLoggerWrn_stateChanged(int arg1);
    void on_cbLoggerErr_stateChanged(int arg1);

    void on_fuel_kg_editingFinished();
    void on_fuel_lb_editingFinished();
    void on_pax_kg_editingFinished();
    void on_pax_lb_editingFinished();

    void on_resetButton_clicked();

    void on_cbVaComm_currentIndexChanged(int index);
    void on_cbVaProto_currentIndexChanged(int index);

    void on_btnDownloadRelease_clicked();
    void on_btnDownloadTesting_clicked();

    void on_pax_lb_textChanged(const QString &arg1);

    void on_pax_kg_textChanged(const QString &arg1);

    void on_cargo_lb_textChanged(const QString &arg1);

    void on_cargo_kg_textChanged(const QString &arg1);

    void on_flightNo_returnPressed();

    void on_arrIcao_editingFinished();

    void on_tailNumber_editingFinished();

    void on_altIcao_editingFinished();

    void on_getFlightplanButton_clicked();

    void on_loginButton_clicked();

    void on_cbAirlineFlight_stateChanged(int arg1);

    void on_btnStatusBarToggle_clicked();

    void on_cbKmlPos_stateChanged(int arg1);

    void on_cbCheckPayload_stateChanged(int arg1);

    void on_cbCheckFuel_stateChanged(int arg1);

    //void on_cbAutorunSV_stateChanged(int arg1);
    //void on_cbAutorunSB_stateChanged(int arg1);
    //void on_btnMetar_clicked();
    //void on_btnSkyVector_clicked();
    //void on_btnSimBrief_clicked();

    void on_cbKmlRte_stateChanged(int arg1);

    void on_cbDisableWarnSound_stateChanged(int arg1);

    void on_btnInhibit_clicked();

    void on_btnCopyLatLonToClip_clicked();

    void on_btnLoadHtmlTemplate_clicked();

    void on_btnFpVATSIM_clicked();

    void on_btnLoginDataSave_clicked();

    void on_cbDevServerScript_stateChanged(int arg1);

    void on_btnLoginDataEdit_clicked();

    void on_suspendButton_clicked();

    void on_changeLogButton_clicked();

    void on_inhibitWarnSoundButton_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_ImageAd_linkActivated(const QString &link);

    void on_btnShowLatLon_clicked();

    void on_btnSaveHtmlTemplate_clicked();

    void on_btnLoadDefaultHtmlTemplate_clicked();

    void on_btnLiveMap_clicked();

    void on_btnHelp_clicked();

private:
    Ui::MainWindow *ui;

    QLabel *sRealTime, *sBlockTime, *sFlightTime;
    QUdpSocket* sock;
    QTimer _ui_InitTimer, _ui_UpdTimer, _ui_KmlTimer, _va_UpdTimer, _comUpdTimer, _nm_MesTimer;

    T_Status flightData;
    QString build;
    QString passwordHash;
    QString passwordDate;

    void initXPUDP();
    void closeXPUDP();
    //bool FSUIPC_connected;
    void initFSUIPC();
    void closeFSUIPC();
    void parseFSUIPC();

    //Helper
    void cancelFlight(QString abortCond); //for suspend/exit/reset
    void updatePayload();
    void createLuaLoad();
    void createKmlRoute();

    void loadHtmlTemplate(QString fname);
    void saveHtmlTemplate(QString fname);

    void calcFlightData(int diff_msec);
    //void updateDone();

    //QJsonArray events;
    //QJsonArray tracks;

    //void newEvent(QString desc, bool critical = 0);
    //void evtStartTaxi();
    //void evtTakeoff();
    //void evtStartClimb();
    //void evtCruise();
    //void evtStartDescend();
    //void evtLanding();
    //void evtShutdown();
    //void evtEngineStart(int e);
    //void evtEngineStop(int e);
    //void critOverspeed();
    //void critStall();
    //void critPaused();
    //void critUnpaused();
    //void critBeaconOff();
    //void critIasBelow10k();
    //void critLightsBelow10k();
    //void critLightsAbove10k();
    //void critTaxiNoLights();
    //void critTakeoffNoLights();
    //void critLandingNoLights();
    //void critWrongAirport();
    //void critTaxiSpeed(double speed);
    //void critQnhTakeoff();
    //void critQnhLanding();
    //void critCrashed(QString reason);

    //T_Maximum maximum;
    //qint32 trackId, eventId, critEvents;

    //$$ struct Mistakes {
    //$$     bool crash, beaconOff, iasLow, lightsLow, lightsHigh, overspeed, pause,
    //$$         refuel, slew, taxiLights, takeoffLights, landingLights,
    //$$         landingAirport, stall, taxiSpeed, qnhTakeoff, qnhLanding;
    //$$
    //$$     void reset() {
    //$$         crash = beaconOff = iasLow = lightsLow = lightsHigh = overspeed = pause
    //$$                 = refuel = slew = taxiLights = takeoffLights = landingLights
    //$$                 = landingAirport = stall = taxiSpeed = qnhTakeoff = qnhLanding = 0;
    //$$     }
    //$$ } mistakes;
    //$$
    //$$ struct T_tracking
    //$$ {
    //$$     QPair<double,double> lastCoords;
    //$$     //unsigned long long lastTimeStamp;
    //$$     //unsigned long long incCounter;
    //$$     //unsigned long long pkgCounter;
    //$$
    //$$     void reset() {
    //$$         lastCoords.first = lastCoords.second = 0;
    //$$         //lastTimeStamp = pkgCounter = incCounter = 0;
    //$$     }
    //$$ } tracking;

	/////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////	
	/////////////////////////////////////////////////////////	

    Airports airports;
    Update update;

    void va_uiState(QString state);
    void va_curl_init();
    void vaLoggerDebug(QString pre, QString txt);
    void vaLoggerReport(QString pre, QString txt);

    QString va_login();
    QString va_getFlightplan();
    QString va_startFlight();
    QString va_endLiveFlight();
    QString va_endFlight();
    QString va_report(QString eventType);
    QString va_resetFlight();

    QString baseUrl;
    QString liveUrl;
    //eProto vaProto;
    //eComm vaComm;
    CURL* gCurl;
    QByteArray gCurlData;

    void va_ftw_json_curl_init();
    QJsonObject va_ftw_json_curl_fetch(QString query, QJsonObject jObj, long timeout);
    QString va_ftw_json_login();
    QString va_ftw_json_getFlightplan();
    QString va_ftw_json_startFlight();
    QString va_ftw_json_endLiveFlight();
    QString va_ftw_json_endFlight();
    QString va_ftw_json_report(QString eventType);
    QString va_ftw_json_resetFlight();

    void va_ftw_xacars_curl_init();
    QString va_ftw_xacars_curl_fetch(QString url, QString query, long timeout);
    QString va_ftw_xacars_login();
    QString va_ftw_xacars_getFlightplan();
    QString va_ftw_xacars_startFlight();
    QString va_ftw_xacars_endLiveFlight();
    QString va_ftw_xacars_endFlight();
    QString va_ftw_xacars_pause(int mode);
    QString va_ftw_xacars_report(QString eventType);

};

void memcat(char** ptr, const char* data, int len);
void memcat(char** ptr, const char* data);
void memcat(char** ptr, const int data);
void sendDRef(QUdpSocket* sock, const char *name, qint32 id, qint32 freq = 20);
void setDRef(QUdpSocket* sock, const char* name, float val);
double greatcircle(QPair<double,double> src, QPair<double,double> tgt);
double greatcircle(double srcLat, double srcLon, QPair<double,double> tgt);
double greatcircle(QPair<double,double> src, double tgtLat, double tgtLon);
double greatcircle(double srcLat, double srcLon, double tgtLat, double tgtLon);
QString latToXACARS(double coord);
QString lonToXACARS(double coord);

#endif // MAINWINDOW_H
