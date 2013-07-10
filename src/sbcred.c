/**
 * sbcred.c
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Pim Vullers, Radboud University Nijmegen, July 2013.
 */

#include "sbcred.h"

// Name everything "sbcred"
#pragma attribute("aid", "73 62 63 72 65 64")
#pragma attribute("dir", "61 10 4f 6 69 64 65 6D 69 78 50 6 69 64 65 6D 69 78")

#include <ISO7816.h> // for APDU constants
#include <multosarith.h> // for COPYN()
#include <multosccr.h> // for ZFlag()
#include <string.h> // for memset()

#include "APDU.h"
#include "MULTOS.h"
#include "debug.h"
#include "ECC.h"

/********************************************************************/
/* Public segment (APDU buffer) variable declaration                */
/********************************************************************/
#pragma melpublic

unsigned char APDU_buffer[255];


/********************************************************************/
/* Session segment (application RAM memory) variable declaration    */
/********************************************************************/
#pragma melsession



/********************************************************************/
/* Static segment (application EEPROM memory) variable declarations */
/********************************************************************/
#pragma melstatic

unsigned char initialised = 0x00;
ECC_domain_params domainParams;
ECC_key_pair keyPair;

/********************************************************************/
/* APDU handling                                                    */
/********************************************************************/

void main(void) {
  switch (INS) {
    case 0x01:
      initialise(APDU_buffer);
      break;

    case 0x02:
      personalise();
      break;

    case 0x03:
      getAttribute();
      break;

    case 0x04:
      getKey();
      break;

    default:
      debugWarning("Unknown instruction");
      APDU_ReturnSW(ISO7816_SW_INS_NOT_SUPPORTED);
    }
}

void initialise(unsigned char *buffer) {
  unsigned int offset = 0;

  if (initialised) {
    debugWarning("Already initialised");
    APDU_ReturnSW(ISO7816_SW_COMMAND_NOT_ALLOWED_AGAIN);
  }

  domainParams.format = 0x00; // format of domain params
  domainParams.bytes = buffer[offset + 1];
  offset += 2;

  // P
  memcpy(domainParams.p, buffer + offset, domainParams.bytes);
  offset += domainParams.bytes;
  
  // R
  offset += 2;
  memcpy(domainParams.r, buffer + offset, domainParams.bytes);
  offset += domainParams.bytes;

  // A
  offset += 2;
  memcpy(domainParams.a, buffer + offset, domainParams.bytes);
  offset += domainParams.bytes;

  // B
  offset += 2;
  memcpy(domainParams.b, buffer + offset, domainParams.bytes);
  offset += domainParams.bytes;

  // G
  offset += 2;
  memcpy(domainParams.G.x, buffer + offset, domainParams.bytes);
  offset += domainParams.bytes;
  memcpy(domainParams.G.y, buffer + offset, domainParams.bytes);

  // Generate keys
  ECC_generate_keys(domainParams, keyPair);
}

void personalise(void) {
}

void getAttribute(void) {
}

void getKey(void) {
}
