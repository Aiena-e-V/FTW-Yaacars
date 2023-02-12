# //////////////////////////////////////////////////////////////////////////////////////
# 
# // YAACARS is Copyright 2018-2021 Markus Hahlbeck (markus@hahlbeck.com)
# // 
# // This file is part of YAACARS.
# // 
# // YAACARS is free software: you can redistribute it and/or modify
# // it under the terms of the GNU General Public License as published by
# // the Free Software Foundation, either version 3 of the License, or
# // (at your option) any later version.
# // 
# // YAACARS is distributed in the hope that it will be useful,
# // but WITHOUT ANY WARRANTY; without even the implied warranty of
# // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# // GNU General Public License for more details.
# // 
# // You should have received a copy of the GNU General Public License
# // along with YAACARS.  If not, see <http://www.gnu.org/licenses/>.
# 
# //////////////////////////////////////////////////////////////////////////////////////

#-------------------------------------------------
#
# Project created by QtCreator 2017-02-21T11:12:47
#
#-------------------------------------------------

QT       += core gui
QT       += network # for UDP-Socket
QT       += websockets
#QT       += multimediawidgets

#CONFIG += release

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YAACARS
TEMPLATE = app
RC_ICONS = ico/OAAE_Star_Blue_trans.png.ico
ICON = ico/OAAE_Star_Blue_trans.png.icns

## https://stackoverflow.com/questions/6614049/adding-extra-compiler-option-in-qt
QMAKE_CFLAGS_WARN_OFF   = -Wdouble-promotion

#https://stackoverflow.com/questions/43579543/how-to-run-application-as-administrator-in-qt
#QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
#CONFIG += embed_manifest_exe
#QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator'"

SOURCES += \
    src/sim/data.cpp \
	\
    src/ui/yaacars.cpp \
	src/ui/main.cpp\
    src/ui/ipdialog.cpp \
    src/ui/logger.cpp \
    \
	src/flt/flt_cmds.cpp \
    src/flt/flt_evts.cpp \
    src/flt/flt_crits.cpp \
	\
    src/va/ftw/json.cpp \
    src/va/airports.cpp \
    src/va/va.cpp \
    src/sim/comm_xpudp.cpp \
    src/sim/comm_fsuipc.cpp \
    src/va/update.cpp \
    src/ui/infobar.cpp \
    src/va/ftw/xacars.cpp \
    src/ui/mapserver.cpp

HEADERS  += \
    src/sim/data.h \
	\
    src/ui/yaacars.h \
    src/ui/ipdialog.h \
    src/ui/logger.h \
	\
    src/va/airports.h \
    src/va/update.h \
    src/ui/infobar.h \
    src/ui/mapserver.h

FORMS    += \
	src/ui/mainwindow.ui \
    src/ui/ipdialog.ui \
    src/ui/infobar.ui

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    doc/todo.txt \
    doc/YAACARS.graphml

PLATFORM = $$QMAKESPEC
PLATFORM ~= s|.+/|
PLATFORM ~= s|.+\\|

LIBS += \
		-L$$PWD/$$PLATFORM \
		-L$$PWD \
		-L$$PWD/src/lib/curl \
		-lcurl

win32:LIBS += \
		-L$$PWD/src/lib/uipc \
		-luipc
		
win32:LIBS += \
		-lws2_32 \
		-lz

ANDROID_EXTRA_LIBS = $$PWD/android-g++/libcurl.so
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
