#!/bin/bash

FW="ATtiny3217-1Finger-Low_Power-4x-Project-MPT2"

[ ! -d fw ] && mkdir fw
[ ! -d fw/out ] && mkdir fw/out

if [ -d fw/out ] ; then
	rm -r fw/out/*

	cp "Debug/${FW}.hex" fw/out/fw.hex
	cp "Debug/${FW}.ihex"	fw/out/fw.ihex
	cp "Debug/${FW}.save"	fw/out/fw.save
	cp "Debug/${FW}.dump"	fw/out/fw.dump
	cp "Debug/${FW}.map" fw/out/fw.map
	cp qtouch/touch.h fw/out/
	cp qtouch/pack.h fw/out/

	cd fw/out/
	tar cvf ../fw.tar *
else
	echo "fw/out is not exit"
fi

