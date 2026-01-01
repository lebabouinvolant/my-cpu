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
6 = jump -> 8 bits pour nouvelle adresse d'instruction
7 = cmp -> 8 bits pour registre A, 8 bits registre B
8 = halt -> rien, on arrête le programme
9 = push -> 8 bits pour le registre
10 = pop -> 8 bits pour le registre
11 = JZ (jump if zero) -> 8 bits pour l'adresse de l'instruction
12 = JNZ (jump if not zero) -> 8 bits pour l'adresse de l'instruction
13 = JL (jump if less) -> 8 bits pour l'adresse de l'instruction
14 = JG (jump if greater) -> 8 bits pour l'adresse de l'instruction
15 = JB (jump if below) -> 8 bits pour l'adresse de l'instruction
16 = JAE (jump if above or equal) -> 8 bits pour l'adresse de l'instruction
17 = call -> 8 bits pour l'adresse de l'instruction suivante
18 = return -> pas d'arguments (carrément chuis choqué)
flags:
sur 8 bits: [inutile], [inutile], [inutile], [inutile], Overflow, carry / borrow, négatif, nul


config : mémoires 8 bits, 4 registres 32 bits, little endian, bits de 192 à 255 réservés à la stack

SP pointe la prochaine case libre, mais qui peut être déjà écrite!, juste pas récupérable
*/


uint8_t memory[256];
uint32_t memorySize = 192; //La taille pour la mémoire sans la stack (la ram utilisable par le programme quoi)
uint32_t realMemorySize = 256; //On n'utilisera pas le size() pour se rapprocher d'un vrai CPU
class VCPU
{
    private:
        uint32_t R[4] = {0,0,0,0};
        uint32_t PC = 0;
        uint32_t SP = 255;
        uint8_t FLAGS = 0;
        bool isPlaying;
        void LOOP()
        {
            if(memory[PC] == 1 && PC < memorySize - 5) // move
            {
                if(memory[PC+1] < 4)
                {
                    R[memory[PC+1]] = memory[PC+2] + (memory[PC+3] << 8) + (memory[PC+4] << 16) + (memory[PC+5] << 24);
                    PC += 6;
                }
                else isPlaying = false; // HALT on invalid index
            }
            else if(memory[PC] == 2 && PC < memorySize-2) // ADD
            {
                uint8_t rIndex = memory[PC+1];
                if(memory[PC+2] < 4 && rIndex < 4)
                {
                    uint32_t originalValue = R[rIndex];
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
                    if(R[rIndex] < originalValue) //Carry car retour au début
                    {
                        FLAGS |= 1 << 2;
                    }
                    else FLAGS &= ~(1 << 2);
                    int32_t a = originalValue; 
                    int32_t b = R[memory[PC+2]]; 
                    int32_t res = R[rIndex];
                    if((a > 0 && b > 0 && res < 0) || (a < 0 && b < 0 && res > 0)) //Overflow (incohérence de signe)
                    {
                        FLAGS |= 1 << 3;
                    }
                    else FLAGS &= ~(1 << 3); 

                    PC += 3; 
                }
                else isPlaying = false;
                                
            }
            else if(memory[PC] == 3 && PC < memorySize-2) // SUB
            {
                uint8_t rIndex = memory[PC+1];
                if(memory[PC+2] < 4 && rIndex <4)
                {
                    uint32_t originalValue = R[rIndex];
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
                    if(R[rIndex] < originalValue)
                    {
                        FLAGS |= 1 << 2;
                    }
                    else FLAGS &= ~(1 << 2);
                    int32_t a = originalValue; 
                    int32_t b = R[memory[PC+2]]; 
                    int32_t res = R[rIndex];
                    if((a > 0 && b < 0 && res < 0) || (a < 0 && b > 0 && res > 0)) //Overflow (incohérence de signe)
                    {
                        FLAGS |= 1 << 3;
                    }
                    else FLAGS &= ~(1 << 3); 
                    PC += 3; 
                }
                else isPlaying = false;
            }

            else if(memory[PC] == 4 && PC < memorySize-2) // LOAD
            {
                if(memory[PC+2] < memorySize-3 && memory[PC+1]<4)
                {
                    R[memory[PC+1]] = memory[memory[PC+2]] + (memory[memory[PC+2]+1] << 8) + (memory[memory[PC+2]+2] << 16) + (memory[memory[PC+2]+3] << 24);
                    PC += 3;
                }
                else isPlaying = false;
            }
            else if(memory[PC] == 5 && PC < memorySize-2) // STORE
            {
                if(memory[PC+2] < memorySize-3 && memory[PC+1] < 4 && memory[PC+2]+3 < 128)
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
            else if(memory[PC] == 6 && PC < memorySize-4) // JUMP
            {
                uint8_t NewPC = memory[PC+1] + (memory[PC+2] << 8) + (memory[PC+3] << 16) + (memory[PC+4] << 24);
                PC = NewPC;
            }
            else if(memory[PC] == 7 && PC < memorySize-1 && memory[PC+2] < 4) // CMP
            {
                uint8_t rIndex = memory[PC+1];
                uint32_t originalValue = R[rIndex];
                uint32_t checkVal = R[rIndex] - R[memory[PC+2]];
                if(checkVal == 0) //Nul
                {
                    FLAGS |= 1 << 0;
                }
                else FLAGS &= ~(1 << 0);
                if(checkVal & (1 << 31)) //Négatif (le bit de poids fort est set à 1 en binaire signé)
                {
                    FLAGS |= 1 << 1;
                }
                else FLAGS &= ~(1 << 1);
                if(checkVal < originalValue)
                {
                    FLAGS |= 1 << 2;
                }
                else FLAGS &= ~(1 << 2);
                int32_t a = R[rIndex]; //On convertit en binaire signé
                int32_t b = R[memory[PC+2]];
                int32_t res = checkVal;
                if((a > 0 && b < 0 && res < 0) || (a < 0 && b > 0 && res > 0)) //Overflow (incohérence de signe)
                {
                    FLAGS |= 1 << 3;
                }
                else FLAGS &= ~(1 << 3); 
                PC += 3; 
            }
            else if(PC >= memorySize || memory[PC] == 8) //HALT
            {
                isPlaying = false;
            }
            else if(memory[PC] == 9 && PC < memorySize - 1) //PUSH
            {
                uint8_t rIndex = memory[PC+1];
                if(rIndex < 4 && SP-3 >= memorySize && SP < 256)
                {
                    memory[SP-3] = R[rIndex];
                    memory[SP-2] = R[rIndex] >> 8;
                    memory[SP-1] = R[rIndex] >> 16;
                    memory[SP] = R[rIndex] >> 24;
                    SP-=4;
                    PC += 2;
                }
                else isPlaying = false;
            }
            else if(memory[PC] == 10 && PC < memorySize - 1) //POP
            {
                uint8_t rIndex = memory[PC+1];
                if(rIndex < 4 && SP+4 < realMemorySize && SP >= memorySize)
                {
                    R[rIndex] = memory[SP+1] + (memory[SP+2] << 8) + (memory[SP+3] << 16) + (memory[SP+4] << 24);                    
                    SP+=4;
                    PC += 2;
                }
                else isPlaying = false;
            }
            else if(memory[PC] == 11 && PC < memorySize - 1 && memory[PC+1] < memorySize) //JZ
            {
                if(FLAGS & (1 << 0) && memory[PC+1] < memorySize)
                {
                    PC = memory[PC+1];
                }
                else PC += 2;
            }
            else if(memory[PC] == 12 && PC < memorySize - 1 && memory[PC+1] < memorySize) //JNZ
            {
                if(!(FLAGS & (1 << 0)))
                {
                    PC = memory[PC+1];
                }
                else PC += 2;

            }
            else if(memory[PC] == 13 && PC < memorySize - 1) //JL
            {
                if((FLAGS & (1 << 1)) != (FLAGS & (1 << 3)))
                {
                    PC = memory[PC+1];
                }
                else PC += 2;
            }
            else if(memory[PC] == 14 && PC < memorySize - 1 && memory[PC+1] < memorySize) //JG
            {
                if((FLAGS & (1 << 1)) == (FLAGS & (1 << 3)) && !(FLAGS & (1 << 0))) //négatif seulement si overflow, et pas nul
                {
                    PC = memory[PC+1];
                }
                else PC += 2;
            }
            else if(memory[PC] == 15 && PC < memorySize - 1 && memory[PC+1] < memorySize) //JB
            {
                if((FLAGS & (1 << 2))) //négatif seulement si overflow, et pas nul
                {
                    PC = memory[PC+1];
                }
                else PC += 2;
            }
            else if(memory[PC] == 16 && PC < memorySize - 1 && memory[PC+1] < memorySize) //JAE
            {
                if(!(FLAGS & (1 << 2))) //négatif seulement si overflow, et pas nul
                {
                    PC = memory[PC+1];
                }
                else PC += 2;
            }
            else if(memory[PC] == 17) //call
            {
                if(SP+3 >= memorySize && SP < 256)
                {    
                    memory[SP-3] = (PC+2);
                    memory[SP-2] = (PC+2) >> 8;
                    memory[SP-1] = (PC+2) >> 16;
                    memory[SP] = (PC+2) >> 24;
                    PC = memory[PC+1];
                    SP-=4;
                }
                else isPlaying = false;
            }
            else if(memory[PC] == 18 && PC < memorySize - 2 && memory[PC+1] < memorySize) //return
            {
                if(SP+4 < realMemorySize && SP >= memorySize)
                {   
                    PC = memory[SP+1] + (memory[SP+2] << 8) + (memory[SP+3] << 16) + (memory[SP+4] << 24);
                    SP += 4;
                }
                else isPlaying = false;
            }

            else{isPlaying = false;} //opcode inconnu -> HALT
            
        }
    public :
        void START()
        {
            isPlaying = true;
            while(isPlaying && PC < 192) //Check if not overflow
            {
                LOOP();
            }
        }
};
