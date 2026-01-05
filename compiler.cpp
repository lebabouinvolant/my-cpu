#include <string>
#include <cstdint>
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
    {"PUSHI", 19},
    {"ADDI", 20},
    {"SUBI", 21}
};

string code;
stringstream codeStream(code);
string line;
vector<uint8_t> programArray;
int main()
{
    
    while(getline(codeStream, line, '\n'))
    {
        uint8_t opcode = instructionToOpcode[line.substr(0, line.find(" "))];
        if(!opcode) return 1; //instruction not found
        programArray.push_back(opcode);
        //instructions sans argument
        if(opcode == 8 || opcode == 18)
        {
            //Do nothing
            programArray.push_back(0); //no register number
            programArray.push_back(0); //no data
            programArray.push_back(0); //no data
            programArray.push_back(0); //no data
            programArray.push_back(0); //no data

        }
        else if(opcode == 6 || opcode == 11 || opcode == 12 || opcode == 13 || opcode == 14|| opcode == 15 || opcode == 16 || opcode == 17) 
        {
            //instructions sans registres
            programArray.push_back(0); //no register number
            int32_t data = stoi(line.substr(1, line.find(" ")));
            programArray.push_back(data);
            programArray.push_back(data >> 8);
            programArray.push_back(data >> 16);
            programArray.push_back(data >> 24);
        }
        else if(opcode == 1 || opcode == 2 || opcode == 3 || opcode == 6 || opcode == 7)
        {
            //instructions avec 2 registres
            string R1 = line.substr(1, line.find(" "));
            string R2 = line.substr(1, line.find(" "));
        }
    }
    return 0;
}