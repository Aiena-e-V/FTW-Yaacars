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

//Based on the example from https://wiki.qt.io/Simple-logger

#include "src/ui/logger.h"

Logger::Logger(QObject *parent, QString fileName, QPlainTextEdit *editor, QStatusBar *statusbar) : QObject(parent)
{
    m_editor = editor;
    m_sbar = statusbar;
    m_showDate = true;
    dbg=log=wrn=err=true;

    if (!fileName.isEmpty())
    {
        file = new QFile;
        file->setFileName(fileName);
        file->open(QIODevice::Append | QIODevice::Text);
    }
}

void Logger::write(const QString &prefix, const QString &value)
{
    QString text = "["+prefix+"] "+value;// + "";
    if (m_showDate)
        text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + text;

    QTextStream out(file);
    out.setCodec("UTF-8");
    if (file != 0)
        out << text+"\n";
}

void Logger::show(const QString &prefix, const QString &value)
{
    QString text = "["+prefix+"] "+value;// + "";
    if (m_showDate)
        text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + text;

    if (m_editor != 0)
        m_editor->appendPlainText(text);
    if (m_sbar != 0)
        m_sbar->showMessage(text);
}

void Logger::write_dbg(const QString &prefix, const QString &value)
{
    if(dbg)
    {
        show("dbg|"+prefix,value);
        //write("dbg|"+prefix,value);
    }
}
void Logger::write_log(const QString &prefix, const QString &value)
{
    if(log)
    {
        show("Log|"+prefix,value);
        write("Log|"+prefix,value);
    }
}
void Logger::write_wrn(const QString &prefix, const QString &value)
{
    if(wrn)
    {
        show("Wrn|"+prefix,value);
        write("Wrn|"+prefix,value);
    }
}
void Logger::write_err(const QString &prefix, const QString &value)
{
    if(err)
    {
        show("ERR|"+prefix,value);
        write("ERR|"+prefix,value);
    }
}

void Logger::setShowDateTime(bool value)
{
    m_showDate = value;
}

Logger::~Logger()
{
    if (file != 0)
        file->close();
}
