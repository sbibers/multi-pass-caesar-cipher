#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdint>
#include <utility>

// 7 column base values (Unicode start points)
const uint32_t COLUMN_BASES[7] = {256, 512, 768, 1024, 1280, 1536, 1792}; // even : capital, odd : small.

// Convert a Unicode codepoint to UTF-8 string
std::string codepoint_to_utf8(uint32_t cp) {
    std::string result;
    if (cp < 0x80) {
        result += static_cast<char>(cp);
    } else if (cp < 0x800) {
        result += static_cast<char>(0xC0 | (cp >> 6));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        result += static_cast<char>(0xE0 | (cp >> 12));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        result += static_cast<char>(0xF0 | (cp >> 18));
        result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    }
    return result;
}

// Decode a UTF-8 character and return its codepoint and byte length
std::pair<uint32_t, size_t> utf8_to_codepoint(const std::string &text, size_t pos) {
    unsigned char c = text[pos];
    uint32_t codepoint = 0;
    size_t len = 0;
    
    if ((c & 0x80) == 0) {
        codepoint = c;
        len = 1;
    } else if ((c & 0xE0) == 0xC0) {
        codepoint = (c & 0x1F) << 6;
        codepoint |= (text[pos + 1] & 0x3F);
        len = 2;
    } else if ((c & 0xF0) == 0xE0) {
        codepoint = (c & 0x0F) << 12;
        codepoint |= (text[pos + 1] & 0x3F) << 6;
        codepoint |= (text[pos + 2] & 0x3F);
        len = 3;
    } else if ((c & 0xF8) == 0xF0) {
        codepoint = (c & 0x07) << 18;
        codepoint |= (text[pos + 1] & 0x3F) << 12;
        codepoint |= (text[pos + 2] & 0x3F) << 6;
        codepoint |= (text[pos + 3] & 0x3F);
        len = 4;
    }
    return {codepoint, len};
}

std::string encrypt(const std::string &text) {
    std::string shifted = text;

    // Step 1: Apply shift cipher
    for (size_t i = 0; i < shifted.size(); i++) {
        char c = shifted[i];
        int offset = 0;
        if(isalpha(c) && isupper(c)) {
            offset = 65;
        } else if(isalpha(c) && islower(c)) {
            offset = 97;
        } else {
            continue; // Non-alphabetic characters are not changed
        }
        int pos = c - offset;                 // 0..25
        int key = (i * 7 + 3) % 26;       // depends ONLY on index
        int enc_pos = (pos + key) % 26;
        shifted[i] = enc_pos + offset;
    }
    std::cout << "Shifted: " << shifted << std::endl;
    // Step 2: Convert to Unicode (7 columns, evens=upper, odds=lower)
    std::string out;
    size_t char_index = 0;  // Index for alphabetic characters only

    for (size_t i = 0; i < shifted.size(); i++) {
        char c = shifted[i];
        
        if (isalpha(c)) {
            int pos = 0;  // 0..25
            bool is_upper = isupper(c);
            
            if (is_upper) {
                pos = c - 'A';
            } else {
                pos = c - 'a';
            }
            
            // Select column based on character index (0-6)
            int column = char_index % 7;
            uint32_t base = COLUMN_BASES[column];
            
            // Uppercase: even offsets (0, 2, 4, ..., 50)
            // Lowercase: odd offsets (1, 3, 5, ..., 51)
            uint32_t unicode_offset;
            if (is_upper) {
                unicode_offset = pos * 2;       // 0, 2, 4, ... 50
            } else {
                unicode_offset = pos * 2 + 1;   // 1, 3, 5, ... 51
            }
            
            uint32_t codepoint = base + unicode_offset;
            out += codepoint_to_utf8(codepoint);
            char_index++;
        } else {
            // Non-alphabetic characters are not changed
            out += c;
        }
    }
    return out;
}

std::string decrypt(const std::string &text) {
    // Step 1: Convert Unicode back to ASCII
    std::string shifted;
    size_t char_index = 0;

    for (size_t i = 0; i < text.size(); ) {
        auto [codepoint, len] = utf8_to_codepoint(text, i);
        
        // Check if this codepoint belongs to one of our 7 columns
        bool found = false;
        int column = char_index % 7;
        uint32_t base = COLUMN_BASES[column];
        
        // Each column uses 52 codepoints (0-51)
        if (codepoint >= base && codepoint < base + 52) {
            uint32_t offset = codepoint - base;
            bool is_upper = (offset % 2 == 0);  // even = uppercase
            int pos = offset / 2;  // 0..25
            
            if (is_upper) {
                shifted += static_cast<char>('A' + pos);
            } else {
                shifted += static_cast<char>('a' + pos);
            }
            char_index++;
            found = true;
        }
        
        if (!found) {
            // Not an encoded character, copy as-is (handles spaces, etc.)
            shifted += text.substr(i, len);
        }
        
        i += len;
    }

    // Step 2: Reverse shift cipher
    std::string out = shifted;
    for (size_t i = 0; i < out.size(); i++) {
        char c = out[i];
        int offset = 0;
        if(isalpha(c) && isupper(c)) {
            offset = 65;
        } else if(isalpha(c) && islower(c)) {
            offset = 97;
        } else {
            continue; // Non-alphabetic characters are not changed
        }
        int pos = c - offset;
        int key = (i * 7 + 3) % 26;
        int dec_pos = (pos - key + 26) % 26;
        out[i] = dec_pos + offset;
    }
    return out;
}

int main()
{
    std::string text = "SAAlam salam";
    std::cout << "Original: " << text << std::endl;

    std::string encrypted = encrypt(text);
    std::cout << "Encrypted: " << encrypted << std::endl;
    
    // Print Unicode codepoints of encrypted string
    for (size_t i = 0; i < encrypted.size(); ) {
        auto [codepoint, len] = utf8_to_codepoint(encrypted, i);
        std::cout << "Char: " << encrypted.substr(i, len) << " -> Codepoint: " << codepoint << std::endl;
        i += len;
    }
    
    std::string decrypted = decrypt(encrypted);
    std::cout << decrypted.at(0) << std::endl;
    std::cout << "Decrypted: " << decrypted << std::endl;
    return 0;
}
