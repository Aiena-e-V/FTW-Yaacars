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

#include "yaacars.h"
#include <QApplication>
#include <qendian.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    MapServer *mapserver = new MapServer(1111, false);
    QObject::connect(mapserver, &MapServer::closed, &a, &QCoreApplication::quit);

    w.show();
    return a.exec();
}

void memcat(char** ptr, const char* data, int len) {
    memcpy(*ptr, data, len);
    (*ptr) += len;
}
void memcat(char** ptr, const char* data) {
    memcpy(*ptr, data, strlen(data)+1);
    (*ptr) += strlen(data)+1;
}
void memcat(char** ptr, const int data) {
    memcpy(*ptr, &data, sizeof(data));
    (*ptr) += sizeof(data);
}

void sendDRef(QUdpSocket* sock, const char* name, qint32 id, qint32 freq) {
    QHostAddress addr("127.0.0.1");
    char buffer[1024];

    memset(buffer, 0, 1024);
    char* pos = buffer;
    memcat(&pos, "RREF", 5);
    memcat(&pos, freq);
    memcat(&pos, id);
    memcat(&pos, name);
    sock->writeDatagram(buffer, 413, addr, 49000);
}


void setDRef(QUdpSocket* sock, const char* name, float val)
{
    QHostAddress addr("127.0.0.1");
    char buffer[1024];

    memset(buffer, 0, 1024);
    char* pos = buffer;

    memcat(&pos, "DREF", 5);

    //memcat(&pos, val);
    qToLittleEndian(val, pos);
    pos+=4;

    memcat(&pos, name);

    sock->writeDatagram(buffer, 509, addr, 49000);
}

double greatcircle(QPair<double,double> src, QPair<double,double> tgt) {
    return greatcircle(src.first, src.second, tgt.first, tgt.second);
}
double greatcircle(double lat1, double lon1, QPair<double,double> tgt) {
    return greatcircle(lat1, lon1, tgt.first, tgt.second);
}
double greatcircle(QPair<double,double> src, double lat2, double lon2) {
    return greatcircle(src.first, src.second, lat2, lon2);
}
double greatcircle(double lat1, double lon1, double lat2, double lon2) {
    lat1 *= (M_PI/180.0);
    lon1 *= (M_PI/180.0);
    lat2 *= (M_PI/180.0);
    lon2 *= (M_PI/180.0);

    double deltalat = lat2 - lat1;
    double deltalon = lon2 - lon1;

    double a = sin(deltalat/2) * sin(deltalat/2) +
                cos(lat1) * cos(lat2) *
                sin(deltalon/2) * sin(deltalon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return 3443.9185 * c; // Earth radius in nmi
}

QString latToXACARS(double coord)
{
    QString latlon;
    if(coord>=0) latlon="N"; else latlon="S";
    coord=fabs(coord);
    latlon+=QS2F(floor(coord),0);//.rightJustified(2, '0');
    latlon+=" ";
    latlon+=QS2F((coord-floor(coord))*60, 4);
    return latlon;
}
QString lonToXACARS(double coord)
{
    QString latlon;
    if(coord>=0) latlon="E"; else latlon="W";
    coord=fabs(coord);
    latlon+=QS2F(floor(coord),0);//.rightJustified(2, '0');
    latlon+=" ";
    latlon+=QS2F((coord-floor(coord))*60, 4);
    return latlon;
}
