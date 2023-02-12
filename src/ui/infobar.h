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

#ifndef INFOBAR_H
#define INFOBAR_H

#include <QDialog>

namespace Ui {
class InfoBar;
}

class InfoBar : public QDialog
{
    Q_OBJECT

public:
    explicit InfoBar(QWidget *parent = nullptr);
    ~InfoBar();

    void setStateTxt(QString);
    void setStateCol(QString);

    void setDistNmTxt(QString);
    void setDistNmCol(QString);
    void setDistPercTxt(QString);
    void setDistPercCol(QString);

    void setFuelOBTxt(QString);
    void setFuelOBCol(QString);
    void setFuelUTxt(QString);
    void setFuelUCol(QString);

private slots:

    void showMainWin();

    //void on_labelDistNm_linkActivated(const QString &link);
	//
    //void on_labelDistPerc_linkActivated(const QString &link);
	//
    //void on_labelFuelOB_linkActivated(const QString &link);
	//
    //void on_labelFuelU_linkActivated(const QString &link);

private:
    Ui::InfoBar *ui;
};

#endif // INFOBAR_H
