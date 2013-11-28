/**
 * debug.c
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
 * Copyright (C) July 2011 - 2013.
 *   Pim Vullers <pim@cs.ru.nl>, Radboud University Nijmegen.
 */

#include "debug.h"

#ifdef SIMULATOR

/**
 * Print a value as debug output.
 *
 * @param label associated with the value in the output.
 * @param value to be printed.
 * @param length in bytes of the value.
 */
void debugValue(const char *label, const void *value,
    unsigned int length) {
  int i;

  printf("%s: ", label);
  for (i = 0; i < length; i++) {
    printf("%02X", ((unsigned char *) value)[i]);
  }
  printf("\n");
}

/**
 * Print an indexed value (from an array) as debug output.
 *
 * @param label associated with the value in the output.
 * @param array containing the value to be printed.
 * @param length in bytes of the values in the array.
 * @param index of the value in the array.
 */
void debugIndexedValue(const char *label, const void *array,
    unsigned int length, unsigned int index) {
  int i;

  printf("%s[%d]: ", label, index);
  for (i = 0; i < length; i++) {
    printf("%02X", (((unsigned char *) array) + (index * length))[i]);
  }
  printf("\n");
}

/**
 * Print the values (from an array) as debug output.
 *
 * @param label associated with the values in the output.
 * @param array containing the values to be printed.
 * @param length in bytes of the values in the array.
 * @param count number of values in the array.
 */
void debugValues(const char *label, const void *array,
    unsigned int length, unsigned int count) {
  int i;

  for (i = 0; i < count; i++) {
    debugIndexedValue(label, array, length, i);
  }
}

#endif // SIMULATOR
