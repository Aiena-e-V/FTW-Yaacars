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

//Based on the QtWebSockets Example - see Header File for full license
/****************************************************************************
**
** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebSockets module of the Qt Toolkit.
**
****************************************************************************/

#include "mapserver.h"
#include "yaacars.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"

QT_USE_NAMESPACE

MapServer::MapServer(quint16 port, bool debug, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Map Server"),
                                            QWebSocketServer::NonSecureMode, this)),
    m_debug(debug)
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        //if (m_debug)
        //    qDebug() << "mapserver listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &MapServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &MapServer::closed);
    }

    memset(&current,0,sizeof(T_Status));
}

MapServer::~MapServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void MapServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &MapServer::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &MapServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &MapServer::socketDisconnected);

    m_clients << pSocket;
}

void MapServer::clearWaypoints()
{
    waypoints.clear();
}

void MapServer::addWaypoint(double lat, double lon)
{
    waypoints.insert(waypoints.count(),QPair<double,double>(lat,lon));
}

void MapServer::setCurrent(T_Status* flightDataOrig)
{
    memcpy(&current, flightDataOrig, sizeof(T_Status));
}

void MapServer::setFlights(QString str)
{
    flights=str.replace("&quot;", "\"");
}

void MapServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    //if (m_debug)
    //    qDebug() << "Message received:" << message;
    if (pClient)
    {
        QJsonObject jObj;

        if(message=="live_flights")
        {
            pClient->sendTextMessage(flights);
            return; //STOP HERE - WE DON'T want the "recode stuff" below!
        }
        else if(message=="track_init")
        {
            jObj["type"] = message;

            jObj["start"] = "1";
            jObj["count"] = QS2I(waypoints.count());

            QPair<double,double> coords;
            foreach(int i, waypoints.keys())
            {
                coords=(QPair<double,double>) waypoints[i];
                jObj[QS2I(i+1)] = QS2F(coords.first,6)+" "+QS2F(coords.second,6);
            }
        }
        else if(message=="track_update")
        {
            jObj["type"] = message;

            jObj["count"] = QS2I(waypoints.count());

            if(waypoints.count()>0)
            {
                QPair<double,double> coords=(QPair<double,double>) waypoints[waypoints.count()-1];
                jObj[QS2I(waypoints.count())] = QS2F(coords.first,6)+" "+QS2F(coords.second,6);
            }

            jObj["pilot"]   = username;

            jObj["lat"]     = QS2F(current.coordsCURR.first,6);
            jObj["lon"]     = QS2F(current.coordsCURR.second,6);
            jObj["hdg_deg"] = QS2I(current.HDG);
            jObj[ "gs_kts"] = QS2I(current.GS_kts);
            jObj["asl_ft" ] = QS2I(current.ASL_ft);
            jObj["agl_ft" ] = QS2I(current.AGL_ft);

            jObj["lat_start"]= QS2F(current.coordsLOADED.first,6);
            jObj["lon_start"]= QS2F(current.coordsLOADED.second,6);
            jObj["flt_state"]= QS2I(current.acarsState);
        }
        else
            return;

        QJsonDocument jDoc(jObj);
        QByteArray payload = jDoc.toJson();
        pClient->sendTextMessage(payload.data());
    }
}

void MapServer::processBinaryMessage(QByteArray message)
{
//    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
//    if (m_debug)
//        qDebug() << "Binary Message received:" << message;
//    if (pClient) {
//        pClient->sendBinaryMessage(message);
//    }
}

void MapServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    //if (m_debug)
    //    qDebug() << "socketDisconnected:" << pClient;
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
