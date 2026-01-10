#include <string>
#include <cstdint>
#include <bits/stdc++.h>
#include <iostream>
#include "./VCPU.h"
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

string code = "";
stringstream codeStream(code);
string line;
vector<uint8_t> programArray;

void pushImm32(vector<uint8_t>& v, int32_t x) {
    v.push_back(x & 0xFF);
    v.push_back((x >> 8) & 0xFF);
    v.push_back((x >> 16) & 0xFF);
    v.push_back((x >> 24) & 0xFF);
}

uint8_t parseRegister(const string& r) {
    return r[1] - '0'; // R0 → 0
}

int main()
{
    
    while(getline(codeStream, line, '\n'))
    {
        stringstream ss(line);
        string instruction, a1, a2;
        ss >> instruction >> a1 >> a2;
        int opcode = instructionToOpcode[instruction];
        if(!opcode) return 1; //instruction not found
        programArray.push_back(opcode);

        //instructions sans argument
        if(opcode == 8 || opcode == 18)
        {
            //Do nothing
            programArray.push_back(0); //no register number
            pushImm32(programArray, 0);
        }
        else if(opcode == 6 || opcode == 11 || opcode == 12 || opcode == 13 || opcode == 14|| opcode == 15 || opcode == 16 || opcode == 17) 
        {
            //instructions sans registres
            programArray.push_back(0); //no register number
            uint32_t data = stoi(a1);
            pushImm32(programArray, data);
        }
        else if(opcode == 1 || opcode == 2 || opcode == 3 || opcode == 6 || opcode == 7)
        {
            //instructions avec 2 registres
            int regNumber1 = parseRegister(a1);
            int regNumber2 = parseRegister(a2);

            programArray.push_back(regNumber1);
            programArray.push_back(regNumber2);
            programArray.push_back(0); //complete with empty data
            programArray.push_back(0); //complete with empty data
            programArray.push_back(0); //complete with empty data
        }
        else
        {
            int regNumber = parseRegister(a1);
            //Instructions avec registre et data
            programArray.push_back(regNumber);
            pushImm32(programArray, stoi(a2));
        }
        if(programArray.size()<192)
        {
            uint8_t memory[192];
            copy(programArray.begin(), programArray.end(), memory);
            VCPU cpu(programArray);
        }
        
    }

    return 0;
}