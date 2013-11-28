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
#pragma attribute("dir", "61 10 4f 6 73 62 63 72 65 64 50 6 73 62 63 72 65 64")

#include <ISO7816.h> // for APDU constants
#include <multosarith.h> // for COPYN()
#include <multosccr.h> // for ZFlag()
#include <string.h> // for memset()

#include "APDU.h"
#include "MULTOS.h"
#include "debug.h"
#include "ECC.h"

#define ATTRIBUTE_COUNT 4

/********************************************************************/
/* Public segment (APDU buffer) variable declaration                */
/********************************************************************/
#pragma melpublic

unsigned char APDU_buffer[255];


/********************************************************************/
/* Session segment (application RAM memory) variable declaration    */
/********************************************************************/
#pragma melsession

ECC_domain_params blindParams;
ECC_key_pair blindPair;
ECC_point P;
ECC_point x;


/********************************************************************/
/* Static segment (application EEPROM memory) variable declarations */
/********************************************************************/
#pragma melstatic

unsigned char initialised = 0x00;
ECC_domain_params domainParams;
ECC_key_pair keyPair;
SBC_attribute attribute[ATTRIBUTE_COUNT];

/********************************************************************/
/* APDU handling                                                    */
/********************************************************************/

void main(void) {
  unsigned int length = 0;

  switch (INS) {
    case 0x02:
      personalise(APDU_buffer);
      APDU_Return();

    case 0x03:
      length = getAttribute(APDU_buffer);
      APDU_ReturnLa(length);

    case 0x01:
      // Initialise the cards parameters and keys
      initialise(APDU_buffer);

    case 0x04:
      // Return the cards public key
      length = getKey(APDU_buffer);
      APDU_ReturnLa(length);

    case 0x05:
      // Compute the Diffie-Hellman key agreement
      length = computeDH(APDU_buffer);
      APDU_ReturnLa(length);

    default:
      debugWarning("Unknown instruction");
      APDU_ReturnSW(ISO7816_SW_INS_NOT_SUPPORTED);
    }
}

unsigned int getShort(unsigned char *buffer) {
  return (buffer[0] << 8) | buffer[1];
}

/**
 * Initialise the ECC domain parameters and generate a fresh key pair
 *
 * @param buffer containing the domain parameters
 */
void initialise(unsigned char *buffer) {
  unsigned int length, offset = 0;

  if (initialised) {
    debugWarning("Already initialised");
    APDU_ReturnSW(ISO7816_SW_COMMAND_NOT_ALLOWED_AGAIN);
  }

  domainParams.format = 0x00; // format of domain params
  domainParams.h = 0x01; // cofactor
  domainParams.bytes = buffer[offset + 1];
  debugInteger("bytes", domainParams.bytes);

  // P
  length = getShort(buffer + offset);
  offset += 2;
  if (length > ECC_KEY_BYTES) {
    debugError("OVERFLOW");
    APDU_ReturnSW(SW_WRONG_LENGTH);
  }
  memcpy(domainParams.p + ECC_KEY_BYTES - length, buffer + offset, length);
  offset += length;
  debugValue("Initialised P", domainParams.p, ECC_KEY_BYTES);

  // R
  length = getShort(buffer + offset);
  offset += 2;
  if (length > ECC_KEY_BYTES) {
    debugError("OVERFLOW");
    APDU_ReturnSW(SW_WRONG_LENGTH);
  }
  memcpy(domainParams.r + ECC_KEY_BYTES - length, buffer + offset, length);
  offset += length;
  debugValue("Initialised R", domainParams.r, ECC_KEY_BYTES);

  // A
  length = getShort(buffer + offset);
  offset += 2;
  if (length > ECC_KEY_BYTES) {
    debugError("OVERFLOW");
    APDU_ReturnSW(SW_WRONG_LENGTH);
  }
  memcpy(domainParams.a + ECC_KEY_BYTES - length, buffer + offset, length);
  offset += length;
  debugValue("Initialised A", domainParams.a, ECC_KEY_BYTES);

  // B
  length = getShort(buffer + offset);
  offset += 2;
  if (length > ECC_KEY_BYTES) {
    debugError("OVERFLOW");
    APDU_ReturnSW(SW_WRONG_LENGTH);
  }
  memcpy(domainParams.b + ECC_KEY_BYTES - length, buffer + offset, length);
  offset += length;
  debugValue("Initialised B", domainParams.b, ECC_KEY_BYTES);

  // G
  length = getShort(buffer + offset);
  offset += 2;
  if (buffer[offset++] != 0x04) {
    debugError("Unsupported point encoding");
    APDU_ReturnSW(SW_WRONG_DATA);
  }
  memcpy(&(domainParams.G), buffer + offset, domainParams.bytes * 2);
  offset += domainParams.bytes * 2;
  debugValue("Initialised G.x", domainParams.G.x, ECC_KEY_BYTES);
  debugValue("Initialised G.y", domainParams.G.y, ECC_KEY_BYTES);

  // Generate keys
  ECC_generate_keys(&domainParams, &keyPair);
  debugValue("Initialised keyPair", &keyPair, sizeof(ECC_key_pair));
  debugValue(" - private", keyPair.privateKey, ECC_KEY_BYTES);
  debugValue(" - public.x", keyPair.publicKey.x, ECC_KEY_BYTES);
  debugValue(" - public.y", keyPair.publicKey.y, ECC_KEY_BYTES);

  initialised = 1;
}

/**
 * Personalise the card with a number of attributes
 *
 * @param buffer containing the attributes to be stored on the card
 */
void personalise(unsigned char *buffer) {
  unsigned int i, index, count, offset = 0;

  // Get the number of attributes
  count = (buffer[offset] << 8) | buffer[offset + 1];
  offset += 2;
  debugInteger("count", count);

  // Process each attribute
  for (i = 0; i < count; i++) {
    index = buffer[offset] - 1;
    debugInteger("index", index);
    // Store the attribute ID
    attribute[index].id = buffer[offset++];
    debugInteger("ID", attribute[index].id);

    // Store the attribute signature
    if (buffer[offset++] != 0x04) {
      debugError("Unsupported point encoding");
      APDU_ReturnSW(SW_WRONG_DATA);
    }
    memcpy(&(attribute[index].signature), buffer + offset, sizeof(ECC_point));
    offset += sizeof(ECC_point);
    debugValue("signature", &(attribute[index].signature), sizeof(ECC_point));

    // Store the attribute length
    attribute[index].length = (buffer[offset] << 8) | buffer[offset + 1];
    offset += 2;
    debugInteger("length", attribute[index].length);

    // Store the attribute value
    memcpy(attribute[index].value, buffer + offset, attribute[index].length);
    offset += attribute[index].length;
    debugValue("value", attribute[index].value, attribute[index].length);
  }
}

/**
 * Generate an attribute prove and store it in the buffer
 *
 * @param buffer containing the attribute request, in which the attribute will be stored
 * @return number of bytes stored in the buffer
 */
unsigned int getAttribute(unsigned char *buffer) {
	unsigned int length = 0, index = 0, offset = 0;

  // Get the index, i.e. look-up the id, throw exception if not found
  unsigned char id = buffer[offset++];
  while (index < ATTRIBUTE_COUNT && attribute[index].id != id) {
   	index++;
  }
  if (index >= ATTRIBUTE_COUNT || attribute[index].id != id) {
   	APDU_ReturnSW(SW_RECORD_NOT_FOUND);
  }

  // Use the same domain parameters for the blinding
  memcpy(&blindParams, &domainParams, sizeof(ECC_domain_params));

  // Get the nonce send by the terminal
  length = (buffer[offset] << 8) | buffer[offset + 1];
	offset += 2;
  if (buffer[offset++] != 0x04) {
    debugError("Unsupported point encoding");
    APDU_ReturnSW(SW_WRONG_DATA);
  }
  if (length != ECC_KEY_BYTES * 2 + 1) {
    debugError("Wrong length");
    APDU_ReturnSW(SW_WRONG_LENGTH);
  }
  memcpy(&(blindParams.G), buffer + offset, blindParams.bytes * 2);
  offset += blindParams.bytes * 2;
  debugValue("N.x", blindParams.G.x, ECC_KEY_BYTES);
  debugValue("N.y", blindParams.G.y, ECC_KEY_BYTES);

	offset = 0;

	// Generate a blinding factor b, store it in blinder and blindKey
  ECC_generate_keys(&blindParams, &blindPair);
  debugValue("Generated blinding factor", &blindPair, sizeof(ECC_key_pair));
  debugValue(" - private (blinding factor)", blindPair.privateKey, ECC_KEY_BYTES);
  debugValue(" - public.x (blinded N)", blindPair.publicKey.x, ECC_KEY_BYTES);
  debugValue(" - public.y (blinded N)", blindPair.publicKey.y, ECC_KEY_BYTES);

	// Sign the nonce using the private key
	buffer[offset++] = ECC_KEY_BYTES >> 8;
	buffer[offset++] = ECC_KEY_BYTES & 0xFF;
	ECC_diffie_hellman(&domainParams, &(keyPair.privateKey), &(blindPair.publicKey), buffer + offset);
	debugValue("Signed Nonce", buffer + offset, ECC_KEY_BYTES);
  offset += ECC_KEY_BYTES;


  // Blind the public key using the blinding factor
	buffer[offset++] = ECC_KEY_BYTES >> 8;
	buffer[offset++] = ECC_KEY_BYTES & 0xFF;
	ECC_diffie_hellman(&domainParams, &(blindPair.privateKey), &(keyPair.publicKey), buffer + offset);
	debugValue("Blinded key", buffer + offset, ECC_KEY_BYTES);
  offset += ECC_KEY_BYTES;

  // Blind attribute signature, which is at attr_index + 2*lengthvalues.length + attribute_value.length
	buffer[offset++] = ECC_KEY_BYTES >> 8;
	buffer[offset++] = ECC_KEY_BYTES & 0xFF;
	ECC_diffie_hellman(&domainParams, &(blindPair.privateKey), &(attribute[index].signature), buffer + offset);
	debugValue("Blinded signature", buffer + offset, ECC_KEY_BYTES);
  offset += ECC_KEY_BYTES;

  // Append attribute
  buffer[offset++] = attribute[index].length >> 8;
  buffer[offset++] = attribute[index].length & 0xFF;
  memcpy(buffer + offset, attribute[index].value, attribute[index].length);
  debugValue("attribute", buffer + offset, attribute[index].length);
  offset += attribute[index].length;

  return offset;
}

/**
 * Store the cards public key in the buffer
 *
 * @param buffer in which the key will be stored
 * @return number of bytes stored in the buffer
 */
unsigned int getKey(unsigned char *buffer) {
  unsigned int length = sizeof(ECC_public_key) + 1, offset = 0;

  // Length
  buffer[offset++] = length >> 8;
  buffer[offset++] = length & 0x00FF;
  buffer[offset++] = 0x04;

  // Value
  memcpy(buffer + offset, &(keyPair.publicKey), sizeof(ECC_public_key));
  offset += sizeof(ECC_public_key);

  return offset;
}

unsigned int computeDH(unsigned char *buffer) {
  unsigned int length, offset = 0;

  length = getShort(buffer + offset);
  offset += 2;
  memcpy(&(x.x) + ECC_KEY_BYTES - length, buffer + offset, length);
  offset += length;
  length = getShort(buffer + offset);
  offset += 2;
  if (buffer[offset++] != 0x04) {
    debugError("Unsupported point encoding");
    APDU_ReturnSW(SW_WRONG_DATA);
  }
  if (length != ECC_KEY_BYTES * 2 + 1) {
    debugError("Wrong length");
    APDU_ReturnSW(SW_WRONG_LENGTH);
  }
  memcpy(&P, buffer + offset, ECC_KEY_BYTES * 2);

  debugValue("x", &x, ECC_KEY_BYTES *2);
  debugValue("P", &P, ECC_KEY_BYTES *2);
  memset(buffer, 0x00, ECC_KEY_BYTES * 2);
  ECC_diffie_hellman(&domainParams, &x, &P, buffer);

  return ECC_KEY_BYTES * 2;
}
