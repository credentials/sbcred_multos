/**
 * APDU.h
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

#ifndef __APDU_H
#define __APDU_H

#include "MULTOS.h"

/*
 * CLAss bytes
 */
#define CLA_ISO7816                0x00
#define CLA_SECURE_MESSAGING       0x0C
#define CLA_COMMAND_CHAINING       0x10
#define CLA_IRMACARD               0x80

/*
 * INStruction bytes
 */
#define INS_VERIFY                 0x20
#define INS_GET_CHALLENGE          0xB4
#define INS_EXTERNAL_AUTHENTICATE  0x82
#define INS_INTERNAL_AUTHENTICATE  0x88
#define INS_GET_RESPONSE           0xC0
#define INS_CHANGE_REFERENCE_DATA  0x24
#define INS_MANAGE_SECURITY_ENVIRONMENT 0x22
#define INS_PERFORM_SECURITY_OPERATION 0x2A

#define INS_GENERATE_SECRET        0x01
#define INS_AUTHENTICATION_SECRET  0x02

#define INS_ISSUE_CREDENTIAL       0x10
#define INS_ISSUE_PUBLIC_KEY       0x11
#define INS_ISSUE_ATTRIBUTES       0x12

#define INS_ISSUE_COMMITMENT       0x1A
#define INS_ISSUE_COMMITMENT_PROOF 0x1B
#define INS_ISSUE_CHALLENGE        0x1C
#define INS_ISSUE_SIGNATURE        0x1D
#define INS_ISSUE_SIGNATURE_PROOF  0x1E
#define INS_ISSUE_VERIFY           0x1F

#define INS_PROVE_CREDENTIAL       0x20

#define INS_PROVE_COMMITMENT       0x2A
#define INS_PROVE_SIGNATURE        0x2B
#define INS_PROVE_ATTRIBUTE        0x2C

#define INS_ADMIN_CREDENTIAL       0x30
#define INS_ADMIN_REMOVE           0x31
#define INS_ADMIN_ATTRIBUTE        0x32
#define INS_ADMIN_FLAGS            0x33

#define INS_ADMIN_CREDENTIALS      0x3A
#define INS_ADMIN_LOG              0x3B

/*
 * Parameter bytes
 */
#define P1_AUTH_EXPONENT    0x00
#define P1_AUTH_MODULUS     0x01

#define P1_PUBLIC_KEY_N     0x00
#define P1_PUBLIC_KEY_S     0x01
#define P1_PUBLIC_KEY_Z     0x02
#define P1_PUBLIC_KEY_R     0x03

#define P2_CRED_PIN         0x00
#define P2_CARD_PIN         0x01

#define P1_PROOF_VERIFY     0x00
#define P1_PROOF_C          0x01
#define P1_PROOF_VPRIMEHAT  0x02
#define P1_PROOF_SHAT       0x03
#define P1_PROOF_S_E        0x04

#define P1_SIGNATURE_VERIFY 0x00
#define P1_SIGNATURE_A      0x01
#define P1_SIGNATURE_E      0x02
#define P1_SIGNATURE_V      0x03

#define P1_SIGNATURE_PROOF_C      0x04
#define P1_SIGNATURE_PROOF_S_E    0x05
#define P1_SIGNATURE_PROOF_VERIFY 0x06
/*
 * Status Word bytes
 */
#define SW_BYTES_REMAINING(SW2)           0x61##SW2
#define SW_COUNTER_PROVIDED_BY_X(X)       0x63C##X
#define SW_WRONG_LENGTH                   0x6700
#define SW_FUNCTIONS_IN_CLA_NOT_SUPPORTED 0x6800
#define SW_LOGICAL_CHANNEL_NOT_SUPPORTED  0x6881
#define SW_SECURE_MESSAGING_NOT_SUPPORTED 0x6882
#define SW_COMMAND_NOT_ALLOWED            0x6900
#define SW_SECURITY_STATUS_NOT_SATISFIED  0x6982
#define SW_FILE_INVALID                   0x6983
#define SW_DATA_INVALID                   0x6984
#define SW_CONDITIONS_NOT_SATISFIED       0x6985
#define SW_COMMAND_NOT_ALLOWED_AGAIN      0x6986
#define SW_WRONG_PARAMS                   0x6A00
#define SW_WRONG_DATA                     0x6A80
#define SW_FUNC_NOT_SUPPORTED             0x6A81
#define SW_FILE_NOT_FOUND                 0x6A82
#define SW_RECORD_NOT_FOUND               0x6A83
#define SW_INCORRECT_P1P2                 0x6A86
#define SW_REFERENCED_DATA_NOT_FOUND      0x6A88
#define SW_WRONG_P1P2                     0x6B00
#define SW_CORRECT_LENGTH(SW2)            0x6C##SW2
#define SW_INS_NOT_SUPPORTED              0x6D00
#define SW_CLA_NOT_SUPPORTED              0x6E00
#define SW_UNKNOWN                        0x6F00
#define SW_NO_ERROR                       0x9000

#define TAG_ISO7816_FCI          0x6F
#define TAG_PROPRIETARY_INFO     0x85
#define TAG_PROPRIETARY_SECURITY 0x86
#define TAG_PROPRIETARY_INFO_TLV 0xA5

#define APDU_wrapped ((CLA & CLA_SECURE_MESSAGING) != 0)
#define APDU_chained ((CLA & CLA_COMMAND_CHAINING) != 0)

#define APDU_checkP1(value) \
  if (P1 != value) { APDU_returnSW(SW_WRONG_P1P2); }

#define APDU_checkP1range(lower, upper) \
  if (P1 < lower || P1 > upper) { APDU_returnSW(SW_WRONG_P1P2); }

#define APDU_checkP2(value) \
  if (P2 != value) { APDU_returnSW(SW_WRONG_P1P2); }

#define APDU_checkP2upper(upper) \
  if (P2 > upper) { APDU_returnSW(SW_WRONG_P1P2); }

#define APDU_checkP2range(lower, upper) \
  if (P2 < lower || P2 > upper) { APDU_returnSW(SW_WRONG_P1P2); }

#define APDU_checkP1P2(value) \
  if (P1P2 != value) { APDU_returnSW(SW_WRONG_P1P2); }

#define APDU_checkLength(length) \
  if (Lc != (length)) { APDU_returnSW(SW_WRONG_LENGTH); }


#define APDU_ReturnSW(sw) \
  __SW = (sw); \
  __code(SYSTEM, 4)

#define APDU_ReturnSWLa(sw, len) \
  __SW = (sw); \
  __La = (len); \
  __code(SYSTEM, 4)

#define APDU_return() \
  APDU_returnSW(SW_NO_ERROR);

#define APDU_returnLa(la) \
  APDU_returnSWLa(SW_NO_ERROR, la);

#define APDU_returnSW(sw) \
  __SW = (sw); \
  return;

#define APDU_returnSWLa(sw, la) \
  __SW = (sw); \
  __La = (la); \
  return;


extern unsigned char CLA;
extern unsigned char INS;
extern unsigned char P1;
extern unsigned char P2;
extern unsigned int P1P2; /* P1 in MSB, P2 in LSB. */
extern unsigned int Lc;
extern unsigned int Le;
extern unsigned int SW; /* SW1 in MSB, SW2 in LSB. */
extern unsigned int La;
extern unsigned int __SW; /* SW1 in MSB, SW2 in LSB. */
extern unsigned int __La;

unsigned char CheckCase(unsigned char isocase);

#endif // __APDU_H
