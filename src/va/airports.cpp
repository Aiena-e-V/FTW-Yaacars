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

#include "airports.h"
#include "src/lib/curl/curl.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#define CURL_TIMEOUT_DOWNLOAD_SECS (30L)

//////////////////////////////////////////////////////////////////////////////////////
#define LOGGER_SECTION "APTDB"

#define BASEDIR QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/YAACARS"
//#define BASEDIR QStandardPaths::writableLocation(QStandardPaths::DataLocation)

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    QByteArray* gCurlData = static_cast<QByteArray*>(userp);

    gCurlData->append(static_cast<const char*>(buffer), size*nmemb);

    return size * nmemb;
}

Airports::Airports(QObject *parent) : QObject(parent)
{
}

QPair<double,double>* Airports::get(QString icao)
{
    QMap<QString, QPair<double,double> >::iterator i = airports.find(icao);
    if (i == airports.end()) return NULL;
    return &*i;
}

void Airports::init(QString server)
{
    QDir dir(BASEDIR);
    if (!dir.exists()) dir.mkpath(".");
    QFile dataFile(BASEDIR+"/airports.csv");

    QSettings settings("YAACARS", "YAACARS", this);
    QString current=settings.value("airportDB", "unknown").toString();

    if(server=="") //not inet or update suppressed
    {
        err("airportDB version check: failed! No server version, local="+current);
        if (dataFile.exists()==false)
        {
            wrn("No local airportDB, trying to download latest ...");
            if(update()==false)
            {
                QMessageBox::critical(NULL, "YAACARS", "Could not download airportDB!\n\nNo airport coordinates available!",
                                      QMessageBox::Ok, QMessageBox::NoButton);
                return;
            }
        }
        else
        {
            log("Local airportDB exists - will use this one for now!");
        }
    }
    else
    {
        log("airportDB version check: server="+server+", local="+current);

        if(server!=current || dataFile.exists()==false)
        {
            log("airportDB needs update ...");
            if(update()==false)
            {
                if (dataFile.exists()==false)
                {
                    QMessageBox::critical(NULL, "YAACARS", "Could not download airportDB!\n\nNo airport coordinates available!",
                                          QMessageBox::Ok, QMessageBox::NoButton);
                    return;
                }
                else
                {
                    log("Local airportDB exists - will use this one for now!");
                }
            }
            else
            {
                settings.setValue("airportDB", server);
            }
        }
        else
        {
            log("airportDB is up-to-date!");
        }
    }

    log("Loading airportDB ...");
    if(load()==false)
        QMessageBox::critical(NULL, "YAACARS - Error loading airports",
                              "Could not download airports.csv!",
                              QMessageBox::Ok, QMessageBox::NoButton);
}

bool Airports::load()
{
    QDir dir(BASEDIR);
    if (!dir.exists()) dir.mkpath(".");
    QFile dataFile(BASEDIR+"/airports.csv");

    if (dataFile.open(QFile::ReadOnly))
    {
        airports.clear();

        QTextStream in(&dataFile);
        QString line, icao;
        double lat, lon;
        QStringList parts;
        int count = 0;
        while (in.readLineInto(&line))
        {
            parts = line.trimmed().split(',');
            icao = parts.takeFirst();
            lat = parts.takeFirst().toDouble();
            lon = parts.takeFirst().toDouble();
            airports.insert(icao, QPair<double,double>(lat,lon));
            count++;
        }
        dataFile.close();
        log("Loaded "+QS2I(count)+" airports from DB");
        return true;
    }
    else
    {
        log("Opening airportDB failed!");
        return false;
    }
}

bool Airports::update()
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    QByteArray gCurlData;
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.ftw-sim.de/yaacars/data/airports.php");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &gCurlData);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT_DOWNLOAD_SECS);

    qint32 ret = curl_easy_perform(curl);
    long code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if (ret != CURLE_OK || code >= 400)
    {
        err("Fetching airportDB, returned: "+QS2I(ret));
        return false;
    }
    else
    {
        log("Downloading airportDB successful!");
    }
    curl_easy_cleanup(curl);

    QDir dir(BASEDIR);
    if (!dir.exists()) dir.mkpath(".");
    QFile dataFile(BASEDIR+"/airports.csv");

    if (dataFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        dataFile.write(gCurlData);
        dataFile.close();
    }
    else
    {
        err("Writing airportDB failed!");
        return false;
    }
    log("Updating airportDB successful!");
    return true;
}
