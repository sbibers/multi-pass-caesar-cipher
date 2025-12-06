# Multi-Pass Caesar Cipher Encryption

A complex Caesar cipher implementation in C++ that uses multiple encryption passes to make the cipher harder to break.

## Overview

This is an enhanced version of the classic Caesar cipher that applies 5 different transformations to encrypt text. Unlike the simple Caesar cipher (which can be broken by trying 26 shifts), this algorithm is much more difficult to crack without knowing the exact steps and constants used.

## How It Works

The encryption process applies 5 steps:

| Step | Description |
|------|-------------|
| 1 | Reverse the string |
| 2 | Forward shift based on character position |
| 3 | Backward pass using next character dependency |
| 4 | Different shift formulas for odd/even positions |
| 5 | Swap adjacent character pairs |

### Example: Encrypting "ahmad"

```
Original:     ahmad
Step 1 (rev): damha
Step 2:       imfhh
Step 3:       ovttm
Step 4:       rpkjr
Step 5 (swap):prjkr  ← Final encrypted result
```

## Features

- ✅ Preserves case (uppercase/lowercase)
- ✅ Non-alphabetic characters remain unchanged
- ✅ Multiple encryption passes
- ✅ Position-dependent keys
- ✅ Character dependency between positions
- ✅ Fully reversible decryption
- ✅ File input/output support
- ✅ Exception handling for errors
- ✅ Automatic output filename generation

## Build & Run

```bash
# Compile
c++ multi-pass-caesar-cipher.cpp -o cipher

# Encrypt a file
./cipher -e input.txt
# Output: input_encrypted.txt

# Decrypt a file
./cipher -d input_encrypted.txt
# Output: input_encrypted_decrypted.txt
```

## Usage

```
Usage: ./cipher <mode> <input_file>
Modes:
  -e  Encrypt the input file
  -d  Decrypt the input file
Output:
  Encrypt: input.txt -> input_encrypted.txt
  Decrypt: input.txt -> input_decrypted.txt
Example:
  ./cipher -e plaintext.txt
  ./cipher -d encrypted.txt
```

## Output Example

```
Reading input file: test.txt
File size: 5 bytes
Encrypting...
Writing output file: test_encrypted.txt
-------------------------------
Operation completed successfully!
Time taken: 0 ms
Input file: test.txt
Output file: test_encrypted.txt
```

## Performance

| Input Size | Approximate Time |
|------------|------------------|
| 1 MB       | ~50 ms           |
| 10 MB      | ~500 ms          |
| 1 GB       | ~51 seconds      |

## Algorithm Details

### Step 2: Position-based shift
```
key = (i * 7 + 5) % 26
```

### Step 3: Next character dependency
```
key = (i * 5 + next_char + 11) % 26
```

### Step 4: Odd/Even formulas
```
Even: key = (i * 7 + 3) % 26
Odd:  key = (i * 11 + 9) % 26
```

## Security Note

⚠️ **This is NOT cryptographically secure.**

This algorithm is suitable for:
- Learning about cryptography
- Simple text obfuscation
- Fun/educational projects

**Do NOT use for:**
- Passwords
- Sensitive data
- Financial information
- Anything requiring real security

For real security, use established encryption libraries like OpenSSL with AES.

## License

MIT License - Feel free to use and modify.
