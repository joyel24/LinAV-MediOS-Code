@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ AV300 Specifics
@
@ Date:     20/01/2004
@ Author:   By DoggerMoore
@

hardwareAV300               =   1

ramStart                    =   0x03000000

lcdRegBacklight             =   0x02600200
lcdWidth                    =   320
lcdHeight                   =   240

ataRegData                  =   0x02400000
ataRegError                 =   0x02400080
ataRegNSector               =   0x02400100
ataRegSector                =   0x02400180
ataRegLCyl                  =   0x02400200
ataRegHCyl                  =   0x02400280
ataRegSelect                =   0x02400300
ataRegStatus                =   0x02400340
ataRegCommand               =   0x02400380

