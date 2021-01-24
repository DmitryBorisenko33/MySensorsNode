/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "pinMap.h"

//nrf-arduino
const uint32_t g_ADigitalPinMap[] = {
    //digital pins=======================
    11,  //D0
    12,  //D1
    13,  //D2
    14,  //D3
    15,  //D4
    16,  //D5
    17,  //D6
    18,  //D7
    19,  //D8
    20,  //D9
    22,  //D10
    23,  //D11
    24,  //D12
    25,  //D13

    //analog pins=======================
    3,   //A0
    4,   //A1
    28,  //A2
    29,  //A3
    30,  //A4
    31,  //A5
    5,   //A6 AIN3 (P0.05)
    2,   //A7 AIN0 (P0.02) / AREF

    //i2c pins=======================
    26,  // SDA
    27,  // SCL

    //uart pins=======================
    8,  // RX
    6   // TX
};
