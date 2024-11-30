#ifndef QR_H
#define QR_H

#include <bitset>
#include <map>
#include <string>
#include <tuple>
#include <vector>

// モード指示子
using ModeSpecifier = u_int8_t;
constexpr ModeSpecifier NUMBER_MODE = 0b0001;
constexpr ModeSpecifier ALNUM_MODE = 0b0010;
constexpr ModeSpecifier BYTE_MODE = 0b0100;
constexpr ModeSpecifier KANJI_MODE = 0b1000;

// 文字数指示子
extern const std::map<u_int8_t, u_int32_t> CHAR_LENGTH_SPECIFIER;
extern const std::map<char, u_int32_t> ALNUM_MODE_CHAR_MAPPING;

std::bitset<11> eleven_bits_from_pair(u_int32_t v1, u_int32_t v2);
std::bitset<6> eleven_bits_from_value(u_int32_t value);
std::vector<std::bitset<11>> from_string(const std::string& s,
                                         ModeSpecifier mode_specifier);
std::vector<bool> flatten_bits(const std::vector<std::bitset<11>>& bits);
std::vector<bool> convert_string_into_bits(const std::string& s,
                                           ModeSpecifier mode_specifier);
std::vector<bool> append_terminating_bits(const std::vector<bool>& bits,
                                          int length);

// エラー訂正レベル
using ErrorCorrectionLevel = u_int8_t;
constexpr ErrorCorrectionLevel L = 0b00;
constexpr ErrorCorrectionLevel M = 0b01;
constexpr ErrorCorrectionLevel Q = 0b10;
constexpr ErrorCorrectionLevel H = 0b11;

std::vector<u_int8_t> convert_to_codewords(
    const std::vector<bool>& bits, ModeSpecifier mode_specifier,
    ErrorCorrectionLevel correction_level, u_int32_t word_length);

std::vector<u_int8_t> convert_string_into_codewords(
    const std::string s, ModeSpecifier mode_specifier,
    ErrorCorrectionLevel correction_level);

class QrCode {
 public:
  QrCode(int size = 21, int version = 1, int mask_byte = 0b100,
         int mode_specifier = ALNUM_MODE, int error_correction_level = 0b00,
         bool autoInitialize = true);

  void initializeWithFinderPatterns();
  void addFinderPatterns(int x, int y);
  void setCell(int x, int y, bool value = true);
  std::string toString() const;
  void printCells() const;
  bool computeByMask(int x, int y, bool bit) const;
  void setModeCells();
  void setMaskingCells();
  int getCharSpecifierLength();
  bool isInRange(int x, int y) const;
  std::tuple<int, int, bool> writeZigZag(int x, int y,
                                         const std::vector<bool>& values,
                                         bool upward = true);
  void setErrorCorrectionCells();
  void setFormatCells();
  void setCharLengthCells(int qr_string_length);
  void createQrCode(std::string raw_string);

 private:
  int size;
  int version;
  int mask_byte;
  int mode_specifier;
  int error_correction_level;
  bool verify_size_and_version();
  std::vector<std::vector<bool>> matrix;
  std::vector<bool> create_char_length_bits(int x);
};

#endif  // QR_H
