#!/bin/bash

export REL=V1.1
export APP=YAACARS
export DRP=/home/mk/Dropbox/$APP
export PATH=$PATH:/home/mk/Qt/5.11.2/gcc_64/bin

export SUF=AppImage
export OUT=$DRP/deploy
export WORK=/home/mk/Desktop/$APP

export SRC=$WORK/source
export PRD=$WORK/prod
export DPL=$WORK/deploy

rm -f "$OUT/$APP*.$SUF"
rm -rf "$WORK"

mkdir -p $SRC
mkdir -p $PRD
mkdir -p $DPL
mkdir -p $OUT

cd $SRC
	# --- CHECKOUT ---
	fossil clone [removed]
	fossil open yaacars.fossil
	fossil update ftw
	
	# --- REVGEN ---
	BRA=`fossil status | sed -n "s/tags: [ \t]*//p"`
	BRA=`echo "$BRA" | tr a-z A-Z`

	REV=`fossil info | sed -n "s/check-ins: [ \t]*//p"`
	echo "" > revision.h
	echo "#define REV_CHECKINS $REV">>revision.h
	echo "#define REV_BRANCH \"$BRA\"">>revision.h
cd ..

cd $PRD
	# --- MAKE ---
	qmake $SRC/$APP.pro
	make
	
	# --- DEPLOY ---
	chmod a+x $SRC/tools/linuxdeployqt
	$SRC/tools/linuxdeployqt $PRD/YAACARS -appimage

	cp $PRD/Application-x86_64.$SUF $DPL/$APP.$SUF
	cp -v $DPL/$APP.$SUF "$OUT/$APP $REL (Build $BRA#$REV).$SUF"
cd ..
