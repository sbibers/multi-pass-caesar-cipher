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
    // "ahmad" -> "damha"
    std::reverse(shifted.begin(), shifted.end());

    std::cout << "step 1: " << shifted << std::endl;
    // step 2: first pass - forward shift based on position.
    // key = (i * 7 + 5) % 26
    // "damha" -> "imfhh"
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
    std::cout << "step 2: " << shifted << std::endl;

    // step 3: second pass - backward, using next character.
    // key = (i * 5 + next + 11) % 26
    // "imfhh" -> "mafmm"
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
        int next = (i < (int)shifted.size() - 1 && isalpha(shifted[i + 1])) ? (shifted[i + 1] % 26) : 0;
        int key = (i * 5 + next + 11) % 26;
        int enc_pos = (pos + key) % 26;
        shifted[i] = enc_pos + offset;
    }
    std::cout << "step 3: " << shifted << std::endl;

    // step 4: third pass - different formulas for odd/even positions.
    // even: key = (i * 7 + 3) % 26
    // odd:  key = (i * 11 + 9) % 26
    // "mafmm" -> "puwcr"
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
    std::cout << "step 4: " << shifted << std::endl;

    // step 5: Swap adjacent pairs.
    // "puwcr" -> "upcwr"
    // final result for "ahmad" -> "upcwr"
    for (size_t i = 0; i + 1 < shifted.size(); i += 2)
    {
        std::swap(shifted[i], shifted[i + 1]);
    }
    std::cout << "step 5: " << shifted << std::endl;

    return (shifted);
}

std::string decrypt(const std::string &text)
{
    std::string out = text;

    // input: "upcwr"
    
    // reverse step 5: swap back.
    // "upcwr" -> "puwcr"
    for (size_t i = 0; i + 1 < out.size(); i += 2)
    {
        std::swap(out[i], out[i + 1]);
    }
    
    // reverse step 4: undo third pass (odd/even positions).
    // "puwcr" -> "mafmm"
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
    // "mafmm" -> "imfhh"
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
        int next = (i < out.size() - 1 && isalpha(out[i + 1])) ? (out[i + 1] % 26) : 0;
        int key = (i * 5 + next + 11) % 26;
        int dec_pos = (pos - key + 26) % 26;
        out[i] = dec_pos + offset;
    }

    // reverse step 2: undo first pass.
    // "imfhh" -> "damha"
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
    // final result for "ahmad" -> "upcwr"
    std::reverse(out.begin(), out.end());

    return (out);
}

void execution(const std::string &input_file, const std::string &output_file, const std::string &content, bool mode)
{
    if (content.empty())
    {
        throw std::runtime_error((mode ? "Encryption" : "Decryption") + std::string(" failed: Input text is empty."));
    }

    std::cout << (mode ? "Encrypting..." : "Decrypting...") << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    std::string result = mode ? encrypt(content) : decrypt(content);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << (mode ? "Encryption" : "Decryption") << " complete in " << duration << " ms.\n";
    std::cout << "Outfile created: " << output_file << ".\n";

    FILE *file = fopen(output_file.c_str(), "w");
    if (!file)
    {
        throw std::runtime_error("Could not open file for writing: " + output_file);
    }
    for (char c : result)
    {
        fputc(c, file);
    }
    fclose(file);
}

std::string read_file(const std::string &file_name)
{
    std::string content;
    FILE *file = fopen(file_name.c_str(), "r");
    if (!file)
    {
        throw std::runtime_error("Could not open file for reading: " + file_name);
    }
    char ch;
    while ((ch = fgetc(file)) != EOF)
    {
        content += ch;
    }
    fclose(file);
    return (content);
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <-e | -d> <inputfile>\n";
            return (1);
        }
        std::string mode = argv[1];
        std::string input_file = argv[2];
        std::string content = read_file(argv[2]);
        std::string output_file;
        
        // generate output filename with suffix before extension.
        // input.txt -> input_encrypted.txt or input_decrypted.txt
        std::string suffix = (mode == "-e") ? "_encrypted" : "_decrypted";
        size_t dot_pos = input_file.find_last_of('.');
        if (dot_pos != std::string::npos)
        {
            // has extension: input.txt -> input_encrypted.txt
            output_file = input_file.substr(0, dot_pos) + suffix + input_file.substr(dot_pos);
        }
        else
        {
            // no extension: input -> input_encrypted
            output_file = input_file + suffix;
        }
        
        if (mode != "-e" && mode != "-d")
        {
            std::cerr << "Invalid mode. Use -e for encrypt or -d for decrypt.\n";
            return (1);
        }
        execution(input_file, output_file, content, mode == "-e");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}

// yes "abcdefghijklmnopqrstuvwxyz" | head -c 1G > bigfile.txt -> command to create a 1GB file for testing.
