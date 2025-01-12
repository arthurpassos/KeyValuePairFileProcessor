#include <impl/EscapeSequenceParser.h>

#include <string>
#include <stdexcept>
#include <util/ReadBuffer.h>

inline bool isControlASCII(char c)
{
    return static_cast<unsigned char>(c) <= 31;
}

namespace CityHash_v1_0_2 { struct uint128; }

namespace wide
{
    template <size_t Bits, typename Signed>
    class integer;
}

namespace impl
{
    /// Maps 0..15 to 0..9A..F or 0..9a..f correspondingly.
    constexpr inline std::string_view hex_digit_to_char_uppercase_table = "0123456789ABCDEF";
    constexpr inline std::string_view hex_digit_to_char_lowercase_table = "0123456789abcdef";

    /// Maps 0..255 to 00..FF or 00..ff correspondingly.
    constexpr inline std::string_view hex_byte_to_char_uppercase_table = //
            "000102030405060708090A0B0C0D0E0F"
            "101112131415161718191A1B1C1D1E1F"
            "202122232425262728292A2B2C2D2E2F"
            "303132333435363738393A3B3C3D3E3F"
            "404142434445464748494A4B4C4D4E4F"
            "505152535455565758595A5B5C5D5E5F"
            "606162636465666768696A6B6C6D6E6F"
            "707172737475767778797A7B7C7D7E7F"
            "808182838485868788898A8B8C8D8E8F"
            "909192939495969798999A9B9C9D9E9F"
            "A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
            "B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
            "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
            "D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
            "E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
            "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";

    constexpr inline std::string_view hex_byte_to_char_lowercase_table = //
            "000102030405060708090a0b0c0d0e0f"
            "101112131415161718191a1b1c1d1e1f"
            "202122232425262728292a2b2c2d2e2f"
            "303132333435363738393a3b3c3d3e3f"
            "404142434445464748494a4b4c4d4e4f"
            "505152535455565758595a5b5c5d5e5f"
            "606162636465666768696a6b6c6d6e6f"
            "707172737475767778797a7b7c7d7e7f"
            "808182838485868788898a8b8c8d8e8f"
            "909192939495969798999a9b9c9d9e9f"
            "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
            "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
            "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
            "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
            "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
            "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";

    /// Maps 0..255 to 00000000..11111111 correspondingly.
    constexpr inline std::string_view bin_byte_to_char_table = //
            "0000000000000001000000100000001100000100000001010000011000000111"
            "0000100000001001000010100000101100001100000011010000111000001111"
            "0001000000010001000100100001001100010100000101010001011000010111"
            "0001100000011001000110100001101100011100000111010001111000011111"
            "0010000000100001001000100010001100100100001001010010011000100111"
            "0010100000101001001010100010101100101100001011010010111000101111"
            "0011000000110001001100100011001100110100001101010011011000110111"
            "0011100000111001001110100011101100111100001111010011111000111111"
            "0100000001000001010000100100001101000100010001010100011001000111"
            "0100100001001001010010100100101101001100010011010100111001001111"
            "0101000001010001010100100101001101010100010101010101011001010111"
            "0101100001011001010110100101101101011100010111010101111001011111"
            "0110000001100001011000100110001101100100011001010110011001100111"
            "0110100001101001011010100110101101101100011011010110111001101111"
            "0111000001110001011100100111001101110100011101010111011001110111"
            "0111100001111001011110100111101101111100011111010111111001111111"
            "1000000010000001100000101000001110000100100001011000011010000111"
            "1000100010001001100010101000101110001100100011011000111010001111"
            "1001000010010001100100101001001110010100100101011001011010010111"
            "1001100010011001100110101001101110011100100111011001111010011111"
            "1010000010100001101000101010001110100100101001011010011010100111"
            "1010100010101001101010101010101110101100101011011010111010101111"
            "1011000010110001101100101011001110110100101101011011011010110111"
            "1011100010111001101110101011101110111100101111011011111010111111"
            "1100000011000001110000101100001111000100110001011100011011000111"
            "1100100011001001110010101100101111001100110011011100111011001111"
            "1101000011010001110100101101001111010100110101011101011011010111"
            "1101100011011001110110101101101111011100110111011101111011011111"
            "1110000011100001111000101110001111100100111001011110011011100111"
            "1110100011101001111010101110101111101100111011011110111011101111"
            "1111000011110001111100101111001111110100111101011111011011110111"
            "1111100011111001111110101111101111111100111111011111111011111111";

    /// Maps 0..9, A..F, a..f to 0..15. Other chars are mapped to implementation specific value.
    constexpr inline std::string_view hex_char_to_digit_table
            = {"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\xff\xff\xff\xff\xff\xff" //0-9
               "\xff\x0a\x0b\x0c\x0d\x0e\x0f\xff\xff\xff\xff\xff\xff\xff\xff\xff" //A-Z
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\x0a\x0b\x0c\x0d\x0e\x0f\xff\xff\xff\xff\xff\xff\xff\xff\xff" //a-z
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
               "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",
               256};

    /// Converts a hex digit '0'..'f' or '0'..'F' to its value 0..15.
    constexpr uint8_t unhexDigit(char c)
    {
        return hex_char_to_digit_table[static_cast<uint8_t>(c)];
    }

    /// Converts an unsigned integer in the native endian to hexadecimal representation and back. Used as a base class for HexConversion<T>.
    template <typename TUInt, typename = void>
    struct HexConversionUInt
    {
        static const constexpr size_t num_hex_digits = sizeof(TUInt) * 2;

        static void hex(TUInt uint_, char * out, std::string_view table)
        {
            union
            {
                TUInt value;
                uint8_t uint8[sizeof(TUInt)];
            };

            value = uint_;

            for (size_t i = 0; i < sizeof(TUInt); ++i)
            {
                if constexpr (std::endian::native == std::endian::little)
                    memcpy(out + i * 2, &table[static_cast<size_t>(uint8[sizeof(TUInt) - 1 - i]) * 2], 2);
                else
                    memcpy(out + i * 2, &table[static_cast<size_t>(uint8[i]) * 2], 2);
            }
        }

        static TUInt unhex(const char * data)
        {
            TUInt res;
            if constexpr (sizeof(TUInt) == 1)
            {
                res = static_cast<uint8_t>(unhexDigit(data[0])) * 0x10 + static_cast<uint8_t>(unhexDigit(data[1]));
            }
            else if constexpr (sizeof(TUInt) == 2)
            {
                res = static_cast<uint16_t>(unhexDigit(data[0])) * 0x1000 + static_cast<uint16_t>(unhexDigit(data[1])) * 0x100
                      + static_cast<uint16_t>(unhexDigit(data[2])) * 0x10 + static_cast<uint16_t>(unhexDigit(data[3]));
            }
            else if constexpr ((sizeof(TUInt) <= 8) || ((sizeof(TUInt) % 8) != 0))
            {
                res = 0;
                for (size_t i = 0; i < sizeof(TUInt) * 2; ++i, ++data)
                {
                    res <<= 4;
                    res += unhexDigit(*data);
                }
            }
            else
            {
                res = 0;
                for (size_t i = 0; i < sizeof(TUInt) / 8; ++i, data += 16)
                {
                    res <<= 64;
                    res += HexConversionUInt<uint64_t>::unhex(data);
                }
            }
            return res;
        }
    };

    /// Helper template class to convert a value of any supported type to hexadecimal representation and back.
    template <typename T, typename SFINAE = void>
    struct HexConversion;

    template <typename TUInt>
    struct HexConversion<TUInt, std::enable_if_t<std::is_integral_v<TUInt>>> : public HexConversionUInt<TUInt> {};

    template <size_t Bits, typename Signed>
    struct HexConversion<wide::integer<Bits, Signed>> : public HexConversionUInt<wide::integer<Bits, Signed>> {};

template <typename CityHashUInt128> /// Partial specialization here allows not to include <city.h> in this header.
struct HexConversion<CityHashUInt128, std::enable_if_t<std::is_same_v<CityHashUInt128, typename CityHash_v1_0_2::uint128>>>
{
    static const constexpr size_t num_hex_digits = 32;

    static void hex(const CityHashUInt128 & uint_, char * out, std::string_view table)
    {
        HexConversion<uint64_t>::hex(uint_.high64, out, table);
        HexConversion<uint64_t>::hex(uint_.low64, out + 16, table);
    }

    static CityHashUInt128 unhex(const char * data)
    {
        CityHashUInt128 res;
        res.high64 = HexConversion<uint64_t>::unhex(data);
        res.low64 = HexConversion<uint64_t>::unhex(data + 16);
        return res;
    }
};
}

/// Converts a hex representation with leading zeros back to an integer value.
/// The function supports native integer types, wide::integer, CityHash_v1_0_2::uint128.
template <typename T>
constexpr T unhexUInt(const char * data)
{
    return impl::HexConversion<T>::unhex(data);
}

/// Converts a hexadecimal digit '0'..'f' or '0'..'F' to uint8_t.
constexpr uint8_t unhex(char c)
{
    return impl::unhexDigit(c);
}

/// Converts two hexadecimal digits to uint8_t.
constexpr uint8_t unhex2(const char * data)
{
    return unhexUInt<uint8_t>(data);
}

inline char parseEscapeSequence(char c)
{
    switch (c)
    {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'e':
            return '\x1B';      /// \e escape sequence is non standard for C and C++ but supported by gcc and clang.
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '0':
            return '\0';
        default:
            return c;
    }
}

/** Parse the escape sequence, which can be simple (one character after backslash) or more complex (multiple characters).
  * It is assumed that the cursor is located on the `\` symbol
  */
template <typename Vector, typename ReturnType = void>
static ReturnType parseComplexEscapeSequence(Vector & s, ReadBuffer & buf)
{
    static constexpr bool throw_exception = std::is_same_v<ReturnType, void>;

    auto error = [](const char * message [[maybe_unused]])
    {
        if constexpr (throw_exception)
            throw std::runtime_error (message);
        return ReturnType(false);
    };

    ++buf.position();

    if (buf.eof())
    {
        return error("Cannot parse escape sequence");
    }

    char char_after_backslash = *buf.position();

    if (char_after_backslash == 'x')
    {
        ++buf.position();
        /// escape sequence of the form \xAA
        char hex_code[2];

        auto bytes_read = buf.read(hex_code, sizeof(hex_code));

        if (bytes_read != sizeof(hex_code))
        {
            return error("Cannot parse escape sequence");
        }

        s.push_back(unhex2(hex_code));
    }
    else if (char_after_backslash == 'N')
    {
        /// Support for NULLs: \N sequence must be parsed as empty string.
        ++buf.position();
    }
    else
    {
        /// The usual escape sequence of a single character.
        char decoded_char = parseEscapeSequence(char_after_backslash);

        /// For convenience using LIKE and regular expressions,
        /// we leave backslash when user write something like 'Hello 100\%':
        /// it is parsed like Hello 100\% instead of Hello 100%
        if (decoded_char != '\\'
            && decoded_char != '\''
            && decoded_char != '"'
            && decoded_char != '`'  /// MySQL style identifiers
            && decoded_char != '/'  /// JavaScript in HTML
            && decoded_char != '='  /// TSKV format invented somewhere
            && !isControlASCII(decoded_char))
        {
            s.push_back('\\');
        }

        s.push_back(decoded_char);
        ++buf.position();
    }

    return ReturnType(true);
}

bool parseComplexEscapeSequence(std::string & s, ReadBuffer & buf)
{
    return parseComplexEscapeSequence<std::string, bool>(s, buf);
}

bool EscapeSequenceParser::parseComplex(std::string & s, ReadBuffer &buf)
{
    return parseComplexEscapeSequence(s, buf);
}