#include<iostream>
#include<fstream>
using namespace std;

int main(){
    ifstream file("compressed file.bin", ios::binary);
    
    if (file.is_open()) {
        uint8_t charCount, codePadding, textPadding;
        
        // Read charCount
        file.read(reinterpret_cast<char*>(&charCount), sizeof(charCount));
        cout << "Char Count: " << static_cast<int>(charCount) << endl;
        
        // Read Huffman Codes (ch and length)
        for (int i = 0; i < charCount; i++) {
            char ch;
            uint8_t length;
            file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
            file.read(reinterpret_cast<char*>(&length), sizeof(length));
            cout << "Char: " << ch << " Length: " << static_cast<int>(length) << endl;
        }

        // Read codePadding
        file.read(reinterpret_cast<char*>(&codePadding), sizeof(codePadding));
        cout << "Code Padding: " << static_cast<int>(codePadding) << endl;

        // Read rest of the data similarly (codeArray, textPadding, textArray)
        // Add additional reads and logic as per your file structure

        file.close();
    } else {
        cout << "Cannot open file" << endl;
    }

    return 0;
}
