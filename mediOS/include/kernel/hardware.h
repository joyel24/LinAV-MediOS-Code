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
#ifdef GMINI4XX
#include <kernel/target/gmini4xx_def.h>
#else
#error bad target device
#endif /* GMINI4XX */
#endif /* AV3XX */

#endif  /* __HARDWARE_H */
