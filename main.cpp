/* The Knight's Tour Encryption System is a cryptographic application that leverages the 
Knight's Tour problem on a chessboard to generate a unique encryption key. 
This key is then used for XOR-based encryption and decryption of messages. 
The system provides a menu-driven command-line interface (CLI) for user interaction, 
allowing users to generate keys, save/load keys, encrypt/decrypt messages, and generate detailed reports.
*/

#include <iostream>     // For standard input/output stream operations
#include <string>       // For string manipulation
#include <ctime>        // For time-related functions (e.g., seeding random number generator)
#include <cstdlib>      // For general purpose functions (e.g., random number generation)
#include <iomanip>      // For input/output manipulation (e.g., setting width, fill characters)
#include <vector>       // For using the vector container
#include <algorithm>    // For standard algorithms (e.g., sort)
#include <fstream>      // For file stream operations
#include <sstream>      // For string stream operations
#include <filesystem>   // For filesystem operations (e.g., directory creation, file listing)
#include <openssl/sha.h> // For SHA-256 hashing functions
#include <chrono>       // For high-resolution clock and timing operations
#include <thread>       // For thread operations (e.g., sleep)

using namespace std;
namespace fs = std::filesystem; // Alias for the filesystem namespace

// Knight's move directions
int dx[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
int dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };

/**
 * @brief Initializes the chessboard and determines the starting position based on a passphrase.
 * 
 * @param board The chessboard to initialize.
 * @param passphrase The passphrase used to generate the starting position.
 * @param startX The starting X position of the knight.
 * @param startY The starting Y position of the knight.
 * @param hashedPassphrase The hashed version of the passphrase.
 */

void createBoard(vector<vector<int>>& board, const string& passphrase, int &startX, int &startY, string& hashedPassphrase) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)passphrase.c_str(), passphrase.size(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    hashedPassphrase = ss.str();

    int value = 0;
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            board[i][j] = value++;
        }
    }

    startX = hash[0] % board.size();
    startY = hash[1] % board[0].size();
}

/**
 * @brief Checks if a move is valid.
 * 
 * @param x The X position to check.
 * @param y The Y position to check.
 * @param visited The visited squares on the board.
 * @return true if the move is valid, false otherwise.
 */

bool isValidMove(int x, int y, const vector<vector<bool>>& visited) {
    return (x >= 0 && x < visited.size() && y >= 0 && y < visited[0].size() && !visited[x][y]);
}

/**
 * @brief Calculates the degree of a square (number of valid moves from the square).
 * 
 * @param x The X position of the square.
 * @param y The Y position of the square.
 * @param visited The visited squares on the board.
 * @return The degree of the square.
 */
int getDegree(int x, int y, const vector<vector<bool>>& visited) {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (isValidMove(nx, ny, visited)) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Performs the Knight's Tour using a backtracking algorithm.
 * 
 * @param x The current X position of the knight.
 * @param y The current Y position of the knight.
 * @param movei The current move number.
 * @param board The chessboard.
 * @param visited The visited squares on the board.
 * @param key The generated key sequence.
 * @return true if a complete tour is found, false otherwise.
 */
bool knightTour(int x, int y, int movei, vector<vector<int>>& board, vector<vector<bool>>& visited, vector<int>& key) {
    visited[x][y] = true;
    key.push_back(board[x][y]);

    if (movei == board.size() * board[0].size()) return true;

    vector<pair<int, int>> moves;
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (isValidMove(nx, ny, visited)) {
            moves.push_back({getDegree(nx, ny, visited), i});
        }
    }

    sort(moves.begin(), moves.end());

    for (auto& move : moves) {
        int i = move.second;
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (knightTour(nx, ny, movei + 1, board, visited, key)) {
            return true;
        }
    }

    visited[x][y] = false;
    key.pop_back();
    return false;
}

/**
 * @brief Saves the generated key sequence to a binary file.
 * 
 * @param filename The name of the file to save the key to.
 * @param key The generated key sequence.
 * @return true if the key is saved successfully, false otherwise.
 */
bool saveKeyToFile(const string& filename, const vector<int>& key) {
    fs::create_directory("data"); // Ensure the data directory exists
    string filepath = "data/" + filename;
    ofstream outFile(filepath, ios::binary);
    if (!outFile) return false;

    outFile.write(reinterpret_cast<const char*>(key.data()), key.size() * sizeof(int));
    return true;
}

/**
 * @brief Loads a key sequence from a binary file.
 * 
 * @param filename The name of the file to load the key from.
 * @param key The loaded key sequence.
 * @return true if the key is loaded successfully, false otherwise.
 */
bool loadKeyFromFile(const string& filename, vector<int>& key) {
    string filepath = "data/" + filename;
    ifstream inFile(filepath, ios::binary);
    if (!inFile) return false;

    key.clear();
    int value;
    while (inFile.read(reinterpret_cast<char*>(&value), sizeof(int))) {
        key.push_back(value);
    }
    return true;
}

/**
 * @brief Lists all available key files in the data directory.
 */
void listKeyFiles() {
    cout << "Available key files:" << endl;
    for (const auto& entry : fs::directory_iterator("data")) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            cout << entry.path().filename().string() << endl;
        }
    }
}

/**
 * @brief Extends the key sequence to ensure it is at least as long as the message.
 * 
 * @param key The key sequence to extend.
 * @param length The desired length of the extended key.
 */
void extendKey(vector<int>& key, size_t length) {
    vector<int> extendedKey;
    while (extendedKey.size() < length) {
        extendedKey.insert(extendedKey.end(), key.begin(), key.end());
    }
    key = extendedKey;
}

/**
 * @brief Encrypts a message using the XOR operation with the key sequence.
 * 
 * @param data The message to encrypt.
 * @param encryptedData The encrypted message.
 * @param key The key sequence.
 */
void encryptData(const string& data, string& encryptedData, const vector<int>& key) {
    for (size_t i = 0; i < data.size(); i++) {
        encryptedData += data[i] ^ key[i % key.size()];
    }
}

/**
 * @brief Decrypts a message using the XOR operation with the key sequence.
 * 
 * @param encryptedData The encrypted message.
 * @param decryptedData The decrypted message.
 * @param key The key sequence.
 */
void decryptData(const string& encryptedData, string& decryptedData, const vector<int>& key) {
    for (size_t i = 0; i < encryptedData.size(); i++) {
        decryptedData += encryptedData[i] ^ key[i % key.size()];
    }
}

/**
 * @brief Converts a string of bytes to a hexadecimal representation.
 * 
 * @param input The input string of bytes.
 * @return The hexadecimal representation of the input string.
 */
string bytesToHex(const string& input) {
    stringstream ss;
    ss << hex << setfill('0');
    for (unsigned char c : input) {
        ss << setw(2) << static_cast<int>(c) << ' ';
    }
    return ss.str();
}

/**
 * @brief Generates a detailed report of the encryption key.
 * 
 * @param key The encryption key sequence.
 * @param hashedPassphrase The hashed passphrase used to generate the key.
 * @param startX The starting X position of the knight.
 * @param startY The starting Y position of the knight.
 */
void generateReport(const vector<int>& key, const string& hashedPassphrase, int startX, int startY) {
    cout << "\n=== Encryption Key Report ===" << endl;
    cout << "Key Length: " << key.size() << endl;
    cout << "Key Sequence: ";
    for (int i : key) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Hashed Passphrase: " << hashedPassphrase << endl;
    cout << "Starting Position: (" << startX << ", " << startY << ")" << endl;
}

/**
 * @brief Measures the performance of key generation, encryption, and decryption.
 */
void measurePerformance() {
    int boardSize = 8;
    vector<vector<int>> board(boardSize, vector<int>(boardSize));
    vector<vector<bool>> visited(boardSize, vector<bool>(boardSize));
    vector<int> key;
    int startX, startY;
    string hashedPassphrase;

    // Measure time to generate key
    auto start = chrono::high_resolution_clock::now();
    createBoard(board, "samplepassphrase", startX, startY, hashedPassphrase);
    knightTour(startX, startY, 1, board, visited, key);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time to generate key: " << duration.count() << " ms" << endl;

    // Measure time to encrypt message
    string message = "This is a sample message for encryption.";
    string encryptedMessage;
    start = chrono::high_resolution_clock::now();
    encryptData(message, encryptedMessage, key);
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time to encrypt message: " << duration.count() << " ms" << endl;

    // Measure time to decrypt message
    string decryptedMessage;
    start = chrono::high_resolution_clock::now();
    decryptData(encryptedMessage, decryptedMessage, key);
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time to decrypt message: " << duration.count() << " ms" << endl;
}

/**
 * @brief Main function providing a menu-driven CLI for the Knight's Tour encryption system.
 * 
 * @return int Exit status.
 */
int main() {
    srand(time(0));

    int boardSize;
    vector<vector<int>> board;
    vector<vector<bool>> visited;
    vector<int> key;
    int startX, startY;
    string hashedPassphrase;

    // Set the board size first
    cout << "Enter board size (e.g., 8 for 8x8 board): ";
    cin >> boardSize;
    cin.ignore(); // Ignore the newline character left in the input buffer
    board.resize(boardSize, vector<int>(boardSize));
    visited.resize(boardSize, vector<bool>(boardSize));
    cout << "Board size set to " << boardSize << "x" << boardSize << endl;

    while (true) {
        cout << "\n=== Knight's Tour Encryption System ===" << endl;
        cout << "1. Generate new key" << endl;
        cout << "2. Save key to file" << endl;
        cout << "3. Load key from file" << endl;
        cout << "4. Encrypt message" << endl;
        cout << "5. Decrypt message" << endl;
        cout << "6. Generate report" << endl;
        cout << "7. Measure performance" << endl;
        cout << "8. Exit" << endl;
        cout << "Choice: ";

        string input;
        getline(cin, input);

        switch (input[0]) {
            case '1': {
                cout << "Enter passphrase: ";
                string passphrase;
                getline(cin, passphrase);
                createBoard(board, passphrase, startX, startY, hashedPassphrase);
                cout << "Starting position: (" << startX << ", " << startY << ")" << endl;

                // Reset visited array and key vector
                fill(visited.begin(), visited.end(), vector<bool>(boardSize, false));
                key.clear();

                if (knightTour(startX, startY, 1, board, visited, key)) {
                    cout << "Knight's Tour completed successfully.\nKey sequence generated :" << endl;
                    for (int i : key) {
                        cout << i << " ";
                    }
                    cout << endl;
                } else {
                    cout << "Knight's Tour failed to complete." << endl;
                }
                break;
            }
            case '2': {
                cout << "Enter filename to save the key: ";
                string filename;
                getline(cin, filename);
                if (saveKeyToFile(filename, key)) {
                    cout << "Key saved successfully to " << filename << endl;
                } else {
                    cout << "Failed to save key to " << filename << endl;
                }
                break;
            }
            case '3': {
                listKeyFiles();
                cout << "Enter key file name to load: ";
                string filename;
                getline(cin, filename);
                if (loadKeyFromFile(filename, key)) {
                    cout << "Key loaded successfully." << endl;
                } else {
                    cout << "Failed to load key." << endl;
                }
                break;
            }
            case '4': {
                cout << "Enter message to encrypt: ";
                string message;
                getline(cin, message);
                extendKey(key, message.size());
                string encryptedMessage;
                encryptData(message, encryptedMessage, key);
                cout << "Encrypted Message (in hex): " << bytesToHex(encryptedMessage) << endl;
                break;
            }
            case '5': {
                cout << "Enter message to decrypt (in hex): ";
                string hexMessage;
                getline(cin, hexMessage);
                string encryptedMessage;
                istringstream hexStream(hexMessage);
                unsigned int c;
                while (hexStream >> hex >> c) {
                    encryptedMessage += static_cast<char>(c);
                }
                string decryptedMessage;
                decryptData(encryptedMessage, decryptedMessage, key);
                cout << "Decrypted Message: " << decryptedMessage << endl;
                break;
            }
            case '6': {
                generateReport(key, hashedPassphrase, startX, startY);
                break;
            }
            case '7': {
                measurePerformance();
                break;
            }
            case '8':
                cout << "Exiting..." << endl;
                return 0;
            default:
                cout << "Invalid choice! Please enter a number between 1 and 8." << endl;
        }
    }

    return 0;
}
