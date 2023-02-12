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

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class Logger : public QObject
{
 Q_OBJECT
public:
 explicit Logger(QObject *parent, QString fileName, QPlainTextEdit *editor = 0, QStatusBar *statusbar = 0);
 ~Logger();
 void setShowDateTime(bool value);

 bool dbg;
 bool log;
 bool wrn;
 bool err;

private:
 QFile *file;
 QPlainTextEdit *m_editor;
 QStatusBar *m_sbar;
 bool m_showDate;

 void write(const QString &prefix, const QString &value);
 void show(const QString &prefix, const QString &value);
signals:

public slots:
 void write_dbg(const QString &prefix, const QString &value);
 void write_log(const QString &prefix, const QString &value);
 void write_wrn(const QString &prefix, const QString &value);
 void write_err(const QString &prefix, const QString &value);

};

#endif // LOGGER_H
