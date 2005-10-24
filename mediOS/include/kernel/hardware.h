/*
 * include/hardware.h
 *
 * AMOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __HARDWARE_H
#define __HARDWARE_H

#ifdef AV3XX
#include <kernel/target/av3xx_def.h>
#else
#error bad target device
#endif

#endif  /* __HARDWARE_H */
