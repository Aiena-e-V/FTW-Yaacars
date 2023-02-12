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
#include "ipdialog.h"
#include "ui_ipdialog.h"

IPDialog::IPDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::IPDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText(parent->myip.toString());
    ui->lineEdit_2->setText(parent->remoteip.toString());
}

IPDialog::~IPDialog()
{
    delete ui;
}

void IPDialog::on_buttonBox_accepted()
{
    MainWindow* p = (MainWindow*)parent();

    p->myip = ui->lineEdit->text();
    p->remoteip = ui->lineEdit_2->text();

    QSettings settings("OAAE", "YAACARS", this);
    settings.setValue("myip", p->myip.toString());
    settings.setValue("remoteip", p->remoteip.toString());

    p->connectToSim();
    this->hide();
    this->destroy();
}

void IPDialog::on_buttonBox_rejected()
{
    this->hide();
    this->destroy();
}
