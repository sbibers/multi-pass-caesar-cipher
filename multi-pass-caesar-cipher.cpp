#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <chrono>

std::string encrypt(const std::string &text)
{
    std::string shifted = text;

    // step 1: reverse the string.
    // input: "ahmad" -> after reverse: "damha"
    std::reverse(shifted.begin(), shifted.end());

    // step 2: first pass - forward shift based on position.
    // shifted = "damha"
    // i=0: 'd'(3) + key(5)  = 8  -> 'i'  | key = (0*7+5)%26 = 5 | enc = (3+5)%26=8 | shifted[i]=8+offset='i'
    // i=1: 'a'(0) + key(12) = 12 -> 'm'  | key = (1*7+5)%26 = 12 | enc = (0+12)%26=12 | shifted[i]=12+offset='m'
    // i=2: 'm'(12)+ key(19) = 5  -> 'f'  | key = (2*7+5)%26 = 19 | enc (12+19)%26=5 | shifted[i]=5+offset='f'
    // i=3: 'h'(7) + key(0)  = 7  -> 'h'  | key = (3*7+5)%26 = 0 | enc = (7+0)%26=7 | shifted[i]=7+offset='h'
    // i=4: 'a'(0) + key(7)  = 7  -> 'h'  | key = (4*7+5)%26 = 7 | enc = (0+7)%26=7 | shifted[i]=7+offset='h'
    // result: "imfhh"
    for (size_t i = 0; i < shifted.size(); i++)
    {
        char c = shifted[i];
        int offset = 0;
        if (isalpha(c) && isupper(c))
        {
            offset = 65;
        }
        else if(isalpha(c) && islower(c))
        {
            offset = 97;
        }
        else
        {
            continue;
        }
        int pos = c - offset;
        int key = (i * 7 + 5) % 26;
        int enc_pos = (pos + key) % 26;
        shifted[i] = enc_pos + offset;
    }

    // step 3: second pass - backward, using next character.
    // shifted = "imfhh"
    // i=4: 'h'(7)  + key(11) = 18 -> 's' | next=0 (no next), key=(4*5+0+11)%26=5, wait recalc: (20+0+11)%26=5, (7+5)%26=12->'m'
    // actually: key = (4*5 + 0 + 11) % 26 = 31 % 26 = 5, enc = (7+5)%26 = 12 -> 'm'
    // i=3: 'h'(7)  + key=? | next='m'(12), key=(3*5+12+11)%26=(15+12+11)%26=38%26=12, enc=(7+12)%26=19 -> 't'
    // i=2: 'f'(5)  + key=? | next='t'(19), key=(2*5+19+11)%26=(10+19+11)%26=40%26=14, enc=(5+14)%26=19 -> 't'
    // i=1: 'm'(12) + key=? | next='t'(19), key=(1*5+19+11)%26=(5+19+11)%26=35%26=9, enc=(12+9)%26=21 -> 'v'
    // i=0: 'i'(8)  + key=? | next='v'(21), key=(0*5+21+11)%26=32%26=6, enc=(8+6)%26=14 -> 'o'
    // Result: "ovttm"
    for (int i = shifted.size() - 1; i >= 0; i--)
    {
        char c = shifted[i];
        int offset = 0;
        if (isalpha(c) && isupper(c))
        {
            offset = 65;
        }
        else if (isalpha(c) && islower(c))
        {
            offset = 97;
        }
        else
        {
            continue;
        }
        int pos = c - offset;
        int next = (i < (int)shifted.size() - 1 && isalpha(shifted[i+1])) ? (shifted[i+1] % 26) : 0;
        int key = (i * 5 + next + 11) % 26;
        int enc_pos = (pos + key) % 26;
        shifted[i] = enc_pos + offset;
    }

    // step 4: Third pass - different formulas for odd/even positions.
    // shifted = "ovttm"
    // i=0 (even): 'o'(14) + key(3)  = 17 -> 'r' | key = (0*7+3)%26 = 3
    // i=1 (odd):  'v'(21) + key(20) = 15 -> 'p' | key = (1*11+9)%26 = 20
    // i=2 (even): 't'(19) + key(17) = 10 -> 'k' | key = (2*7+3)%26 = 17
    // i=3 (odd):  't'(19) + key(16) = 9  -> 'j' | key = (3*11+9)%26 = 42%26 = 16
    // i=4 (even): 'm'(12) + key(5)  = 17 -> 'r' | key = (4*7+3)%26 = 31%26 = 5
    // Result: "rpkjr"
    for (size_t i = 0; i < shifted.size(); i++)
    {
        char c = shifted[i];
        int offset = 0;
        if (isalpha(c) && isupper(c))
        {
            offset = 65;
        }
        else if (isalpha(c) && islower(c))
        {
            offset = 97;
        }
        else
        {
            continue;
        }
        int pos = c - offset;
        int key = (i % 2 == 0) ? (i * 7 + 3) % 26 : (i * 11 + 9) % 26;
        int enc_pos = (pos + key) % 26;
        shifted[i] = enc_pos + offset;
    }

    // step 5: Swap adjacent pairs.
    // shifted = "rpkjr"
    // Swap [0] and [1]: 'r' <-> 'p' -> "prkjr"
    // Swap [2] and [3]: 'k' <-> 'j' -> "prjkr"
    // [4] has no pair, stays as 'r'
    // Result: "prjkr"
    for (size_t i = 0; i + 1 < shifted.size(); i += 2)
    {
        std::swap(shifted[i], shifted[i + 1]);
    }

    return (shifted);
}

std::string decrypt(const std::string &text)
{
    std::string out = text;

    // input: "prjkr"
    
    // reverse step 5: swap back.
    // swap [0] and [1]: 'p' <-> 'r' -> "rpjkr"
    // swap [2] and [3]: 'j' <-> 'k' -> "rpkjr"
    // Result: "rpkjr"
    for (size_t i = 0; i + 1 < out.size(); i += 2)
    {
        std::swap(out[i], out[i + 1]);
    }
    
    // reverse step 4: undo third pass (odd/even positions).
    // out = "rpkjr"
    // i=0 (even): 'r'(17) - key(3)  = 14 -> 'o' | key = 3
    // i=1 (odd):  'p'(15) - key(20) = -5+26 = 21 -> 'v' | key = 20
    // i=2 (even): 'k'(10) - key(17) = -7+26 = 19 -> 't' | key = 17
    // i=3 (odd):  'j'(9)  - key(16) = -7+26 = 19 -> 't' | key = 16
    // i=4 (even): 'r'(17) - key(5)  = 12 -> 'm' | key = 5
    // Result: "ovttm"
    for (size_t i = 0; i < out.size(); i++)
    {
        char c = out[i];
        int offset = 0;
        if (isalpha(c) && isupper(c))
        {
            offset = 65;
        }
        else if (isalpha(c) && islower(c))
        {
            offset = 97;
        }
        else
        {
            continue;
        }
        int pos = c - offset;
        int key = (i % 2 == 0) ? (i * 7 + 3) % 26 : (i * 11 + 9) % 26;
        int dec_pos = (pos - key + 26) % 26;
        out[i] = dec_pos + offset;
    }

    // reverse step 3: undo second pass - go FORWARD.
    // out = "ovttm"
    // i=0: 'o'(14) - key(?) | next='v'(21), key=(0*5+21+11)%26=6, dec=(14-6+26)%26=8 -> 'i'
    // i=1: 'v'(21) - key(?) | next='t'(19), key=(5+19+11)%26=9, dec=(21-9+26)%26=12 -> 'm'
    // i=2: 't'(19) - key(?) | next='t'(19), key=(10+19+11)%26=14, dec=(19-14+26)%26=5 -> 'f'
    // i=3: 't'(19) - key(?) | next='m'(12), key=(15+12+11)%26=12, dec=(19-12+26)%26=7 -> 'h'
    // i=4: 'm'(12) - key(?) | next=0, key=(20+0+11)%26=5, dec=(12-5+26)%26=7 -> 'h'
    // result: "imfhh"
    for (size_t i = 0; i < out.size(); i++)
    {
        char c = out[i];
        int offset = 0;
        if (isalpha(c) && isupper(c))
        {
            offset = 65;
        }
        else if (isalpha(c) && islower(c))
        {
            offset = 97;
        }
        else
        {
            continue;
        }
        int pos = c - offset;
        int next = (i < out.size() - 1 && isalpha(out[i+1])) ? (out[i+1] % 26) : 0;
        int key = (i * 5 + next + 11) % 26;
        int dec_pos = (pos - key + 26) % 26;
        out[i] = dec_pos + offset;
    }

    // reverse step 2: undo first pass.
    // out = "imfhh"
    // i=0: 'i'(8)  - key(5)  = 3  -> 'd' | key = 5
    // i=1: 'm'(12) - key(12) = 0  -> 'a' | key = 12
    // i=2: 'f'(5)  - key(19) = -14+26 = 12 -> 'm' | key = 19
    // i=3: 'h'(7)  - key(0)  = 7  -> 'h' | key = 0
    // i=4: 'h'(7)  - key(7)  = 0  -> 'a' | key = 7
    // Result: "damha"
    for (size_t i = 0; i < out.size(); i++)
    {
        char c = out[i];
        int offset = 0;
        if (isalpha(c) && isupper(c))
        {
            offset = 65;
        }
        else if (isalpha(c) && islower(c))
        {
            offset = 97;
        }
        else
        {
            continue;
        }
        int pos = c - offset;
        int key = (i * 7 + 5) % 26;
        int dec_pos = (pos - key + 26) % 26;
        out[i] = dec_pos + offset;
    }

    // reverse step 1: undo the string reversal.
    // "damha" -> "ahmad"
    std::reverse(out.begin(), out.end());

    return (out);
}

int main()
{
    std::string text = "ahmad";
    std::cout << "Original: " << text << std::endl;

    std::string encrypted = encrypt(text);
    std::cout << "Encrypted: " << encrypted << std::endl;
    
    std::string decrypted = decrypt(encrypted);
    std::cout << "Decrypted: " << decrypted << std::endl;
    std::cout << "-------------------------------\n";
    std::string text2 = "Hello World";
    std::cout << "Original: " << text2 << std::endl;
    std::string encrypted2 = encrypt(text2);
    std::cout << "Encrypted: " << encrypted2 << std::endl;
    std::string decrypted2 = decrypt(encrypted2);
    std::cout << "Decrypted: " << decrypted2 << std::endl;
    
    return (0);
}
