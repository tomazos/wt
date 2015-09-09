#pragma once

#include <cstring>
#include <vector>

/**
  *  Function to compute SHA3-224 on the input message. The output length is
 * fixed to 28 bytes.
  */

inline void SHAKE128(const void *input, unsigned int inputByteLen, void *output,
                     int outputByteLen);

template <typename Collection>
std::vector<uint8> SHAKE128(const Collection &input, int outputByteLen) {
  std::vector<uint8> output(outputByteLen);
  SHAKE128(input.data(), input.size(), output.data(), outputByteLen);
  return output;
}

inline void SHAKE256(const void *input, unsigned int inputByteLen, void *output,
                     int outputByteLen);

template <typename Collection>
std::vector<uint8> SHAKE256(const Collection &input, int outputByteLen) {
  std::vector<uint8> output(outputByteLen);
  SHAKE256(input.data(), input.size(), output.data(), outputByteLen);
  return output;
}

inline void SHA3_224(const void *input, unsigned int inputByteLen,
                     void *output);

template <typename Collection>
std::array<uint8, 224 / 8> SHA3_224(const Collection &input) {
  std::array<uint8, 224 / 8> output;
  SHA3_224(input.data(), input.size(), output.data());
  return output;
}

/**
  *  Function to compute SHA3-256 on the input message. The output length is
 * fixed to 32 bytes.
  */
inline void SHA3_256(const void *input, unsigned int inputByteLen,
                     void *output);

template <typename Collection>
std::array<uint8, 256 / 8> SHA3_256(const Collection &input) {
  std::array<uint8, 256 / 8> output;
  static_assert(sizeof(typename Collection::value_type) == 1, "bad collection");
  SHA3_256(input.data(), input.size(), output.data());
  return output;
}

/**
  *  Function to compute SHA3-384 on the input message. The output length is
 * fixed to 48 bytes.
  */

inline void SHA3_384(const void *input, unsigned int inputByteLen,
                     void *output);

template <typename Collection>
std::array<uint8, 384 / 8> SHA3_384(const Collection &input) {
  std::array<uint8, 384 / 8> output;
  SHA3_384(input.data(), input.size(), output.data());
  return output;
}

/**
  *  Function to compute SHA3-512 on the input message. The output length is
 * fixed to 64 bytes.
  */
inline void SHA3_512(const void *input, unsigned int inputByteLen,
                     void *output);

template <typename Collection>
std::array<uint8, 512 / 8> SHA3_512(const Collection &input) {
  std::array<uint8, 512 / 8> output;
  SHA3_512(input.data(), input.size(), output.data());
  return output;
}

/**
  * Function to compute the Keccak[r, c] sponge function over a given input.
  * @param  rate            The value of the rate r.
  * @param  capacity        The value of the capacity c.
  * @param  input           Pointer to the input message.
  * @param  inputByteLen    The number of input bytes provided in the input
 * message.
  * @param  delimitedSuffix Bits that will be automatically appended to the end
  *                         of the input message, as in domain separation.
  *                         This is a byte containing from 0 to 7 bits
  *                         These <i>n</i> bits must be in the least significant
 * bit positions
  *                         and must be delimited with a bit 1 at position
 * <i>n</i>
  *                         (counting from 0=LSB to 7=MSB) and followed by bits
 * 0
  *                         from position <i>n</i>+1 to position 7.
  *                         Some examples:
  *                             - If no bits are to be appended, then @a
 * delimitedSuffix must be 0x01.
  *                             - If the 2-bit sequence 0,1 is to be appended
 * (as for SHA3-*), @a delimitedSuffix must be 0x06.
  *                             - If the 4-bit sequence 1,1,1,1 is to be
 * appended (as for SHAKE*), @a delimitedSuffix must be 0x1F.
  *                             - If the 7-bit sequence 1,1,0,1,0,0,0 is to be
 * absorbed, @a delimitedSuffix must be 0x8B.
  * @param  output          Pointer to the buffer where to store the output.
  * @param  outputByteLen   The number of output bytes desired.
  * @pre    One must have r+c=1600 and the rate a multiple of 8 bits in this
 * implementation.
  */
inline void Keccak(unsigned int rate, unsigned int capacity,
                   const unsigned char *input,
                   unsigned long long int inputByteLen,
                   unsigned char delimitedSuffix, unsigned char *output,
                   unsigned long long int outputByteLen);

/**
  *  Function to compute SHAKE128 on the input message with any output length.
  */
inline void SHAKE128(const void *input, unsigned int inputByteLen, void *output,
                     int outputByteLen) {
  Keccak(1344, 256, (const uint8 *)input, inputByteLen, 0x1F, (uint8 *)output,
         outputByteLen);
}

/**
  *  Function to compute SHAKE256 on the input message with any output length.
  */
inline void SHAKE256(const void *input, unsigned int inputByteLen, void *output,
                     int outputByteLen) {
  Keccak(1088, 512, (const uint8 *)input, inputByteLen, 0x1F, (uint8 *)output,
         outputByteLen);
}

inline void SHA3_224(const void *input, unsigned int inputByteLen,
                     void *output) {
  Keccak(1152, 448, (const uint8 *)input, inputByteLen, 0x06, (uint8 *)output,
         28);
}

inline void SHA3_256(const void *input, unsigned int inputByteLen,
                     void *output) {
  Keccak(1088, 512, (const uint8 *)input, inputByteLen, 0x06, (uint8 *)output,
         32);
}

inline void SHA3_384(const void *input, unsigned int inputByteLen,
                     void *output) {
  Keccak(832, 768, (const uint8 *)input, inputByteLen, 0x06, (uint8 *)output,
         48);
}

inline void SHA3_512(const void *input, unsigned int inputByteLen,
                     void *output) {
  Keccak(576, 1024, (const uint8 *)input, inputByteLen, 0x06, (uint8 *)output,
         64);
}

template <typename T, typename U>
auto ROL64(T a, U offset) {
  return (((uint64)a) << offset) ^ (((uint64)a) >> (64 - offset));
}
#define ixxx(x, y) ((x) + 5 * (y))

#ifdef LITTLE_ENDIAN
#define readLane(x, y) (((uint64 *)state)[ixxx(x, y)])
#define writeLane(x, y, lane) (((uint64 *)state)[ixxx(x, y)]) = (lane)
#define XORLane(x, y, lane) (((uint64 *)state)[ixxx(x, y)]) ^= (lane)
#else
#error LITTLE ENDIAN REQUIRED
#endif

inline int LFSR86540(uint8 *LFSR) {
  int result = ((*LFSR) & 0x01) != 0;
  if (((*LFSR) & 0x80) != 0)
    (*LFSR) = ((*LFSR) << 1) ^ 0x71;
  else
    (*LFSR) <<= 1;
  return result;
}

inline void KeccakF1600_StatePermute(void *state) {
  unsigned int round, x, y, j, t;
  uint8 LFSRstate = 0x01;

  for (round = 0; round < 24; round++) {
    {
      uint64 C[5], D;

      for (x = 0; x < 5; x++)
        C[x] = readLane(x, 0) ^ readLane(x, 1) ^ readLane(x, 2) ^
               readLane(x, 3) ^ readLane(x, 4);
      for (x = 0; x < 5; x++) {
        D = C[(x + 4) % 5] ^ ROL64(C[(x + 1) % 5], 1);
        for (y = 0; y < 5; y++) XORLane(x, y, D);
      }
    }

    {
      uint64 current, temp;
      x = 1;
      y = 0;
      current = readLane(x, y);
      for (t = 0; t < 24; t++) {
        unsigned int r = ((t + 1) * (t + 2) / 2) % 64;
        unsigned int Y = (2 * x + 3 * y) % 5;
        x = y;
        y = Y;
        temp = readLane(x, y);
        writeLane(x, y, ROL64(current, r));
        current = temp;
      }
    }

    {
      uint64 temp[5];
      for (y = 0; y < 5; y++) {
        for (x = 0; x < 5; x++) temp[x] = readLane(x, y);
        for (x = 0; x < 5; x++)
          writeLane(x, y, temp[x] ^ ((~temp[(x + 1) % 5]) & temp[(x + 2) % 5]));
      }
    }

    {
      for (j = 0; j < 7; j++) {
        unsigned int bitPosition = (1 << j) - 1;  // 2^j-1
        if (LFSR86540(&LFSRstate)) XORLane(0, 0, (uint64)1 << bitPosition);
      }
    }
  }
}

template <typename T, typename U>
auto inline_min(T a, U b) {
  return a < b ? a : b;
}

inline void Keccak(unsigned int rate, unsigned int capacity,
                   const unsigned char *input,
                   unsigned long long int inputByteLen,
                   unsigned char delimitedSuffix, unsigned char *output,
                   unsigned long long int outputByteLen) {
  uint8 state[200];
  unsigned int rateInBytes = rate / 8;
  unsigned int blockSize = 0;
  unsigned int i;

  if (((rate + capacity) != 1600) || ((rate % 8) != 0)) return;

  std::memset(state, 0, sizeof(state));

  while (inputByteLen > 0) {
    blockSize = inline_min(inputByteLen, rateInBytes);
    for (i = 0; i < blockSize; i++) state[i] ^= input[i];
    input += blockSize;
    inputByteLen -= blockSize;

    if (blockSize == rateInBytes) {
      KeccakF1600_StatePermute(state);
      blockSize = 0;
    }
  }

  state[blockSize] ^= delimitedSuffix;
  if (((delimitedSuffix & 0x80) != 0) && (blockSize == (rateInBytes - 1)))
    KeccakF1600_StatePermute(state);
  state[rateInBytes - 1] ^= 0x80;
  KeccakF1600_StatePermute(state);

  while (outputByteLen > 0) {
    blockSize = inline_min(outputByteLen, rateInBytes);
    memcpy(output, state, blockSize);
    output += blockSize;
    outputByteLen -= blockSize;

    if (outputByteLen > 0) KeccakF1600_StatePermute(state);
  }
}
