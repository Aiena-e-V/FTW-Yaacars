#!/bin/bash

export REL=V1.1
export APP=YAACARS
export DRP=/Users/markus/Dropbox/$APP
export PATH=$PATH:$DRP/mac/bin:/Users/markus/Qt/5.11.2/clang_64/bin

export SUF=dmg
export OUT=$DRP/deploy
export WORK=/Users/markus/Desktop/$APP

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

	macdeployqt $APP.app -dmg

	cp $PRD/$APP.$SUF $DPL
	cp -v $DPL/$APP.$SUF "$OUT/$APP $REL (Build $BRA#$REV).$SUF"
cd ..
