#pragma once

#include <string>
#include <vector>

namespace util {

    static const char kBase64Chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    inline int base64_char_value(char c) {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    }

    inline bool base64_decode(const std::string& encoded, std::vector<unsigned char>& output) {
        output.clear();

        if (encoded.empty()) {
            return true;
        }

        size_t padding = 0;
        if (encoded.size() >= 1 && encoded[encoded.size() - 1] == '=') padding++;
        if (encoded.size() >= 2 && encoded[encoded.size() - 2] == '=') padding++;

        size_t expectedSize = (encoded.size() / 4) * 3 - padding;
        output.reserve(expectedSize);

        int accum = 0;
        int bits = 0;

        for (char c : encoded) {
            if (c == '=') break;

            int val = base64_char_value(c);
            if (val < 0) {
                output.clear();
                return false;
            }

            accum = (accum << 6) | val;
            bits += 6;

            if (bits >= 8) {
                bits -= 8;
                output.push_back(static_cast<unsigned char>((accum >> bits) & 0xFF));
            }
        }

        return true;
    }

    inline std::string base64Encode(const std::vector<unsigned char>& data) {
        std::string result;
        result.reserve(((data.size() + 2) / 3) * 4);

        size_t i = 0;
        while (i < data.size()) {
            unsigned char b0 = data[i];
            unsigned char b1 = (i + 1 < data.size()) ? data[i + 1] : 0;
            unsigned char b2 = (i + 2 < data.size()) ? data[i + 2] : 0;

            result.push_back(kBase64Chars[b0 >> 2]);
            result.push_back(kBase64Chars[((b0 & 0x03) << 4) | (b1 >> 4)]);
            result.push_back(kBase64Chars[((b1 & 0x0F) << 2) | (b2 >> 6)]);
            result.push_back(kBase64Chars[b2 & 0x3F]);

            i += 3;
        }

        if (data.size() % 3 == 1) {
            result[result.size() - 1] = '=';
            result[result.size() - 2] = '=';
        }
        else if (data.size() % 3 == 2) {
            result[result.size() - 1] = '=';
        }

        return result;
    }

}  // namespace util