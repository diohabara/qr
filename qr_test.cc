#include "qr.h"

#include <gtest/gtest.h>

#include <bitset>

namespace {
TEST(QrTest, BitManipulation) {
  // bits manipulation
  // AB
  std::bitset<11> expected_elevent_bits_by_pair_ab("00111001101");
  auto actual_elevent_bits_by_pair_ab = eleven_bits_from_pair(10, 11);
  EXPECT_EQ(expected_elevent_bits_by_pair_ab, actual_elevent_bits_by_pair_ab);
  // CD
  std::bitset<11> expected_elevent_bits_by_pair_cd("01000101001");
  auto actual_elevent_bits_by_pair_cd = eleven_bits_from_pair(12, 13);
  EXPECT_EQ(expected_elevent_bits_by_pair_cd, actual_elevent_bits_by_pair_cd);
  // E1
  std::bitset<11> expected_elevent_bits_by_pair_e1("01001110111");
  auto actual_elevent_bits_by_pair_e1 = eleven_bits_from_pair(14, 1);
  EXPECT_EQ(expected_elevent_bits_by_pair_e1, actual_elevent_bits_by_pair_e1);
  // 23
  std::bitset<11> expected_elevent_bits_by_pair_23("00001011101");
  auto actual_elevent_bits_by_pair_23 = eleven_bits_from_pair(2, 3);
  EXPECT_EQ(expected_elevent_bits_by_pair_23, actual_elevent_bits_by_pair_23);

  std::vector<std::bitset<11>> expected_from_string = {
      std::bitset<11>("00111001101"),
      std::bitset<11>("01000101001"),
      std::bitset<11>("01001110111"),
      std::bitset<11>("00001011101"),
  };
  auto actual_from_string = from_string("ABCDE123", ALNUM_MODE);
  EXPECT_EQ(expected_from_string, actual_from_string);

  std::vector<bool> expected_flatten_bits = {
      false, false, true,  true,  true,  false,
      false, true,  true,  false, true,  // 00111001101
      false, true,  false, false, false, true,
      false, true,  false, false, true,  // 01000101001
      false, true,  false, false, true,  true,
      true,  false, true,  true,  true,  // 01001110111
      false, false, false, false, true,  false,
      true,  true,  true,  false, true  // 00001011101
  };
  auto actual_flattend_bits = flatten_bits(actual_from_string);
  EXPECT_EQ(expected_flatten_bits, actual_flattend_bits);
  EXPECT_EQ(expected_flatten_bits,
            convert_string_into_bits("ABCDE123", ALNUM_MODE));

  std::vector<bool> expected_terminating_bits = {
      false, false, true,  true,  true,  false,
      false, true,  true,  false, true,  // 00111001101
      false, true,  false, false, false, true,
      false, true,  false, false, true,  // 01000101001
      false, true,  false, false, true,  true,
      true,  false, true,  true,  true,  // 01001110111
      false, false, false, false, true,  false,
      true,  true,  true,  false, true,  // 00001011101
      false, false, false, false,        // 0000
  };
  auto actual_terminating_bits =
      append_terminating_bits(actual_flattend_bits, 4);
  EXPECT_EQ(expected_terminating_bits, actual_terminating_bits);

  std::vector<u_int8_t> expected_codewords = {
      0b00100000, 0b01000001, 0b11001101, 0b01000101, 0b00101001,
      0b11011100, 0b00101110, 0b10000000, 0b11101100,
  };
  auto actual_codewords =
      convert_to_codewords(actual_terminating_bits, ALNUM_MODE, H, 8);
  EXPECT_EQ(expected_codewords, actual_codewords);
  EXPECT_EQ(expected_codewords,
            convert_string_into_codewords("ABCDE123", ALNUM_MODE, H));
}
}  // namespace