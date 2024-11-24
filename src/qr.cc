#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

class QrCode {
 public:
  // Constructor with default size 21x21 and version 1
  QrCode(int size = 21, int version = 1, int mask_byte = 0b100,
         int mode_specifier = 0b0100, int error_correction_level = 0b00,
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
    std::cout << surrounding_string;
    for (int i = 0; i < size; i++) {
      std::cout << surrounding_string;
    }
    std::cout << surrounding_string << "\n";

    // Content with left and right borders
    for (const auto &row : matrix) {
      std::cout << surrounding_string;  // left border
      for (bool cell : row) {
        // std::cout << (cell ? surrounding_string : "  ");
        std::cout << (!cell ? "██" : "  ");  // "██" if true, otherwise "  "
      }
      std::cout << surrounding_string << "\n";  // right border
    }

    // Bottom border
    std::cout << surrounding_string;
    for (int i = 0; i < size; i++) {
      std::cout << surrounding_string;
    }
    std::cout << surrounding_string << "\n";
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
    if (mode_specifier == 0b0001) {  // number mode
      matrix[size - 2][size - 2] = true;
    } else if (mode_specifier == 0b0010) {  // alnum mode
      matrix[size - 2][size - 1] = true;
    } else if (mode_specifier == 0b0100) {  // 8-bit mode
      matrix[size - 1][size - 2] = true;
    } else if (mode_specifier == 0b1000) {  // kanji-mode
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

  /*
    Read n characters from (x, y) in a zigzag pattern.

    Example:
    Starting point (x, y) = (3, 3)
    values = (1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0)
    Write order:
      (3, 3) → (2, 3) → (3, 2) → (2, 2) → (3, 1) → (2, 1) → (3, 0) → (2, 0) →
      (1, 0) → (0, 0) → (1, 1)
    Matrix:
      [
        [0, 1, 0, 1],
        [x, 0, 1, 1],
        [x, x, 0, 0],
        [x, x, 0, 1]
      ]
    n = 11
  */
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

  /**
   * `qr_string_length`: how long a string in QR is
   */
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
};

int main(int argc, char *argv[]) {
  QrCode qr;
  if (argc != 2) {
    std::cerr << "the number of arguments must be 1 but got "
              << std::to_string(argc) << "\n";
    return 1;
  }
  std::cout << "Input: " << argv[1] << '\n';
  qr.createQrCode(argv[1]);
  qr.printCells();
  // for (auto i = 0; i < 21; i++) {
  //   for (auto j = 0; j < 21; j++) {
  //     std::cout << qr.computeByMask(i, j, true) << ' ';
  //   }
  //   std::cout << '\n';
  // }
}
