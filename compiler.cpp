#include <string>
#include <bits/stdc++.h>
using namespace std;

//This script is made to convert an assembly like code (string) to a memory array, usable in the vcpu

unordered_map<string, int> instructionToOpcode = {
    {"MOV", 1},
    {"ADD", 2},
    {"SUB", 3},
    {"LOAD", 4},
    {"STORE", 5},
    {"JMP", 6},
    {"CMP", 7},
    {"HALT", 8},
    {"PUSH", 9},
    {"POP", 10},
    {"JZ", 11},
    {"JNZ", 12},
    {"JL", 13},
    {"JG", 14},
    {"JB", 15},
    {"JAE", 16},
    {"CALL", 17},
    {"RET", 18},
    {"PUSHI", 19},   // push imm32
    {"ADDI", 20},   // add imm32
    {"SUBI", 21}    // sub imm32
};

string code;
stringstream codeStream(code);
string line;

int main()
{
    
    while(getline(codeStream, line, '\n'))
    {
        int opcode = instructionToOpcode[line.substr(0, line.find(" "))];
        if(!opcode) return 1; //instruction not found

    }
    return 0;
}