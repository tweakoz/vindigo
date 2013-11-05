// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7402.h
 *
 *  DM7402: Quad 2-Input NOR Gates
 *
 *          +--------------+
 *       Y1 |1     ++    14| VCC
 *       A1 |2           13| Y4
 *       B1 |3           12| B4
 *       Y2 |4    7400   11| A4
 *       A2 |5           10| Y3
 *       B2 |6            9| B3
 *      GND |7            8| A3
 *          +--------------+
 *                  ___
 *              Y = A+B
 *          +---+---++---+
 *          | A | B || Y |
 *          +===+===++===+
 *          | 0 | 0 || 1 |
 *          | 0 | 1 || 0 |
 *          | 1 | 0 || 0 |
 *          | 1 | 1 || 0 |
 *          +---+---++---+
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */


#include "nld_signal.h"

#ifndef NLD_7402_H_
#define NLD_7402_H_

#define TTL_7402_NOR(_name, _I1, _I2)                                               \
        NET_REGISTER_DEV(7402, _name)                                               \
        NET_CONNECT(_name, A, _I1)                                                  \
        NET_CONNECT(_name, B, _I2)

NETLIB_SIGNAL(7402, 2, 1, 0);

#endif /* NLD_7402_H_ */
