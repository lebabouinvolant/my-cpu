#include <cstdint>
#include <iostream>
#include <cmath>

/*
opcodes:
1 = move -> 8bits registres, 32 bits valeur
2 = add -> 8 bits registre dest, 8 bits registre src
3 = sub -> 8 bits registre dest, 8 bits registre src
4 = load -> 8 bits d'index registre, 8 bits d'index mémoire
5 = store -> 8 bits d'index registre, 8 bits d'index mémoire
6 = jump -> 8 bits pour nouvelle adresse de programme
7 = cmp -> 8 bits pour registre A, 8 bits registre B
8 = halt -> rien, on arrête le programme


flags:
sur 8 bits: [inutile], [inutile], [inutile], [inutile], Overflow, carry / borrow, négatif, nul


config : mémoires 8 bits, 4 registres 32 bits, little endian, bits de 192 à 256 réservés à la stack
*/


uint8_t memory[256];
uint32_t memorySize = 256;
class VCPU
{
    private:
        uint32_t R[4] = {0,0,0,0};
        uint32_t PC = 0;
        uint32_t SP = 256;
        uint8_t FLAGS = 0;
        bool isPlaying;
        void LOOP()
        {
            if(memory[PC] == 1) // STORE
            {
                if(PC < memorySize - 5 && memory[PC+1] < 4)
                {
                    R[memory[PC+1]] = memory[PC+2] + (memory[PC+3] << 8) + (memory[PC+4] << 16) + (memory[PC+5] << 24);
                    PC += 6;
                }
                else isPlaying = false; // HALT on invalid index
            }
            else if(memory[PC] == 2) // ADD
            {
                uint8_t rIndex = memory[PC+1];
                if(PC < memorySize-2 && memory[PC+2] < 4 && rIndex < 4)
                {
                    uint64_t checkCarry = R[rIndex] + R[memory[PC+2]];
                    R[rIndex] += R[memory[PC+2]];
                    if(R[rIndex] == 0)
                    {
                        FLAGS |= 1 << 0;
                    }
                    else FLAGS &= ~(1 << 0);
                    if(R[rIndex] & (1 << 31))
                    {
                        FLAGS |= 1 << 1;
                    }
                    else FLAGS &= ~(1 << 1);
                    if(checkCarry > 0xFFFFFFFF)
                    {
                        FLAGS |= 1 << 2;
                    }
                    else FLAGS &= ~(1 << 2);
                    PC += 3; 
                }
                else isPlaying = false;
                                
            }
            else if(memory[PC] == 3) // SUB
            {
                uint8_t rIndex = memory[PC+1];
                if(PC < memorySize-2 && memory[PC+2] < 4 && rIndex <4)
                {
                    uint64_t checkCarry = R[rIndex] - R[memory[PC+2]];
                    R[rIndex] -= R[memory[PC+2]];
                    if(R[rIndex] == 0)
                    {
                        FLAGS |= 1 << 0;
                    }
                    else FLAGS &= ~(1 << 0);
                    if(R[rIndex] & (1 << 31))
                    {
                        FLAGS |= 1 << 1;
                    }
                    else FLAGS &= ~(1 << 1);
                    if(R[rIndex] < R[memory[PC+2]])
                    {
                        FLAGS |= 1 << 2;
                    }
                    else FLAGS &= ~(1 << 2);
                    PC += 3; 
                }
                else isPlaying = false;
            }

            else if(memory[PC] == 4) // LOAD
            {
                if(PC < memorySize-2 && memory[PC+2] < memorySize-3 && memory[PC+1]<4)
                {
                    R[memory[PC+1]] = memory[memory[PC+2]] + (memory[memory[PC+2]+1] << 8) + (memory[memory[PC+2]+2] << 16) + (memory[memory[PC+2]+3] << 24);
                    PC += 3;
                }
                else isPlaying = false;
            }
            else if(memory[PC] == 5) // STORE
            {
                if(PC < memorySize-2 && memory[PC+2] < memorySize-3 && memory[PC+1] < 4 && memory[PC+2]+3 < 128)
                {
                    uint8_t Rindex = memory[PC+1];
                    memory[memory[PC+2]] = R[Rindex];
                    memory[memory[PC+2]+1] = R[Rindex] >> 8;
                    memory[memory[PC+2]+2] = R[Rindex] >> 16;
                    memory[memory[PC+2]+3] = R[Rindex] >> 24;
                    PC += 3;
                }
                else isPlaying = false;
            }
            else if(memory[PC] == 6) // JUMP
            {
                if(PC < memorySize-1)
                {
                    uint8_t NewPC = memory[PC+1];
                    PC = NewPC;
                }
                else isPlaying = false;
                
            }
            else if(memory[PC] == 7) // CMP
            {
                uint8_t rIndex = memory[PC+1];
                uint64_t checkCarry = R[rIndex] - R[memory[PC+2]];
                uint32_t checkVal = R[rIndex] - R[memory[PC+2]];
                if(checkVal == 0)
                {
                    FLAGS |= 1 << 0;
                }
                else FLAGS &= ~(1 << 0);
                if(checkVal & (1 << 31))
                {
                    FLAGS |= 1 << 1;
                }
                else FLAGS &= ~(1 << 1);
                if(checkVal < R[memory[PC+2]])
                {
                    FLAGS |= 1 << 2;
                }
                else FLAGS &= ~(1 << 2);
                PC += 3; 

            }
            else if(memory[PC] == 8) //HALT
            {
                isPlaying = false;
            }
            else{isPlaying = false;} //opcode inconnu -> HALT
            
        }
    public :
        void START()
        {
            isPlaying = true;
            while(isPlaying)
            {
                LOOP();
                if(PC > 127) isPlaying = false;
            }
        }
};
