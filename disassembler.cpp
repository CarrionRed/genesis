#include <iostream>
#include <vector>
#include <string>
#include <string>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]){
    ifstream inputFile;
    ofstream outputFile;
    string tRecords, toWrite;
    //type 2 opcode and mnemonics
    const static string type2[] = {"ADDR", "CLEAR", "COMPR", "DIVR", "MULR", "RMO", "SHIFTL", "SHIFTR", "SUBR", "SVC", "TIXR"};
    const static int ops2[] = {0x90, 0xB4, 0xA0, 0x9C, 0x98, 0xAC, 0xA4, 0xA8, 0x94, 0xB0, 0xB8};
    //type 3/4 opcode and mnemonics
    const static string type3[] = {
    "ADD", "ADDF", "AND", "COMP", "COMPF", "DIV", "DIVF", "J", "JEQ", "JGT", "JLT",
    "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", 
    "MULF", "OR", "RD", "RSUB", "SSK", "STA", "STB","STCH", "STF", "STI", "STL",
    "STS", "STSW", "STT", "STX", "SUB", "SUBF", "TD", "TIX", "WD"};
    const static int ops3[] = {
    0x18, 0x58, 0x40, 0x28, 0x88, 0x24, 0x64, 0x3C, 0x30, 0x34, 0x38, 0x48, 0x00, 
    0x68, 0x50, 0x70, 0x08, 0x6C, 0x74, 0x04, 0xD0, 0x20, 0x60, 0x44, 0xD8, 0x4C,
    0xEC, 0x0C, 0x78, 0x54, 0x80, 0xD4, 0x14, 0x7C, 0xE8, 0x84, 0x10, 0x1C, 0x5C,
    0xE0, 0x2C, 0xDC};

    // pulls object code from text records in obj file
    inputFile.open(argv[1]);
    if(inputFile.is_open()){
        string line;
        while(getline (inputFile, line)){
            if(line.at(0) == 'T'){
                tRecords = line.substr(9);
                inputFile.close(); 
            }       
        }
    }

    outputFile.open("obj_struct.txt");
    outputFile << "INSTR           FORMAT          OAM             TAAM            OBJ\n" << toWrite;
    while(tRecords.length() > 1){
        bool isType2 = false;
        int opcode = stoi(tRecords.substr(0, 2), 0, 16) & 0b11111100;

        //check for type 2 instructions
        for(int i = 0; i < sizeof(ops2)/sizeof(ops2[0]); i++){
            if(opcode == ops2[i]){
                isType2 = true;
                outputFile << type2[i] << std::setw(17-type2[i].length())<< 2 << std::setw(51)<< tRecords.substr(0,4)<< '\n';
                tRecords = tRecords.substr(4);
                break;
            }
        }
        if(isType2)
            continue;
        //find type 3/4 mnemonic 
        for(int i = 0; i < sizeof(ops3)/sizeof(ops3[0]); i++){
            if(opcode == ops3[i]){
                outputFile << type3[i] << std::setw(17-type3[i].length());
                break;
            }
        }
        //checking for type 3 or type 4
        int nixbpe = stoi(tRecords.substr(0, 3), 0, 16) & 0b00111111;
        bool isType4 = false;
        bool isIndexed = false;
        string prev;
        if((nixbpe & 0b000001) == 0b000001){
            outputFile << 4;
            isType4 = true;
        }
        else{
                outputFile << 3;
        }
        //check operand addressing mode
        if((nixbpe & 0b110000) == 0b100000){
            prev = "indirect"; 
        }
        else if((nixbpe & 0b110000) == 0b010000){
            prev = "immediate";
        }
        else{
            prev = "simple";
            //check indexed or not indexed
            if((nixbpe & 0b001000) == 0b001000){
                isIndexed = true;
            }
        }
        outputFile << std::setw(15 + prev.length()) << prev;

        //check for pc/base/absolute addressing
        string TAAM;
        if((nixbpe & 0b000110) == 0b000010){
            TAAM = "pc";
        }
        else if((nixbpe & 0b000110) == 0b000100){
            TAAM = "base";
        }
        else{
            TAAM = "absolute";
        }
        if(isIndexed){
            TAAM = TAAM + "_indexed";
        }
        outputFile << std::setw(16-prev.length()+TAAM.length()) << TAAM;
        //handle type 3 vs 4 object code coumn
        if(isType4){
            outputFile << std::setw(24-TAAM.length()) << tRecords.substr(0,8)<< '\n';
            tRecords = tRecords.substr(8);
        }
        else{
            outputFile << std::setw(22-TAAM.length()) << tRecords.substr(0,6)<< '\n';
            tRecords = tRecords.substr(6);
        }
    }
    outputFile.close();
}
