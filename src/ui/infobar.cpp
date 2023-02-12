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

#include "infobar.h"
#include "ui_infobar.h"

InfoBar::InfoBar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoBar)
{
    ui->setupUi(this);
}

InfoBar::~InfoBar()
{
    delete ui;
}

void InfoBar::setStateTxt(QString txt)
{
    ui->labelState->setText("  "+txt);
}
void InfoBar::setStateCol(QString txt)
{
    ui->labelState->setStyleSheet(txt);
}

/////////////////////////////////////////

void InfoBar::setDistNmTxt(QString txt)
{
    ui->labelDistNm->setText(txt+"  ");
}
void InfoBar::setDistNmCol(QString txt)
{
    ui->labelDistNm->setStyleSheet(txt);
}

void InfoBar::setDistPercTxt(QString txt)
{
    ui->labelDistPerc->setText(txt+"%  ");
}
void InfoBar::setDistPercCol(QString txt)
{
    ui->labelDistPerc->setStyleSheet(txt);
}

/////////////////////////////////////////

void InfoBar::setFuelOBTxt(QString txt)
{
    ui->labelFuelOB->setText("  FOB: "+txt);
}
void InfoBar::setFuelOBCol(QString txt)
{
    ui->labelFuelOB->setStyleSheet(txt);
}

void InfoBar::setFuelUTxt(QString txt)
{
    ui->labelFuelU->setText("FU: "+txt);
}
void InfoBar::setFuelUCol(QString txt)
{
    ui->labelFuelU->setStyleSheet(txt);
}

/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////

void InfoBar::showMainWin()
{
    //parent->invokeMethod("show()");
}

/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////
