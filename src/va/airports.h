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

#ifndef AIRPORTS_H
#define AIRPORTS_H

#include <QObject>
#include <QMap>
#include <QPair>
#include "src/ui/logger.h"

//--- MACROs
#define err(txt) logger->write_err(QString(LOGGER_SECTION),txt)
#define dbg(txt) logger->write_dbg(QString(LOGGER_SECTION),txt)
#define log(txt) logger->write_log(QString(LOGGER_SECTION),txt)
#define wrn(txt) logger->write_wrn(QString(LOGGER_SECTION),txt)

#define QS2F(a,b) QString::number(a, 'f', b)
#define QS2I(a) QString::number(a, 'f', 0)

class Airports : public QObject
{
    Q_OBJECT
public:
    Logger *logger;

    explicit Airports(QObject *parent = 0);
    QPair<double, double> *get(QString icao);
    void init(QString server);
    bool load();
    bool update();

signals:

public slots:

private:
    QMap<QString, QPair<double,double> > airports;
};

#endif // AIRPORTS_H
