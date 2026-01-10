#ifndef VCPU_H
#define VCPU_H

#include <cstdint>

class VCPU {
public:
    VCPU(uint8_t memory[192]);
    void START();
};

#endif