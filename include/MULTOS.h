/**
 * MULTOS.h
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
 * Copyright (C) March 2013 - 2013.
 *   Pim Vullers <pim@cs.ru.nl>, Radboud University Nijmegen.
 */

#ifndef __MULTOS_H
#define __MULTOS_H

#include <melasm.h>

/**
 * Load CCR
 *
 * This primitive pushes the Condition Code register onto the stack.
 *
 * Stack input:  []
 * Stack output: [ CCR ]
 *
 * The 1-byte output parameter CCR holds a byte whose value is the same
 * as that of the CCR.
 *
 * This primitive pushes one byte to the stack that contains the same
 * bit settings as the Condition Code Register.
 */
#define PRIM_LOAD_CCR 0x05

/**
 * Bit Manipulate Byte
 *
 * This primitive performs bit-wise operations on the top byte of the
 * stack.
 *
 * The argument Option is a bitmap controlling what logical operation is
 * performed and MaskByte is a literal byte holding the mask to use for
 * operation.
 *
 * Stack input:  [ ByteIn ]
 * Stack output: [ ByteOut ]
 *
 * The 1-byte parameter ByteIn is the byte value that will be
 * manipulated according to the binary operation specified by Option
 * using the literal MaskByte as the second operand. The 1-byte value
 * ByteOut depends on the Options argument. It may be the original byte
 * or the result of the logical operation.
 *
 * Depending on the Option argument this primitive performs one of four
 * binary logical operations. They are:
 *  - AND: which returns a true bit only if both corresponding bits in
 *    the input and mask are true
 *  - OR: which returns a true bit if either of the corresponding bits
 *    in the input or mask are true
 *  - XOR: This is a logical Exclusive OR operation, which returns a
 *    true bit only if either of the corresponding bits in the input or
 *    mask are true, but false if both are true
 *  - EQU: This logical operation is also known as a Exclusive NOR
 *    (XNOR), which returns a true bit only if both corresponding bits
 *    in the input and mask are of the same value
 *
 * The following table shows how the Option argument is coded. The
 * numbers in the top row correspond to the bit offset, where the most
 * significant bit occupies offset 7.
 *
 *  7  6  5  4  3  2  1  0  Comments
 * ---------------------------------
 *  0  -  -  -  -  -  -  -  Do not modify ByteIn
 *  1  -  -  -  -  -  -  -  Overwrite ByteIn with result of operation
 *  -  0  0  0  0  0  -  -  Any other values are undefined
 *  -  -  -  -  -  -  0  0  Calculate ByteIn XOR MaskByte
 *  -  -  -  -  -  -  0  1  Calculate ByteIn EQU MaskByte
 *  -  -  -  -  -  -  1  0  Calculate ByteIn OR MaskByte
 *  -  -  -  -  -  -  1  1  Calculate ByteIn AND MaskByte
 *
 * Regardless of whether the ByteIn value is modified, the Condition
 * Code Register reflects the result of the operation.
 */
#define PRIM_BIT_MANIPULATE_BYTE 0x01

/**
 * Memory Copy
 *
 * This primitive copies a block of bytes from one location to another.
 *
 * Stack input:  [ length | destAddr | sourceAddr ]
 * Stack output: []
 *
 * All of the parameters are 2 bytes in size. The value Length is the
 * number of bytes to copy. The values DestAddr and SourceAddr are,
 * respectively, the locations to where and from where the data is
 * copied.
 *
 * Where the number of bytes to be copied is a compile time constant and
 * Length is no more than 255 bytes the primitive Memory Copy Fixed
 * Length may be used.
 *
 * This primitive works correctly even if the source and destination
 * blocks overlap.
 */
#define PRIM_COPY 0x0C

/**
 * Shift Right
 *
 * This primitive performs a bitwise shift right on a block of bytes.
 *
 * Both arguments are 1-byte in size. Length gives the size of the data
 * block to be shifted and ShiftBits indicates the number of bits to
 * shift.
 *
 * Stack input:  [ BytesIn ]
 * Stack output: [ BytesOut ]
 *
 * The input parameter BytesIn is of size Length and is the byte block
 * to be shifted. The output parameter BytesOut is the byte block of
 * size Length that holds the result of ShiftBits shift operations on
 * BytesIn.
 *
 * This primitive bit-shifts data rightwards, filling the
 * most-significant bits with zeroes. The effect of the primitive is
 * undefined if any of the following is true:
 *  - ShiftBits is zero
 *  - Length is zero
 *  - ShiftBits >=8 * Length
 */
#define PRIM_SHIFT_RIGHT 0x03

/**
 * Memory Copy Fixed Length
 *
 * This primitive copies a block of bytes of a fixed length from one
 * location to another.
 *
 * The argument Length is the number of bytes to copy.
 *
 * Stack input:  [ destAddr | sourceAddr ]
 * Stack output: []
 *
 * All of the parameters are 2 bytes in size. The values DestAddr and
 * SourceAddr are, respectively, the locations to where and from where
 * the data is copied.
 *
 * The Length value is specified using a single byte. Therefore, the
 * maximum length of a block is 255 bytes.
 *
 * This primitive works correctly even if the blocks overlap.
 */
#define PRIM_COPY_FIXED 0x0E

/**
 * Memory Copy Non-Atomic
 *
 * This primitive copies a block of bytes from one location to another.
 * If the byte block is copied into the static area, data item
 * protection function will be disabled if possible.
 *
 * Stack input:  [ length | destAddr | srcAddr ]
 * Stack output: []
 *
 * All of the parameters are 2 bytes in size. The value Length is the
 * number of bytes to copy. The values DestAddr and SourceAddr are,
 * respectively, the locations to where and from where the data is
 * copied.
 *
 * This primitive works correctly even if the source and destination
 * blocks overlap.
 *
 * Where the number of bytes to be copied is a compile time constant and
 * Length is no more than 255 bytes the primitive Memory Copy Non-Atomic
 * Fixed Length may be used.
 *
 * When copying into the static memory area with this primitive, the
 * copying will be performed more quickly than with Memory Copy
 * primitive as the data items are not protected.
 *
 * This primitive is a request for a non-atomic memory copy. Non-atomic
 * means that the data will be written in EEPROM page size blocks (see
 * [MIR] for page size information for a specific implementation) when
 * complete pages are available. If the data being copied results in
 * writing to only a part of a page, then MULTOS will revert to an
 * atomic copy. Whilst this copy operation may be faster the data in the
 * destination will not be protected if power-off occurred during the
 * copying to the static area. MULTOS will always guarantee the
 * integrity of data other than the data being copied.
 */
#define PRIM_COPY_NON_ATOMIC 0x0F

/**
 * Memory Copy Non-Atomic Fixed Length
 *
 * This primitive copies a block of bytes of a fixed length from one
 * location to another. If the byte block is copied into the static
 * area, data item protection function will be disabled if possible.
 *
 * The argument Length is the number of bytes to copy.
 *
 * Stack input:  [ destAddr | sourceAddr ]
 * Stack output: []
 *
 * All of the parameters are 2 bytes in size. The values DestAddr and
 * SourceAddr are, respectively, the locations to where and from where
 * the data is copied.
 *
 * This primitive works correctly even if the source and destination
 * blocks overlap.
 *
 * When copying into the static memory area with this primitive, the
 * copying will be performed more quickly than with Memory Copy
 * primitive as the data items are not protected.
 *
 * This primitive is a request for a non-atomic memory copy. Non-atomic
 * means that the data will be written in EEPROM page size blocks (see
 * [MIR] for page size information for a specific implementation) when
 * complete pages are available. If the data being copied results in
 * writing to only a part of a page, then MULTOS will revert to an
 * atomic copy. Whilst this copy operation may be faster the data in the
 * destination will not be protected if power-off occurred during the
 * copying to the static area. MULTOS will always guarantee the
 * integrity of data other than the data being copied.
 */
#define PRIM_COPY_FIXED_NON_ATOMIC 0x13

/**
 * Memory Compare
 *
 * This primitive compares two blocks of bytes to determine if they hold
 * the same data.
 *
 * Stack input:  [ length | addr1 | addr2 ]
 * Stack output: []
 *
 * The 2-byte parameter Length gives the size of the memory areas to be
 * compared. The 2-byte values Addr1 and Addr2 are the locations of the
 * areas.
 *
 * The comparison performed by this primitive is based on subtraction.
 * The second operand, the area corresponding to the address on the top
 * of the stack, is subtracted from the first. No data is modified, but
 * the Condition Code Register is set according to the result of the
 * operation.
 *
 * There are three possible results of the comparison of blocks of size
 * Length. They and the CCR setting used to indicate that result are:
 *  - When the byte block at Addr1 > the byte block at Addr2, both CCR C
 *    and CCR Z flags are cleared.
 *  - When the byte block at Addr1 = the byte block at Addr2, the CCR C
 *    flag is cleared and CCR Z flag is set.
 *  - When the byte block at Addr1 < the byte block at Addr2, the CCR C
 *    flag is set and CCR Z flag is cleared.
 *
 * Where the number of bytes to be compared is a compile time constant
 * and Length is no more than 255 bytes the primitive Memory Compare
 * Fixed Length may be used.
 */
#define PRIM_COMPARE 0x0B

/**
 * Get Random Number
 *
 * This primitive places an eight byte random number onto the stack.
 *
 * Stack input:  []
 * Stack output: [ bytes ]
 *
 * The output parameter Bytes holds the 8-byte block of random data
 * returned by the primitive.
 *
 * The method of random number generation is implementation specific.
 * So, it may be generated using a hardware assisted 'true' random
 * number generator or it may be generated as a pseudo-random number
 * from a seed value. In either case, the process is performed in such a
 * way that the secrecy of the number is guaranteed. It is not possible
 * for any coresident application to determine what number was provided
 * or will be provided subsequently.
 */
#define PRIM_RANDOM_NUMBER 0xC4

/**
 * SHA-1
 *
 * This primitive calculates the SHA-1 hash digest of a message of
 * arbitrary length (including 0 bytes).
 *
 * Stack input:  [ lenMsg | addrHash | addrMsg ]
 * Stack output: []
 *
 * Each of the input parameters is 2-bytes in size. The value lenMsg is
 * the size of the input to the SHA-1 algorithm. The second parameter
 * addrHash is the location where the 20-byte hash digest will be
 * written. The parameter addrMsg is the location of the input of size
 * lenMsg.
 */
#define PRIM_SHA1 0xCA

/**
 * Secure Hash
 *
 * This primitive calculates the SHA-1, SHA-224, SHA-256, SHA-384 or
 * SHA-512 digest of a message of arbitrary length (including 0 bytes)
 * in accordance with [FIPS180-3].
 *
 * Stack input:  [ lenMsg | lenHash | addrHash | addrMsg ]
 * Stack output: []
 *
 * Each of the input parameters is 2-bytes in size. The value lenMsg is
 * the size in bytes of the input to the Secure Hash algorithm. The
 * value lenHash is either 20, 28, 32, 48 or 64 and is the size of the
 * resultant hash digest (computed using the SHA-1, SHA-224, SHA-256,
 * SHA-384 and SHA-512 algorithms, respectively). Other lengths will
 * cause an abend. The parameter addrHash is the location where the hash
 * digest will be written. The parameter addrMsg is the location of the
 * input of size lenMsg.
 */
#define PRIM_SECURE_HASH 0xCF

/**
 * Secure Hash IV
 *
 * This primitive calculates the SHA-1, SHA-224, SHA-256, SHA-384 or
 * SHA-512 digest of a message of arbitrary length (including 0 bytes)
 * in accordance with [FIPS180-3] with the ability to pass a previously
 * calculated intermediate hash value and message remainder (where the
 * previous message was not block-aligned) to the algorithm.
 *
 * Stack in:  [ lenMsg | lenHash | addrHash | addrMsg |
 *             addrIntermediateHash | addrPrevHashedBytes |
 *             lenMessageRemainder | addrMessageRemainder ]
 * Stack out: [ lenMessageRemainder | addrMessageRemainder ]
 *
 * Each of the input parameters is 2-bytes in size. The value lenMsg is
 * the size in bytes of the input to the Secure Hash algorithm. The
 * value lenHash is either 20, 28, 32, 48 or 64 and is the size of the
 * resultant hash digest (computed using the SHA-1, SHA-224, SHA-256,
 * SHA-384 and SHA-512 algorithms, respectively). The parameter addrHash
 * is the location where the hash digest will be written. The parameter
 * addrMsg is the location of the input message of size lenMsg. The
 * parameter addrIntermediateHash is the location of the previously
 * calculated intermediate hash value input to the algorithm and output
 * from the algorithm. It is 20, 32 or 64 bytes in length dependent upon
 * the algorithm requested. The parameter addrPrevHashedBytes is the
 * location of the 4 byte (32-bit) counter indicating the number of
 * bytes previously input to the hashing algorithm, including previous
 * calculations. The parameter lenMessageRemainder is the number
 * remaining non-block aligned bytes from a previously hashed message.
 * The parameter addrMessageRemainder is the address of the remaining
 * non-block aligned bytes of a previously hashed message, of length
 * lenMessageRemainder.
 *
 * If the value at addrIntermediateHash is all zeros, then the algorithm
 * shall replace this value with the standard IV value used by the
 * algorithm, as specified in [FIPS180-3].
 *
 * The 32-bit value at addrPrevHashedBytes is the number of bytes
 * previously hashed by a call to this primitive or an alternative
 * calculation method. If the value at this address is zero, the
 * primitive will start a new hash calculation and ignore the values
 * contained at addrIntermediateHash and addrMessageRemainder. This
 * value is updated by the primitive and may serve as input to a
 * subsequent call to the primitive.
 *
 * If lenMessageRemainder is zero, the value at addrMessageRemainder
 * will be ignored, but value at addrIntermediateHash will still be used
 * as the input value to the algorithm.
 *
 * Following calculation, the memory at location addrIntermediateHash
 * shall contain the last intermediate hash value H(n) calculated by the
 * algorithm prior to any truncation when performing a SHA-224 or
 * SHA-384 algorithm. This value may serve as input to a subsequent call
 * to the primitive. The memory at addrHash will always contain a final
 * hash value complete with truncation if applicable.
 *
 * If the message hashed (the value at addrMessageRemainder prepended to
 * the value at addrMsg) is not block aligned (i.e. not a multiple of
 * either 32 or 64 bytes depending upon the hash algorithm), following
 * calculation of the intermediate value, the remainder of the message
 * of length lenMessageRemainder shall be pointed to by
 * addrMessageRemainder and will be placed on the returned stack. This
 * memory address may be within the area starting at addrMsg for length
 * lenMsg or it may be at the address passed to the primitive.
 *
 * Developers should ensure that there is sufficient memory at address
 * addrMessageRemainder to contain the message remainder of the
 * appropriate block size, as the returned message remainder can be
 * longer than the input message remainder. If a developer does not
 * allocate such a memory area, then the primitive may overwrite memory
 * beyond addrMessageRemainder + lenMessageRemainder or abend.
 */
#define PRIM_SECURE_HASH_IV 0xE4

/**
 * MultiplyN
 *
 * This primitive multiplies two unsigned blocks of bytes from the stack
 * together and leaves the result on the stack.
 *
 * The argument Length indicates the size of the multiplicands.
 *
 * Stack input:  [ Operand1 | Operand2 ]
 * Stack output: [ Output ]
 *
 * The parameters Operand1 and Operand2 are the values of size Length
 * that are to be multiplied. The output parameter Output holds the
 * result of the multiplication is twice the size of Length.
 *
 * This primitive performs unsigned multiplication of two numbers. The
 * result replaces the two operands at the top of stack.
 *
 * Note that it is possible for Operand1 and Operand2 to be placed at
 * the end of session data rather than on the stack.
 */
#define PRIM_MULTIPLY 0x10

/**
 * Modular Exponentiation / RSA Sign
 *
 * This primitive performs a modular exponentiation operation, the basis
 * of the RSA algorithm. This version of the primitive will execute with
 * full countermeasures to protect the algorithm.
 *
 * Stack input:  [ eLen | mLen | eAddr | mAddr | inAddr | outAddr ]
 * Stack output: []
 *
 * All parameters are 2 bytes in size. The values eLen and mLen
 * represent the length of the exponent and modulus respectively. These
 * lengths represent the size in bytes. The value eAddr is the location
 * of the exponent of size eLen, while mAddr is the location of the
 * modulus of size mLen. The addresses inAddr and outAddr are the
 * location of the input to the modular exponentiation operation and the
 * address to where the output will be written.
 *
 * This primitive performs modular exponentiation operation and the
 * result is written at the specified address outAddr.
 *
 * Moduli with length that is not a multiple of 8 bits are padded at the
 * least significant end with bits 0. So, a 1023-bit modulus would have
 * the least significant bit of the least significant byte set to 0.
 *
 * The size of the input and output is considered to the same as that of
 * the modulus. They are all mLen in size.
 *
 * The primitive will function normally if inAddr and outAddr point to
 * the same memory area. That is to say the output can overwrite the
 * input.
 *
 * In order to enable modular exponentiation to operate correctly there
 * are a number of general conditions that must be met:
 *  - The modulus must be odd.
 *  - The base value must be less than the modulus.
 *  - The exponent must be less than the modulus.
 *  - The length of the exponent must be less than or equal to the
 *    length of the modulus.
 *
 * There are some implementation specifics that may impact on the usage
 * of this primitive. For example, the most significant byte of the
 * modulus should not be zero although some platforms may permit it. As
 * another example, some implementations may only work on fixed key
 * lengths. It may also be the case that an implementation may provide
 * optimised support for an exponent length of 1 with a value of 3 and
 * from MULTOS 4.2 one may also provide optimised support for an
 * exponent length of 3 and a value of 65537. See the MULTOS
 * Implementation Report [MIR] for specific information.
 */
#define PRIM_MODULAR_EXPONENTIATION 0xC8

/**
 * RSA Verify / Modular Exponentiation
 *
 * This primitive performs a modular exponentiation operation, the basis
 * of the RSA algorithm. This version of the primitive is optimised for
 * use with Public key operations only and platform countermeasures that
 * protect the RSA algorithm may be disabled. For Private key operations
 * the Modular Exponentiation / RSA Sign primitive should be used.
 *
 * IT IS STRONGLY ADVISED THIS PRIMITIVE IS USED WITH PUBLIC KEYS ONLY.
 *
 * Stack input:  [ eLen | mLen | eAddr | mAddr | inAddr | outAddr ]
 * Stack output: []
 *
 * All parameters are 2 bytes in size. The values eLen and mLen
 * represent the length of the exponent and modulus respectively. These
 * lengths represent the size in bytes. The value eAddr is the location
 * of the exponent of size eLen, while mAddr is the location of the
 * modulus of size mLen. The addresses inAddr and outAddr are the
 * location of the input to the modular exponentiation operation and the
 * address to where the output will be written.
 *
 * This primitive performs modular exponentiation operation and the
 * result is written at the specified address outAddr.
 *
 * Moduli with length that is not a multiple of 8 bits are padded at the
 * least significant end with bits 0. So, a 1023-bit modulus would have
 * the least significant bit of the least significant byte set to 0.
 *
 * The size of the input and output is considered to the same as that of
 * the modulus. They are all mLen in size.
 *
 * The primitive will function normally if inAddr and outAddr point to
 * the same memory area. That is to say the output can overwrite the
 * input.
 *
 * In order to enable modular exponentiation to operate correctly there
 * are a number of general conditions that must be met:
 *  - The modulus must be odd.
 *  - The base value must be less than the modulus.
 *  - The exponent must be less than the modulus.
 *  - The length of the exponent must be less than or equal to the
 *    length of the modulus.
 *
 * There are some implementation specifics that may impact on the usage
 * of this primitive. For example, the most significant byte of the
 * modulus should not be zero although some platforms may permit it. As
 * another example, some implementations may only work on fixed key
 * lengths. It may also be the case that an implementation may provide
 * optimised support for an exponent length of 1 with a value of 3 and
 * from MULTOS 4.2 one may also provide optimised support for an
 * exponent length of 3 and a value of 65537. See the MULTOS
 * Implementation Report [MIR] for specific information.
 */
#define PRIM_RSA_VERIFY 0xEB

/**
 * Modular Multiplication
 *
 * This primitive multiples two operands and reduces the result of the
 * multiplication modulo a given modulus.
 *
 * Stack input:  [ lenMod | addrOp1 | addrOp2 | addrMod ]
 * Stack output: []
 *
 * All the parameters are 2 bytes in size. The parameter lenMod is the
 * size of the modulus supplied and located at addrMod. The parameters
 * addrOp1 and addrOp2 are the locations of the multiplicands.
 *
 * This primitive calculates a product modulo a modulus, that is
 * (Operand1 * Operand2) mod modulus. The result overwrites the first
 * operand.
 *
 * Both operands must represent values that are less than that of the
 * modulus. The modulus and both operands are considered to be of size
 * lenMod.
 */
#define PRIM_MODULAR_MULTIPLICATION 0xC2

/**
 * Generate Triple DES CBC Signature
 *
 * This primitive generates an 8-byte Triple DES CBC Signature over a
 * block of memory.
 *
 * Stack input:  [ Length | IVAddr | KeyAddr | SigAddr | InputAddr ]
 * Stack output: []
 *
 * All the parameters are 2-bytes in size. The parameter Length is the
 * size of the plaintext used as input to the signature generation
 * process. The value IVAddr is the location of an 8-byte initial
 * vector, KeyAddr is the location of two 8-byte DES key, SigAddr is the
 * location where the 8-byte signature is written and InputAddr is the
 * location of data of size Length to be signed.
 *
 * This primitive uses two 8-byte DES keys and operates in CBC mode. The
 * 16-byte key value assumes that the most significant 8 bytes are
 * "Key 1" and the least significant 8 bytes are "Key 2". At each step
 * the DES operations performed are: encipher using the Key 1, decipher
 * using Key 2, encipher using Key 1.
 *
 * The primitive operates only on complete 8-byte blocks in the
 * plaintext. If Length is not an integer multiple of 8, trailing bytes
 * are ignored. For example, if Length was 17 bytes, the 16 most
 * significant bytes would serve as input to the algorithm and the last
 * byte would be ignored. The parity bits of the key are ignored.
 */
#define PRIM_GENERATE_TRIPLE_DES_CBC_SIGNATURE 0xC7

/**
 * Block Decipher
 *
 * This primitive performs a Block Decipher on a block of memory. The
 * algorithms that may be used are DES, 3DES, SEED and AES in ECB and
 * CBC modes of operation.
 *
 * The 1-byte argument AlgorithmID indicates the type of decipher
 * algorithm to be used (DES: 0x03, 3DES: 0x04, SEED: 0x05, AES: 0x06).
 * The 1-byte argument Chaining Mode indicates the block cipher mode of
 * operation to be used (ECB: 0x01, CBC: 0x02).
 *
 * Stack input (ECB): [ InputLen | KeyAddr | KeyLen | OutputAddr |
 *                     InputAddr ]
 * Stack input (CBC): [ IVLen | IVAddr | InputLen | KeyAddr | KeyLen |
 *                     OutputAddr | InputAddr ]
 * Stack output:      []
 *
 * CBC mode requires the addition of an Initialisation Vector of length
 * equal to the block size for the selected algorithm. The 1-byte
 * parameter IVLen specifies the size of the Initialisation Vector. The
 * 2-byte parameter IVAddr is the address of the Initialisation Vector.
 * The size of the Initialisation Vector depends upon the specified
 * algorithm (DES: 8 bytes, 3DES: 8 bytes, SEED: 16 bytes, AES: 16
 * bytes).
 *
 * The 2-byte parameter InputLen specifies the number of bytes to
 * decipher. The 2-byte parameter KeyAddr is the address of the key(s)
 * to be used. The size and format of the key at this address depends
 * upon the specified algorithm (DES: one 8-byte DES key, 2-key 3DES:
 * two 8-byte DES keys of which the first key is located at address
 * KeyAddr and the second key at address KeyAddr + 8, 3-key 3DES: three
 * 8-byte DES keys of which the first key is located at address KeyAddr
 * and the second key at address KeyAddr + 8 and the third key at
 * address KeyAddr+16, SEED: one 16-byte key, AES: one 128 bit (16 byte)
 * AES key). The 1 byte parameter KeyLen is the length in bytes of the
 * key at address KeyAddr. The 2-byte parameter OutputAddr is the
 * starting address of the resultant plaintext. The 2-byte parameter
 * InputAddr is the start address of the ciphertext to be deciphered.
 *
 * This primitive performs the block decipher operation on a block of
 * memory of InputLen bytes. The algorithm used may be DES, 2-key 3DES,
 * 3-key 3DES, SEED and 128-bit key AES. The key is held in a block of
 * the appropriate length for the algorithm.
 *
 * The DES algorithms require that the ciphertext length is a multiple
 * of 8 bytes and the SEED and AES-128 algorithms require that the
 * ciphertext is a multiple of 16 bytes. If the ciphertext length does
 * not meet these restrictions then the primitive will abend. Padding is
 * not removed during the block decipher operation.
 *
 * The output is written at the specified segment address and this may
 * be the same as the address of the input; i.e., the output overwrites
 * the input. However, the output plaintext cannot partially overlap the
 * input ciphertext. If the primitive is called with partially
 * overlapping input and output memory areas then it abends. If an
 * Initialisation Vector is used then this can be at any segment
 * address, including in the input ciphertext or output plaintext memory
 * areas.
 */
#define PRIM_BLOCK_DECIPHER 0xDA

#define BLOCK_CIPHER_MODE_ECB 0x01
#define BLOCK_CIPHER_MODE_CBC 0x02

#define BLOCK_CIPHER_ALGORITHM_DES 0x03
#define BLOCK_CIPHER_ALGORITHM_3DES 0x04
#define BLOCK_CIPHER_ALGORITHM_SEED 0x05
#define BLOCK_CIPHER_ALGORITHM_AES 0x06

/**
 * Block Decipher
 *
 * This primitive performs a Block Encipher on a block of memory. The
 * algorithms that may be used are DES, 3DES, SEED and AES in ECB and
 * CBC modes of operation.
 *
 * The 1-byte argument AlgorithmID indicates the type of decipher
 * algorithm to be used (DES: 0x03, 3DES: 0x04, SEED: 0x05, AES: 0x06).
 * The 1-byte argument Chaining Mode indicates the block cipher mode of
 * operation to be used (ECB: 0x01, CBC: 0x02).
 *
 * Stack input (ECB): [ InputLen | KeyAddr | KeyLen | OutputAddr |
 *                     InputAddr ]
 * Stack input (CBC): [ IVLen | IVAddr | InputLen | KeyAddr | KeyLen |
 *                     OutputAddr | InputAddr ]
 * Stack output:      []
 *
 * CBC mode requires the addition of an Initialisation Vector of length
 * equal to the block size for the selected algorithm. The 1-byte
 * parameter IVLen specifies the size of the Initialisation Vector. The
 * 2-byte parameter IVAddr is the address of the Initialisation Vector.
 * The size of the Initialisation Vector depends upon the specified
 * algorithm (DES: 8 bytes, 3DES: 8 bytes, SEED: 16 bytes, AES: 16
 * bytes).
 *
 * The 2-byte parameter InputLen specifies the number of bytes to
 * encipher. The 2-byte parameter KeyAddr is the address of the key(s)
 * to be used. The size and format of the key at this address depends
 * upon the specified algorithm (DES: one 8-byte DES key, 2-key 3DES:
 * two 8-byte DES keys of which the first key is located at address
 * KeyAddr and the second key at address KeyAddr + 8, 3-key 3DES: three
 * 8-byte DES keys of which the first key is located at address KeyAddr
 * and the second key at address KeyAddr + 8 and the third key at
 * address KeyAddr+16, SEED: one 16-byte key, AES: one 128 bit (16 byte)
 * AES key). The 1 byte parameter KeyLen is the length in bytes of the
 * key at address KeyAddr. The 2-byte parameter OutputAddr is the
 * starting address of the resultant ciphertext. The 2-byte parameter
 * InputAddr is the start address of the plaintext to be enciphered.
 *
 * This primitive performs the block encipher operation on a block of
 * memory of InputLen bytes. The algorithm used may be DES, 2-key 3DES,
 * 3-key 3DES, SEED and 128-bit key AES. The key is held in a block of
 * the appropriate length for the algorithm.
 *
 * The DES algorithms require that the plaintext length is a multiple of
 *  8 bytes and the SEED and AES-128 algorithms require that the
 * plaintext is a multiple of 16 bytes. If the plaintext length does not
 * meet these restrictions then the primitive will abend.
 *
 * The output is written at the specified segment address and this may
 * be the same as the address of the input; i.e., the output overwrites
 * the input. However, the output ciphertext cannot partially overlap
 * the input plaintext. If the primitive is called with partially
 * overlapping input and output memory areas then it abends. If an
 * Initialisation Vector is used then this can be at any segment
 * address, including in the input plaintext or output ciphertext memory
 * areas.
 */
#define PRIM_BLOCK_ENCIPHER 0xDB

/**
 * ECC Generate Key Pair
 *
 * This primitive generates an Elliptic Curve Cryptography public and 
 * private key pair.
 *
 * The 1 byte argument Options is used to specify whether the private key
 * input will be maintained in a “protected” form (0x00 for no, 0x80 for
 * yes).
 *
 * Stack input:  [ domainAddr | keyAddr ]
 * Stack output: []
 *
 * All parameters are 2 bytes in size. domainAddr is the location of the 
 * elliptic curve domain parameter. keyAddr is the location where the key 
 * pair generated will be written.
 *
 * The key pair consists of the public key followed by the private key. 
 * The public key consists of ecc_X followed by ecc_Y and is of length 
 * prime_len x 2. The private key is of length prime_len.
 *
 * The Z flag is cleared on successful key pair generation.
 *
 * The format of the domain parameters is shown in the example below. 
 * P, A, B, Gx, Gy and N are prime_len long. The format, prime length and 
 * H are a single byte.
 */
#define PRIM_ECC_GENERATE_KEY_PAIR 0xE7

/**
 * ECC Elliptic Curve Diffie Hellman
 *
 * This primitive performs an Elliptic Curve Diffie Hellman key agreement 
 * in accordance with ANSI X9.63.
 *
 * The 1 byte argument Options is used to specify whether the private key 
 * input will be maintained in a “protected” form (0x00 for no, 0x80 for 
 * yes).
 * 
 * Stack input:  [ domainAddr | privatekeyAddr | publickeyAddr | sharedAddr ]
 * Stack output: []
 *
 * All parameters are 2 bytes in size. domainAddr is the location of the 
 * elliptic curve domain parameter. privatekeyAddr is the location of the 
 * private key to be used. publickeyAddr is the location of the public key
 * to be used. sharedAddr is the location where the shared secret key is 
 * written.
 *
 * The private key is of length prime_len. The public key consists of ecc_X
 * followed by ecc_Y and is of length prime_len x 2. The shared secret key 
 * is of length prime_len.
 *
 * The Z flag is cleared on successful processing.
 */
#define PRIM_ECC_ELLIPTIC_CURVE_DIFFIE_HELLMAN 0xE8

#endif // __MULTOS_H
