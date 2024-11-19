# Knight's Tour Encryption System

The Knight's Tour Encryption System is a cryptographic application that leverages the Knight's Tour problem on a chessboard to generate a unique encryption key. This key is then used for XOR-based encryption and decryption of messages. The system provides a menu-driven command-line interface (CLI) for user interaction, allowing users to generate keys, save/load keys, encrypt/decrypt messages, and generate detailed reports.

## Table of Contents

- [Features](#features)
- [Technologies Used](#technologies-used)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Features

- **Board Initialization**: Initializes the chessboard and determines the starting position based on a hashed passphrase.
- **Knight's Tour Algorithm**: Uses a backtracking algorithm with Warnsdorff's rule to perform the Knight's Tour and generate the key sequence.
- **File Operations**: Allows users to save and load key sequences to and from files.
- **Encryption and Decryption**: Encrypts and decrypts messages using the generated key sequence.
- **Report Generation**: Generates detailed reports of the encryption key, including its length, sequence, hashed passphrase, and starting position.
- **Performance Measurement**: Measures the performance of key generation, encryption, and decryption processes.
- **Menu-Driven CLI**: Provides a user-friendly command-line interface for interacting with the system.

## Technologies Used

- **C++**: The primary programming language used for the implementation.
- **OpenSSL**: Used for SHA-256 hashing functions.
- **C++ Standard Library**: Provides essential utilities and data structures.
- **Filesystem Library**: Used for filesystem operations (e.g., directory creation, file listing).

## Prerequisites

- **Git**: To clone the repository.
- **C++ Compiler**: To compile the code (e.g., g++17).
- **OpenSSL**: For cryptographic functions.
- **Homebrew** (macOS): To install OpenSSL (optional).

## Installation

1. **Clone the Repository**:
   ```sh
   git clone https://github.com/Venkat-Kolasani/Knight-s-Tour-Based-Cryptographic-Key-Generation.git
   cd Knight-s-Tour-Based-Cryptographic-Key-Generation

2. **Install Dependencies**: Ensure you have OpenSSL Installed.On MACOS,you can istall it using Homebrew :
   ```sh
   brew install openssl

3. **Compile the Program**: 
   ```sh
   g++ -std=c++17 main.cpp -o knight_tour_encryption -I/opt/homebrew/opt/openssl@3/include -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

4. **Run the Program**:
   ```sh
   ./knight_tour_encryption

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgements
- This project was developed as a part of the End Semester Project for course "Design and Analysis of Algorithms" at the University
- The Knight's Tour problem and its applications in cryptography.
- OpenSSL for providing cryptographic functions.
- The C++ Standard Library for providing essential utilities and data structures.
- Insipired by various open source projects and articles
