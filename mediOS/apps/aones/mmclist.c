/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** mmclist.c
**
** list of all mapper interfaces
** $Id$
*/

/*************************************
  Little John GP32
  File : mmclist.c  
  Version : 0.2
  Last update : 7th July 2003
**************************************/

#include "datatypes.h"
#include "unes_mapper.h"

/* mapper interfaces */
extern mapintf_t map0_intf;
extern mapintf_t map1_intf;
extern mapintf_t map2_intf;
extern mapintf_t map3_intf;
extern mapintf_t map4_intf;
extern mapintf_t map5_intf;
extern mapintf_t map6_intf;
extern mapintf_t map7_intf;
extern mapintf_t map8_intf;
extern mapintf_t map9_intf;
extern mapintf_t map10_intf;
extern mapintf_t map11_intf;
extern mapintf_t map13_intf;
extern mapintf_t map15_intf;
extern mapintf_t map16_intf;
extern mapintf_t map17_intf;
extern mapintf_t map18_intf;
extern mapintf_t map19_intf;
extern mapintf_t map21_intf;
extern mapintf_t map22_intf;
extern mapintf_t map23_intf;
extern mapintf_t map24_intf;
extern mapintf_t map25_intf;
extern mapintf_t map26_intf;
extern mapintf_t map32_intf;
extern mapintf_t map33_intf;
extern mapintf_t map34_intf;
extern mapintf_t map40_intf;
extern mapintf_t map41_intf;
extern mapintf_t map42_intf;
extern mapintf_t map43_intf;
extern mapintf_t map44_intf;
extern mapintf_t map45_intf;
extern mapintf_t map64_intf;
extern mapintf_t map66_intf;
extern mapintf_t map68_intf;
extern mapintf_t map69_intf;
extern mapintf_t map70_intf;
extern mapintf_t map71_intf;
extern mapintf_t map72_intf;
extern mapintf_t map73_intf;
extern mapintf_t map74_intf;
extern mapintf_t map75_intf;
extern mapintf_t map76_intf;
extern mapintf_t map77_intf;
extern mapintf_t map78_intf;
extern mapintf_t map79_intf;
extern mapintf_t map80_intf;
extern mapintf_t map82_intf;
extern mapintf_t map83_intf;
extern mapintf_t map85_intf;
extern mapintf_t map86_intf;
extern mapintf_t map87_intf;
extern mapintf_t map88_intf;
extern mapintf_t map89_intf;
extern mapintf_t map90_intf;
extern mapintf_t map91_intf;
extern mapintf_t map92_intf;
extern mapintf_t map93_intf;
extern mapintf_t map94_intf;
extern mapintf_t map95_intf;
extern mapintf_t map96_intf;
extern mapintf_t map97_intf;
extern mapintf_t map99_intf;
extern mapintf_t map100_intf;
extern mapintf_t map101_intf;
extern mapintf_t map105_intf;
extern mapintf_t map112_intf;
extern mapintf_t map113_intf;
extern mapintf_t map114_intf;
extern mapintf_t map115_intf;
extern mapintf_t map117_intf;
extern mapintf_t map118_intf;
extern mapintf_t map119_intf;
extern mapintf_t map122_intf;
extern mapintf_t map187_intf;
extern mapintf_t map255_intf;

/* implemented mapper interfaces */
mapintf_t *mappers[] =
{
   &map0_intf,
   &map1_intf,
   &map2_intf,
   &map3_intf,
   &map4_intf,
   &map5_intf,
   &map6_intf,
   &map7_intf,
   &map8_intf,
   &map9_intf,
   &map10_intf,
   &map11_intf,
   &map13_intf,
   &map15_intf,
   &map16_intf,
   &map17_intf,
   &map18_intf,
   &map19_intf,
   &map21_intf,
   &map22_intf,
   &map23_intf,
   &map24_intf,
   &map25_intf,
   &map26_intf,
   &map32_intf,
   &map33_intf,      
   &map34_intf,
   &map40_intf,
   &map41_intf,
   &map42_intf,
   &map43_intf,
   &map44_intf,
   &map45_intf,
   &map64_intf,
   &map66_intf,
   &map68_intf,
   &map69_intf,
   &map70_intf,
   &map71_intf,
   &map72_intf,
   &map73_intf,
   &map74_intf,
   &map75_intf,
   &map76_intf,
   &map77_intf,
   &map78_intf,
   &map79_intf,
   &map80_intf,
   &map82_intf,
   &map83_intf,
   &map85_intf,
   &map86_intf,
   &map87_intf,
   &map88_intf,
   &map89_intf,
   &map90_intf,
   &map91_intf,
   &map92_intf,
   &map93_intf,
   &map94_intf,
   &map95_intf,
   &map96_intf,
   &map97_intf,
   &map99_intf,
   &map100_intf,
   &map101_intf,
   &map105_intf,
   &map112_intf,
   &map113_intf,
   &map114_intf,
   &map115_intf,
   &map117_intf,
   &map118_intf,
   &map119_intf,
   &map122_intf,
   &map187_intf,
   &map255_intf,   
   NULL
};

