/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
 
/**
 * \file
 *        Example of an interfering node
 * \author
 *        Sergio Diaz
 */
 
#include "contiki.h"
 
#include "dev/cc2420.h" // Include the CC2420 library
#include "dev/cc2420_const.h" // Include the CC2420 constants 
#include "dev/spi.h" // Include basic SPI macros
 
/*---------------------------------------------------------------------------*/
PROCESS(unmodulated_carrier, "CC2420 Unmodulated Carrier"); // Declares the process unmodulated_carrier
AUTOSTART_PROCESSES(&unmodulated_carrier); // Load the process on boot

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(unmodulated_carrier, ev, data) // Defines the process unmodulated_carrier
{
 
  PROCESS_BEGIN();  // Says where the process starts

  cc2420_set_channel(26);
  cc2420_set_txpower(CC2420_TXPOWER_MAX);
 
  // Creates an unmodulated carrier
  CC2420_WRITE_REG(CC2420_MANOR, 0x0100);
  CC2420_WRITE_REG(CC2420_TOPTST, 0x0004);
  CC2420_WRITE_REG(CC2420_MDMCTRL1, 0x0508);
  CC2420_WRITE_REG(CC2420_DACTST, 0x1800);
  CC2420_STROBE(CC2420_STXON); 
 
  PROCESS_END();  //Says where the process ends
}
/*---------------------------------------------------------------------------*/