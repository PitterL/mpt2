#!/bin/sh

rm -r fw/out/
mkdir fw/out/
cp Debug/ATtiny3217-1Finger-Low_Power-Project-MPT2.hex fw/out/fw.hex
cp Debug/ATtiny3217-1Finger-Low_Power-Project-MPT2.ihex	fw/out/fw.ihex
cp Debug/ATtiny3217-1Finger-Low_Power-Project-MPT2.save	fw/out/fw.save
cp Debug/ATtiny3217-1Finger-Low_Power-Project-MPT2.dump	fw/out/fw.dump
cp Debug/ATtiny3217-1Finger-Low_Power-Project-MPT2.map fw/out/fw.map
cp qtouch/touch.h fw/out/
cp qtouch/pack.h fw/out/

cd fw/out/
tar cvf ../fw.tar *