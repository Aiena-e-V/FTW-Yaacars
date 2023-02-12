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

#include "update.h"
#include "src/lib/curl/curl.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#define CURL_TIMEOUT_GENERAL_SECS (15L)

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    QByteArray* gCurlData = static_cast<QByteArray*>(userp);

    gCurlData->append(static_cast<const char*>(buffer), size*nmemb);

    return size * nmemb;
}

Update::Update(QObject *parent) : QObject(parent)
{
    airport=release=testing="";
}

void Update::fetchVersions()
{
    airport=release=testing="";

    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    QByteArray gCurlData;
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.ftw-sim.de/yaacars/data/versions.php");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &gCurlData);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT_GENERAL_SECS);

    //qint32 ret =
    curl_easy_perform(curl);
    //long code;
    //curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    //if (ret != CURLE_OK || code >= 400)
    //{
    //    QMessageBox::warning(NULL, "YAACARS",
    //                          "Error loading Version Info\nUnable to check for updates!",
    //                          QMessageBox::Ok, QMessageBox::NoButton);
    //}
    //else
    {
        QJsonParseError parseErr;
        QJsonDocument jDoc = QJsonDocument::fromJson(gCurlData, &parseErr);
        QJsonObject jServer = jDoc.object();

        if(parseErr.error==parseErr.NoError)
        {
            if(jServer.find("airport")!=jServer.end())
                airport=jServer["airport"].toString();
            if(jServer.find("release")!=jServer.end())
                release=jServer["release"].toString();
            if(jServer.find("testing")!=jServer.end())
                testing=jServer["testing"].toString();
        }

        qInfo(qUtf8Printable("Fetching version info: [release="+release+"]/[testing="+testing+"]"));
        qInfo(qUtf8Printable("Fetching airport info: [airport="+airport+"]"));
    }
    curl_easy_cleanup(curl);
}
