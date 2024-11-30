
#include "qr.h"

#include <algorithm>
#include <bitset>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// 文字数指示子
const std::map<u_int8_t, u_int32_t> CHAR_LENGTH_SPECIFIER = {
    {NUMBER_MODE, 10},
    {ALNUM_MODE, 9},
    {BYTE_MODE, 8},
    {KANJI_MODE, 8},
};

// 英数字モードの各文字に対応する値
const std::map<char, u_int32_t> ALNUM_MODE_CHAR_MAPPING = {
    {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},  {'6', 6},
    {'7', 7},  {'8', 8},  {'9', 9},  {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}, {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19}, {'K', 20},
    {'L', 21}, {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27},
    {'S', 28}, {'T', 29}, {'U', 30}, {'V', 31}, {'W', 32}, {'X', 33}, {'Y', 34},
    {'Z', 35}, {' ', 36}, {'$', 37}, {'%', 38}, {'*', 39}, {'+', 40}, {'-', 41},
    {'.', 42}, {'/', 43}, {':', 44},
};

// "AB" -> 45 * 10 + 11 = 461
// 461 -> 0b00111001101
std::bitset<11> eleven_bits_from_pair(u_int32_t v1, u_int32_t v2) {
  std::vector<bool> result;
  std::bitset<11> bits(v1 * 45u + v2);
  return bits;
}

std::bitset<6> eleven_bits_from_value(u_int32_t value) {
  std::bitset<6> bits(value);
  return bits;
}

std::vector<std::bitset<11>> from_string(const std::string& s,
                                         u_int8_t mode_specifier = ALNUM_MODE) {
  if (mode_specifier == ALNUM_MODE) {
    std::vector<std::bitset<11>> result;
    // read two characters at a time
    // if the length of the string is odd, add a # at the end
    std::string s_ = s;
    if (s_.size() % 2 == 1) {
      s_.push_back('#');
    }
    for (size_t i = 0; i < s_.size(); i += 2) {
      // std::cout << s_[i] << ' ' << s_[i + 1] << '\n';
      char c1 = s_[i];
      char c2 = s_[i + 1];
      if (c2 == '#') {
        if (ALNUM_MODE_CHAR_MAPPING.find(c1) == ALNUM_MODE_CHAR_MAPPING.end()) {
          throw std::invalid_argument("Invalid character in the input string");
        }
        throw std::logic_error("Not implemented yet");
      } else {
        if (ALNUM_MODE_CHAR_MAPPING.find(c1) == ALNUM_MODE_CHAR_MAPPING.end() ||
            ALNUM_MODE_CHAR_MAPPING.find(c2) == ALNUM_MODE_CHAR_MAPPING.end()) {
          throw std::invalid_argument("Invalid character in the input string");
        }
        u_int32_t v1 = ALNUM_MODE_CHAR_MAPPING.at(c1);
        u_int32_t v2 = ALNUM_MODE_CHAR_MAPPING.at(c2);
        result.push_back(eleven_bits_from_pair(v1, v2));
      }
    }
    return result;
  } else {
    throw std::logic_error("Not implemented yet");
  }
}

std::vector<bool> flatten_bits(const std::vector<std::bitset<11>>& bitsets) {
  std::vector<bool> result;
  for (const auto& bits : bitsets) {
    for (auto i = 10; i >= 0; i--) {
      result.push_back(bits[i]);
    }
  }
  return result;
}

std::vector<bool> convert_string_into_bits(const std::string& s,
                                           u_int8_t mode_specifier) {
  std::vector<std::bitset<11>> bits = from_string(s, mode_specifier);
  return flatten_bits(bits);
}

std::vector<bool> append_terminating_bits(const std::vector<bool>& bits,
                                          int length = 4) {
  std::vector<bool> result = bits;
  for (int i = 0; i < length; i++) {
    result.push_back(false);
  }
  return result;
}

const std::map<ErrorCorrectionLevel, u_int32_t> COUNT_OF_CODE_WORDS = {
    {L, 19},
    {M, 16},
    {Q, 13},
    {H, 9},
};

std::vector<bool> convert_mode_specifier_into_vector_bool(
    const u_int8_t mode_specifier, int32_t word_length) {
  std::vector<bool> result;
  // mode_spcifier is 4bits
  for (int i = 0; i < 4; i++) {
    result.push_back(mode_specifier & (1 << (3 - i)));
  }
  auto char_length_specifier = CHAR_LENGTH_SPECIFIER.at(mode_specifier);
  u_int32_t bits = 0;
  for (int i = 0; i < char_length_specifier; i++) {
    u_int32_t value = 1 << (8 - i);
    if (value <= word_length) {
      result.push_back(true);
      word_length -= value;
    } else {
      result.push_back(false);
    }
  }

  return result;
}

std::vector<u_int8_t> convert_to_codewords(
    const std::vector<bool>& bits_, ModeSpecifier mode_specifier,
    ErrorCorrectionLevel correction_level, u_int32_t word_length) {
  std::vector<u_int8_t> result;

  std::vector<bool> bits =
      convert_mode_specifier_into_vector_bool(mode_specifier, word_length);
  for (auto bit : bits_) {
    bits.push_back(bit);
  }
  // bitsを8bitごとに区切ります。
  // 最後のビット列が８未満の場合は0で埋めます
  // それをu_int8_tに変換してcodewordsに挿入します
  for (int i = 0; i < bits.size() % 8; i++) {
    bits.push_back(false);
  }
  std::cout << bits_.size() % 8 << "\n";
  std::cout << bits_.size() << "\n";
  std::cout << bits.size() << "\n";
  for (int i = 0; i < bits.size() / 8; i++) {
    u_int8_t byte = 0;
    for (int j = 0; j < 8; j++) {
      byte = (byte << 1) | bits[i * 8 + j];
    }
    result.push_back(byte);
  }

  // Add padding result if necessary
  u_int8_t padding_codewords[] = {0b11101100, 0b00010001};
  size_t padding_index = 0;
  u_int32_t symbol_data_codewords = COUNT_OF_CODE_WORDS.at(correction_level);
  while (result.size() < symbol_data_codewords) {
    result.push_back(padding_codewords[padding_index]);
    padding_index = (padding_index + 1) % 2;
  }

  return result;
}

std::vector<u_int8_t> convert_string_into_codewords(
    const std::string s, ModeSpecifier mode_specifier,
    ErrorCorrectionLevel correction_level) {
  auto word_length = s.size();
  auto bits = convert_string_into_bits(s, mode_specifier);
  return convert_to_codewords(bits, mode_specifier, correction_level,
                              word_length);
}

QrCode::QrCode(int size, int version, int mask_byte, int mode_specifier,
               int error_correction_level, bool autoInitialize)
    : size(size),
      version(version),
      mask_byte(mask_byte),
      mode_specifier(mode_specifier),
      error_correction_level(error_correction_level),
      matrix(size, std::vector<bool>(size, false)) {
  if (!verify_size_and_version()) {
    std::string error_message =
        "size must be 21 but got " + std::to_string(size) +
        "; version must be 1 but got " + std::to_string(version);
    throw std::invalid_argument(error_message);
  }
  if (autoInitialize) {
    initializeWithFinderPatterns();
  }
}

void QrCode::initializeWithFinderPatterns() {
  addFinderPatterns(0, 0);         // Upper left
  addFinderPatterns(size - 7, 0);  // Lower left
  addFinderPatterns(0, size - 7);  // Upper right

  for (int i = 8; i < size - 8; i++) {
    setCell(6, i, i % 2 == 0);  // Horizontal timing pattern
    setCell(i, 6, i % 2 == 0);  // Vertical timing pattern
  }

  setCell(13, 8);  // timing pattern
  setModeCells();
  setFormatCells();
}

void QrCode::addFinderPatterns(int x, int y) {
  for (int row = 0; row < 7; row++) {
    setCell(x + row, y);
    setCell(x + row, y + 6);
  }
  for (int col = 0; col < 7; col++) {
    setCell(x, y + col);
    setCell(x + 6, y + col);
  }

  for (int row = 2; row < 5; row++) {
    for (int col = 2; col < 5; col++) {
      setCell(x + row, y + col);
    }
  }
}

void QrCode::setCell(int x, int y, bool value) {
  if (x >= 0 && x < size && y >= 0 && y < size) {
    matrix[x][y] = value;
  }
}

std::string QrCode::toString() const {
  std::ostringstream oss;
  for (const auto& row : matrix) {
    for (bool cell : row) {
      oss << (cell ? "██" : "  ");
    }
    oss << '\n';
  }
  return oss.str();
}

void QrCode::printCells() const {
  std::string surrounding_string = "██";
  for (int i = 0; i < 2; i++) {
    std::cout << surrounding_string << surrounding_string;
    for (int j = 0; j < size; j++) {
      std::cout << surrounding_string;
    }
    std::cout << surrounding_string << surrounding_string << "\n";
  }

  for (const auto& row : matrix) {
    std::cout << surrounding_string << surrounding_string;
    for (bool cell : row) {
      std::cout << (!cell ? "██" : "  ");
    }
    std::cout << surrounding_string << surrounding_string << "\n";
  }

  for (int i = 0; i < 2; i++) {
    std::cout << surrounding_string << surrounding_string;
    for (int j = 0; j < size; j++) {
      std::cout << surrounding_string;
    }
    std::cout << surrounding_string << surrounding_string << "\n";
  }
}

bool QrCode::computeByMask(int x, int y, bool bit) const {
  int mask_of_mask = 0b101;
  if (mask_byte == (0b000 ^ mask_of_mask)) {
    return (x + y) % 2 == 0 ? !bit : bit;
  } else if (mask_byte == (0b001 ^ mask_of_mask)) {
    return x % 2 == 0 ? !bit : bit;
  } else if (mask_byte == (0b010 ^ mask_of_mask)) {
    return y % 3 == 0 ? !bit : bit;
  } else if (mask_byte == (0b011 ^ mask_of_mask)) {
    return (x + y) % 3 == 0 ? !bit : bit;
  } else if (mask_byte == (0b100 ^ mask_of_mask)) {
    return (x / 2 + y / 3) % 2 == 0 ? !bit : bit;
  } else if (mask_byte == (0b101 ^ mask_of_mask)) {
    return (x * y) % 2 + (x * y) % 3 == 0 ? !bit : bit;
  } else if (mask_byte == (0b110 ^ mask_of_mask)) {
    return (((x * y) % 3 + x * y) % 2 == 0) ? !bit : bit;
  } else if (mask_byte == (0b111 ^ mask_of_mask)) {
    return ((x * y) % 3 + x + y) % 2 == 0 ? !bit : bit;
  } else {
    throw std::logic_error("This mask is invalid (" +
                           std::to_string(mask_byte) + ")");
  }
}

void QrCode::setModeCells() {
  if (mode_specifier == NUMBER_MODE) {
    matrix[size - 2][size - 2] = true;
  } else if (mode_specifier == ALNUM_MODE) {
    matrix[size - 2][size - 2] = false;
  } else {
    matrix[size - 2][size - 2] = false;
  }
}

void QrCode::setMaskingCells() {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (!isInRange(i, j)) {
        setCell(i, j, false);
      }
    }
  }
}

int QrCode::getCharSpecifierLength() {
  return CHAR_LENGTH_SPECIFIER.at(mode_specifier);
}

bool QrCode::isInRange(int x, int y) const {
  return x < size && y < size && x >= 0 && y >= 0;
}

std::tuple<int, int, bool> QrCode::writeZigZag(int x, int y,
                                               const std::vector<bool>& values,
                                               bool upward) {
  int row = x;
  int col = y;
  for (size_t i = 0; i < values.size(); ++i) {
    setCell(row, col, values[i]);
    if (upward) {
      if (col == 0 || row == size - 1) {
        row++;
        upward = false;
      } else {
        row--;
        col--;
      }
    } else {
      if (row == 0 || col == size - 1) {
        row++;
        upward = true;
      } else {
        row++;
        col--;
      }
    }
  }
  return std::make_tuple(row, col, upward);
}

void QrCode::setErrorCorrectionCells() {
  error_correction_level ^= 0b10;  // 0b10 is in specification
  if (error_correction_level & 0b10) {
    matrix[8][0] = true;
  }
  if (error_correction_level & 0b01) {
    matrix[8][1] = true;
  }
}

void QrCode::setFormatCells() {
  // masking format
  setMaskingCells();
  setErrorCorrectionCells();

  // TODO: remove hardcode and implement according to the spec
  // upper-left
  // below square
  setCell(8, 7);
  // right to square
  setCell(0, 8);
  setCell(2, 8);
  setCell(5, 8);

  // upper-right
  setCell(8, size - 1);
  setCell(8, size - 3);
  setCell(8, size - 6);

  // lower-left
  setCell(size - 1, 8);
  setCell(size - 3, 8);
  setCell(size - 7, 8);
}

void QrCode::setCharLengthCells(int qr_string_length) {
  int offset_x = 3;
  int offset_y = 1;
  std::vector<bool> bits_to_write = create_char_length_bits(qr_string_length);
  // for (const auto &b : bits_to_write) {
  //   std::cerr << b << ' ';
  // }
  // std::cerr << '\n';
  // std::cerr << "size:" << bits_to_write.size() << '\n';
  // std::cerr << offset_x << '\n';
  // std::cerr << offset_y << '\n';
  writeZigZag(size - offset_x, size - offset_y, bits_to_write);
}

void QrCode::createQrCode(std::string raw_string) {
  setCharLengthCells(static_cast<int>(raw_string.size()));
  int start_offset = getCharSpecifierLength() +
                     4;  // char length specifier(8) + mode specifier(4)
  int start_x = size - 1 - start_offset / 2;
  int start_y = size - 1 - start_offset % 2;
  bool start_upward = true;
  // std::cerr << start_x << ' ' << start_y << '\n';
  auto bits_to_write = flatten_bits(from_string(raw_string, mode_specifier));
  for (const auto& one_character_byte : bits_to_write) {
    auto [x, y, upward] = writeZigZag(
        start_x, start_y, std::vector<bool>{one_character_byte}, start_upward);
    start_x = x;
    start_y = y;
    start_upward = upward;
  }
  // TODO: implement error correction part
}

bool QrCode::verify_size_and_version() {
  return (size == 21) && (version == 1);
}

std::vector<bool> QrCode::create_char_length_bits(int x) {
  std::vector<bool> result;
  while (0 < x) {
    result.push_back((x & 0b1) == 1);
    x >>= 1;
  }
  while (result.size() < 8) {
    result.push_back(false);
  }
  std::reverse(result.begin(), result.end());
  return result;
}

/*
class QrCode {
 public:
  // Constructor with default size 21x21 and version 1
  QrCode(int size = 21, int version = 1, int mask_byte = 0b100,
         int mode_specifier = ALNUM_MODE, int error_correction_level = 0b00,
         bool autoInitialize = true)
      : size(size),
        version(version),
        mask_byte(mask_byte),
        mode_specifier(mode_specifier),
        error_correction_level(error_correction_level),
        matrix(size, std::vector<bool>(size, false)) {
    if (!verify_size_and_version()) {
      std::string error_message = "size must be 21 but got " +
                                  std::to_string(size) +
                                  "; "
                                  "version must be 1 but got " +
                                  std::to_string(version);
      throw std::invalid_argument(error_message);
    }
    if (autoInitialize) {
      initializeWithFinderPatterns();
    }
  }

  void initializeWithFinderPatterns() {
    // Fill in finder patterns at three corners
    addFinderPatterns(0, 0);         // Upper left
    addFinderPatterns(size - 7, 0);  // Lower left
    addFinderPatterns(0, size - 7);  // Upper right

    // Fill timing patterns
    for (int i = 8; i < size - 8; i++) {
      setCell(6, i, i % 2 == 0);  // Horizontal timing pattern
      setCell(i, 6, i % 2 == 0);  // Vertical timing pattern
    }

    // others
    setCell(13, 8);    // timing pattern
    setModeCells();    // set mode (bottom right)
    setFormatCells();  // set according to format specifications
  }

  void addFinderPatterns(int x, int y) {
    // Outer square (7x7)
    for (int row = 0; row < 7; row++) {
      setCell(x + row, y);
      setCell(x + row, y + 6);
    }
    for (int col = 0; col < 7; col++) {
      setCell(x, y + col);
      setCell(x + 6, y + col);
    }

    // Inner square (3x3, center filled)
    for (int row = 2; row < 5; row++) {
      for (int col = 2; col < 5; col++) {
        setCell(x + row, y + col);
      }
    }
  }

  void setCell(int x, int y, bool value = true) {
    if (x >= 0 && x < size && y >= 0 && y < size) {
      matrix[x][y] = value;
    }
  }

  std::string toString() const {
    std::ostringstream oss;
    for (const auto &row : matrix) {
      for (bool cell : row) {
        oss << (cell ? "██" : "  ");  // "██" if true, otherwise "  "
      }
      oss << '\n';
    }
    return oss.str();
  }

  void printCells() const {
    std::string surrounding_string = "██";
    // std::string surrounding_string = "▓▓";
    // Top border
    for (int i = 0; i < 2; i++) {
      std::cout << surrounding_string;
      std::cout << surrounding_string;
      for (int j = 0; j < size; j++) {
        std::cout << surrounding_string;
      }
      std::cout << surrounding_string;
      std::cout << surrounding_string << "\n";
    }

    // Content with left and right borders
    for (const auto &row : matrix) {
      std::cout << surrounding_string;  // left border
      std::cout << surrounding_string;
      for (bool cell : row) {
        // std::cout << (cell ? surrounding_string : "  ");
        std::cout << (!cell ? "██" : "  ");  // "██" if true, otherwise "  "
      }
      std::cout << surrounding_string;
      std::cout << surrounding_string << "\n";  // right border
    }

    // Bottom border
    for (int i = 0; i < 2; i++) {
      std::cout << surrounding_string;
      std::cout << surrounding_string;
      for (int j = 0; j < size; j++) {
        std::cout << surrounding_string;
      }
      std::cout << surrounding_string;
      std::cout << surrounding_string << "\n";
    }
  }

  bool computeByMask(int x, int y, bool bit) const {
    int mask_of_mask = 0b101;
    if (mask_byte == (0b000 ^ mask_of_mask)) {
      return (x + y) % 2 == 0 ? !bit : bit;
    } else if (mask_byte == (0b001 ^ mask_of_mask)) {
      return x % 2 == 0 ? !bit : bit;
    } else if (mask_byte == (0b010 ^ mask_of_mask)) {
      return y % 3 == 0 ? !bit : bit;
    } else if (mask_byte == (0b011 ^ mask_of_mask)) {
      return (x + y) % 3 == 0 ? !bit : bit;
    } else if (mask_byte == (0b100 ^ mask_of_mask)) {
      return (x / 2 + y / 3) % 2 == 0 ? !bit : bit;
    } else if (mask_byte == (0b101 ^ mask_of_mask)) {
      return (x * y) % 2 + (x * y) % 3 == 0 ? !bit : bit;
    } else if (mask_byte == (0b110 ^ mask_of_mask)) {
      return (((x * y) % 3 + x * y) % 2 == 0) ? !bit : bit;
    } else if (mask_byte == (0b111 ^ mask_of_mask)) {
      return ((x * y) % 3 + x + y) % 2 == 0 ? !bit : bit;
    } else {
      throw std::logic_error("This mask is invalid (" +
                             std::to_string(mask_byte) + ")");
    }
  }

  void setModeCells() {
    if (mode_specifier == NUMBER_MODE) {
      matrix[size - 2][size - 2] = true;
    } else if (mode_specifier == ALNUM_MODE) {
      matrix[size - 2][size - 1] = true;
    } else if (mode_specifier == BYTE_MODE) {
      matrix[size - 1][size - 2] = true;
    } else if (mode_specifier == KANJI_MODE) {
      matrix[size - 1][size - 1] = true;
    }
    for (int i = 1; i <= 2; i++) {
      for (int j = 1; j <= 2; j++) {
        matrix[size - i][size - j] =
            !computeByMask(i, j, matrix[size - i][size - j]);
        // std::cerr << "(i, j): " << i << ' ' << j << '='
        //           << matrix[size - i][size - j] << '\n';
      }
    }
  }

  void setMaskingCells() {
    if (mask_byte & 0b100) {
      setCell(8, 2);
    }
    if (mask_byte & 0b010) {
      setCell(8, 3);
    }
    if (mask_byte & 0b001) {
      setCell(8, 4);
    }
    if (mask_byte > 0b111) {
      throw std::logic_error("This mask is invalid (" +
                             std::to_string(mask_byte) + ")");
    }
  }

  int getCharSpecifierLength() {
    if (1 <= version && version <= 9) {
      if (mode_specifier == 0b0001) {  //
        return 10;
      } else if (mode_specifier == 0b0010) {
        return 9;
      } else if (mode_specifier == 0b0100 || mode_specifier == 0b1000) {
        return 8;
      }
    }
    throw std::logic_error("not implemented");
  }

  bool isInRange(int x, int y) const {
    // Check if coordinates are within the matrix bounds
    if (x < 0 || y < 0 || size <= x || size <= y) {
      return false;
    }
    // Upper left 9x9
    if (x < 9 && y < 9) {
      return false;
    }
    // Lower left 9x9
    if (size - 9 <= x && y < 9) {
      return false;
    }
    // Upper right 9x9
    if (x < 9 && size - 9 <= y) {
      return false;
    }
    // center part
    if (x == 8 || y == 8) {
      return false;
    }
    // TODO: add more other-version specs
    return true;
  }

    // Read n characters from (x, y) in a zigzag pattern.

    // Example:
    // Starting point (x, y) = (3, 3)
    // values = (1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0)
    // Write order:
    //   (3, 3) → (2, 3) → (3, 2) → (2, 2) → (3, 1) → (2, 1) → (3, 0) → (2, 0) →
    //   (1, 0) → (0, 0) → (1, 1)
    // Matrix:
    //   [
    //     [0, 1, 0, 1],
    //     [x, 0, 1, 1],
    //     [x, x, 0, 0],
    //     [x, x, 0, 1]
    //   ]
    // n = 11
std::tuple<int, int, bool> writeZigZag(int x, int y,
                                       const std::vector<bool> &values,
                                       bool upward = true) {
  size_t index = 0;  // To track the position in the input vector

  // Check if the values vector is non-empty
  if (values.empty()) {
    throw std::invalid_argument(
        "The input vector must contain at least one element.");
  }

  // Iterate through the values and write them in the zigzag pattern
  // std::cerr << "start\n";
  while (index < values.size()) {
    // std::cerr << "index: " << index << ' ';
    // std::cerr << "(x,y): " << x << ' ' << y << '\n';
    matrix[x][y] = computeByMask(x, y, values[index++]);

    // Determine the next cell in the zigzag pattern
    if (upward) {
      if (y % 2 == 0) {  // go left
        y--;
      } else {  // go upper-right
        x--;
        y++;
      }
      if (!isInRange(x, y)) {
        x++;
        y -= 2;
        upward = !upward;
      }
    } else {
      if (y % 2 == 0) {  // go left
        y--;
      } else {  // go lower-right
        x++;
        y++;
      }
      if (!isInRange(x, y)) {
        x--;
        y -= 2;
        upward = !upward;
      }
    }
  }

  // Check if not all values could be written
  if (index < values.size()) {
    throw std::runtime_error(
        "Not all values could be written within the matrix.");
  }
  return std::make_tuple(x, y, upward);
}

void setErrorCorrectionCells() {
  error_correction_level ^= 0b10;  // 0b10 is in specification
  if (error_correction_level & 0b10) {
    matrix[8][0] = true;
  }
  if (error_correction_level & 0b01) {
    matrix[8][1] = true;
  }
}

void setFormatCells() {
  // masking format
  setMaskingCells();
  setErrorCorrectionCells();

  // TODO: remove hardcode and implement according to the spec
  // upper-left
  // below square
  setCell(8, 7);
  // right to square
  setCell(0, 8);
  setCell(2, 8);
  setCell(5, 8);

  // upper-right
  setCell(8, size - 1);
  setCell(8, size - 3);
  setCell(8, size - 6);

  // lower-left
  setCell(size - 1, 8);
  setCell(size - 3, 8);
  setCell(size - 7, 8);
}

  // `qr_string_length`: how long a string in QR is
void setCharLengthCells(int qr_string_length) {
  int offset_x = 3;
  int offset_y = 1;
  std::vector<bool> bits_to_write = create_char_length_bits(qr_string_length);
  // for (const auto &b : bits_to_write) {
  //   std::cerr << b << ' ';
  // }
  // std::cerr << '\n';
  // std::cerr << "size:" << bits_to_write.size() << '\n';
  // std::cerr << offset_x << '\n';
  // std::cerr << offset_y << '\n';
  writeZigZag(size - offset_x, size - offset_y, bits_to_write);
}

void createQrCode(std::string raw_string) {
  setCharLengthCells(static_cast<int>(raw_string.size()));
  int start_offset = getCharSpecifierLength() +
                     4;  // char length specifier(8) + mode specifier(4)
  int start_x = size - 1 - start_offset / 2;
  int start_y = size - 1 - start_offset % 2;
  bool start_upward = true;
  // std::cerr << start_x << ' ' << start_y << '\n';
  auto bits_to_write = from_string(raw_string);
  for (const auto &one_character_byte : bits_to_write) {
    auto [x, y, upward] =
        writeZigZag(start_x, start_y, one_character_byte, start_upward);
    start_x = x;
    start_y = y;
    start_upward = upward;
  }
  // TODO: implement error correction part
}

private:
int size;
int version;
int mask_byte;
int mode_specifier;
int error_correction_level;
std::vector<std::vector<bool>> matrix;

bool verify_size_and_version() { return (size == 21) && (version == 1); }

std::vector<bool> create_char_length_bits(int x) {
  std::vector<bool> result;
  while (0 < x) {
    result.push_back((x & 0b1) == 1);
    x >>= 1;
  }
  while (result.size() < 8) {
    result.push_back(false);
  }
  std::reverse(result.begin(), result.end());
  return result;
}

std::vector<std::vector<bool>> from_string(const std::string &s) {
  std::vector<std::vector<bool>> result;
  for (const auto &c : s) {
    // Check if the character is within the ASCII range
    if (static_cast<unsigned char>(c) > 127) {
      throw std::runtime_error("Input must be ASCII: " + s);
    }
    // Convert the character to its binary representation
    std::vector<bool> bits(8);
    for (int i = 7; i >= 0; --i) {
      bits[i] = (c >> (7 - i)) & 1;
    }
    result.push_back(bits);
  }
  return result;
}
}
;
*/