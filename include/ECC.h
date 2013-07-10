/**
 * ECC.h
 *
 * This file is part of IRMAcard.
 *
 * IRMAcard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * IRMAcard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IRMAcard. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) July 2013 - 2013.
 *   Pim Vullers <pim@cs.ru.nl>, Radboud University Nijmegen.
 */

#ifndef __ECC_H
#define __ECC_H

#include "ECC.config.h"

#ifndef ECC_KEY_BITS
  #error "ECC_KEY_BITS not defined"
#endif // !ECC_KEY_BITS

#define ECC_BITS_TO_BYTES(bits) ((bits + 7) /8)

#define ECC_KEY_BYTES ECC_BITS_TO_BYTES(ECC_KEY_BITS)

typedef struct {
  unsigned char x[ECC_KEY_BYTES];
  unsigned char y[ECC_KEY_BYTES];
} ECC_point;

typedef ECC_point ECC_public_key;

typedef unsigned char ECC_private_key[ECC_KEY_BYTES];

typedef struct {
  ECC_public_key publicKey;
  ECC_private_key privateKey;
} ECC_key_pair;

typedef struct {
  unsigned char format;
  unsigned char bytes;
  unsigned char p[ECC_KEY_BYTES];
  unsigned char a[ECC_KEY_BYTES];
  unsigned char b[ECC_KEY_BYTES];
  ECC_point G;
  unsigned char r[ECC_KEY_BYTES];
  unsigned char h;
} ECC_domain_params;


#define ECC_generate_keys(params, keys) \
do { \
  __push(params); \
  __push(keys); \
  __code(PRIM, PRIM_ECC_GENERATE_KEY_PAIR, 0x00); \
} while (0)

#define ECC_diffie_hellman(params, privateKey, publicKey, sharedKey) \
do { \
  __push(params); \
  __push(privateKey); \
  __push(publicKey); \
  __push(sharedKey); \
  __code(PRIM, PRIM_ECC_ELLIPTIC_CURVE_DIFFIE_HELLMAN, 0x00); \
} while (0)

#endif // __ECC_H
