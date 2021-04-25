#!/bin/bash

ELF_h="^[0-9][0-9]*: Elf [0-9][0-9]*: taking holidays$"
ELF_hv=0
ELF_s="^[0-9][0-9]*: Elf [0-9][0-9]*: started$"
ELF_sv=0
ELF_n="^[0-9][0-9]*: Elf [0-9][0-9]*: need help$"
ELF_nv=0
ELF_g="^[0-9][0-9]*: Elf [0-9][0-9]*: get help$"
ELF_gv=0

RD_s="^[0-9][0-9]*: RD [0-9][0-9]*: rstarted$"
RD_sv=0
RD_r="^[0-9][0-9]*: RD [0-9][0-9]*: return home$"
RD_rv=0
RD_h="^[0-9][0-9]*: RD [0-9][0-9]*: get hitched$"
RD_hv=0

S_s="^[0-9][0-9]*: Santa: going to sleep$"
S_sv=0
S_h="^[0-9][0-9]*: Santa: helping elves$"
S_hv=0
S_c="^[0-9][0-9]*: Santa: closing workshop$"
S_cv=0
S_ch="^[0-9][0-9]*: Santa: Christmas started$"
S_chv=0

while read line
do
  if echo $line | grep "$ELF_h" >/dev/null; then
  	ELF_hv=1
  elif echo $line | grep "$ELF_s" >/dev/null; then
  	ELF_sv=1
  elif echo $line | grep "$ELF_n" >/dev/null; then
  	ELF_nv=1
  elif echo $line | grep "$ELF_g" >/dev/null; then
  	ELF_gv=1
  elif echo $line | grep "$RD_s" >/dev/null; then
  	RD_sv=1
  elif echo $line | grep "$RD_r" >/dev/null; then
  	RD_rv=1
  elif echo $line | grep "$RD_h" >/dev/null; then
  	RD_hv=1
  elif echo $line | grep "$S_s" >/dev/null; then
  	S_sv=1
  elif echo $line | grep "$S_h" >/dev/null; then
  	S_hv=1
  elif echo $line | grep "$S_c" >/dev/null; then
  	S_cv=1
  elif echo $line | grep "$S_ch" >/dev/null; then
  	S_chv=1
  else
  	echo "Line format error:" $line
  fi
done

if [ ! X$ELF_hv = "X1" ]; then 
	echo "WARNING: no elf taking holidays"
fi
if [ ! X$ELF_sv = "X1" ]; then 
	echo "WARNING: no elf started"
fi
if [ ! X$ELF_nv = "X1" ]; then 
	echo "WARNING: no elf need help"
fi
if [ ! X$ELF_gv = "X1" ]; then 
	echo "WARNING: no elf get help"
fi
if [ ! X$RD_sv = "X1" ]; then 
	echo "WARNING: no RD started"
fi
if [ ! X$RD_rv = "X1" ]; then 
	echo "WARNING: no RD returned home"
fi
if [ ! X$RD_hv = "X1" ]; then 
	echo "WARNING: no RD hitched"
fi
if [ ! X$S_sv = "X1" ]; then 
	echo "WARNING: no SANTA going to sleep"
fi
if [ ! X$S_cv = "X1" ]; then 
	echo "WARNING: no SANTA closing workshop"
fi
if [ ! X$S_hv = "X1" ]; then 
	echo "WARNING: no SANTA helping elves"
fi
if [ ! X$S_chv = "X1" ]; then 
	echo "WARNING: no Christmas started"
fi

