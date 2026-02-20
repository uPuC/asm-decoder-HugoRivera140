/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, 0xA0, 0xE0, 0xB2, 0xE0, 0x0D, 0x91, 0x00, 0x30, 0xE9, 0xF7, 0x11, 0x97, 0xC0, 0xE0, 0xC4, 
    0xD2, 0xE0, 0x09, 0x91, 0x1E, 0x91, 0x01, 0x17, 0x51, 0xF4, 0x0A, 0x2F, 0x0A, 0x95, 0x1C, 0x2F, 0x65, 
    0x01, 0x17, 0xB9, 0xF7, 0x0B, 0x2F, 0x1D, 0x2F, 0x01, 0x17, 0x99, 0xF7, 0x03, 0x94, 0x00, 0x00 };

// 0x2400 - 0010 0100 0000 0000
// CLR R0

// 0xE0A0 - 1110 0000 1010 0000
// LDI R26, 0x00

// 0xE0B2 - 1110 0000 1011 0010
// LDI R27, 0x02

// 0x910D - 1001 0001 0000 1101
// LD R16, X+

// 0x3000 - 0011 0000 0000 0000
// CPI R16, 0x00

// 0xF7E9 - 1111 0111 1110 1001
// BRNE 0x7D

// 0x9711 - 1001 0111 0001 0001
// SBIW R25, 0x01

// 0xE0C0 - 1110 0000 1100 0000
// LDI R28, 0x00

// 0xD2C4 - 1101 0010 1100 0100
// RCALL 0x2C4

// 0xE0E0 - 1110 0000 1110 0000
// SBC R30, R0

// 0x9109 - 1001 0001 0000 1001
// ADC R9, R17

// 0x911E - 1001 0001 0001 1110
// MOVW R18, R2

// 0xF401 - 1111 0100 0000 0001
// SUBI R17, 0x107

// 0x2F51 - 0010 1111 0101 0001
// SBC R15, R20

// 0x950A - 1001 0101 0000 1010
// SBC R2, R31

// 0x2F1C - 0010 1111 0001 1100
// ADC R9, R5

// 0x0165 - 0000 0001 0110 0101
// ORI R18, 0x50F

// 0xF7B9 - 1111 0111 1011 1001
// CP R16, R17

// 0x2F0B - 0010 1111 0000 1011
// BRNE 0x77

// 0x2F1D - 0010 1111 0001 1101
// MOV R16, R27

// 0xF701 - 1111 0111 0000 0001
// MOV R17, R29

// 0x9499 - 1001 0100 1001 1001
// CP R16, R17

// 0x0000 - 0000 0000 0000 0000
// BRNE 0x73

enum{
    e_NOP,
};


// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND,SBC,CP
    
    struct{
        uint16_t K4:4;
        uint16_t d4:4;
        uint16_t op8:8;
    }type3;
    
    struct{
        uint16_t s3:3;
        uint16_t d5:5;
        uint16_t op8:8;
    }type4;
    
    struct{
        uint16_t s7:7;
        uint16_t op9:9;
    }type5;
    
    struct{
        uint16_t K6:6;
        uint16_t d2:2;
        uint16_t op8:8;
    }type6;
    
    struct{
        uint16_t K12:12;
        uint16_t op4:4;
    }type7;
    
    struct{
        uint16_t spare8:8;
        uint16_t op8:8;
    }type8;
    
} Op_Code_t;

int main()
{
    Op_Code_t instruction;
    uint16_t inst_count = 0;
    uint16_t inst16;
    uint16_t flash_size = sizeof(flash_mem);
    
    printf("- Practica 2: AVR OpCode -\n");
    printf("Flash size: %d bytes\n\n", flash_size);
    
    // Decode the instructions by cycling through the array
    for (uint16_t idx = 0; idx < flash_size; idx += 2)
    {
        // Reconstruir la instrucción en little endian correctamente
        inst16 = flash_mem[idx] | (flash_mem[idx+1] << 8);
        instruction.op16 = inst16;
        
        printf("[%2d] 0x%04X - ", inst_count, instruction.op16);
        
        // CLR R0 (0010 0100 0000 0000)
        if (instruction.op16 == 0x2400)
            printf("CLR r0\n");
        
        // LDI Rd, K (1110 KKKK dddd KKKK)
        else if ((instruction.op16 & 0xF000) == 0xE000)
            printf("LDI r%d, 0x%X\n", 16 + instruction.type3.d4, (instruction.type3.op8 & 0x0F) << 4 | instruction.type3.K4);
        
        // LD Rd, X+ (1001 000d dddd 1101)
        else if ((instruction.op16 & 0xFE0F) == 0x900D)
            printf("LD r%d, X+\n", (instruction.op16 >> 4) & 0x1F);
        
        // CPI Rd, K (0011 KKKK dddd KKKK)
        else if ((instruction.op16 & 0xF000) == 0x3000)
            printf("CPI r%d, 0x%X\n", 16 + instruction.type4.d5, (instruction.type4.op8 & 0x0F) << 4 | instruction.type4.s3);
        
        // BRNE s (1111 01kk kkkk ks00) - Branch if Not Equal
        else if ((instruction.op16 & 0xF800) == 0xF000)
            printf("BRNE 0x%X\n", instruction.type5.s7);
        
        // SBIW Rd, K (1001 0111 KKdd KKKK)
        else if ((instruction.op16 & 0xFF00) == 0x9700)
            printf("SBIW r%d, 0x%X\n", 24 + (instruction.type6.d2 * 2), instruction.type6.K6);
        
        // RCALL k (1101 kkkk kkkk kkkk)
        else if ((instruction.op16 & 0xF000) == 0xD000)
            printf("RCALL 0x%X\n", instruction.type7.K12 & 0xFFF);
        
        // MOVW Rd, Rr (0000 0001 dddd rrrr)
        else if ((instruction.op16 & 0xFF00) == 0x0100)
            printf("MOVW r%d, r%d\n", ((instruction.op16 >> 4) & 0x0F) * 2, (instruction.op16 & 0x0F) * 2);
        
        // CP Rd, Rr (0001 01rd dddd rrrr) - bits 11-10 = 01
        else if ((instruction.op16 & 0xFC00) == 0x1400)
            printf("CP r%d, r%d\n", (instruction.op16 >> 4) & 0x1F, (instruction.op16 & 0x0F) | ((instruction.op16 >> 5) & 0x10));
        
       // SBC Rd, Rr (0001 10rd dddd rrrr) - bits 15-12 = 0001
        else if ((instruction.op16 & 0xFC00) == 0x1800)
            printf("SBC r%d, r%d\n", (instruction.op16 >> 4) & 0x1F, (instruction.op16 & 0x0F) | ((instruction.op16 >> 5) & 0x10));
        
        // SBC Rd, Rr (0000 10rd dddd rrrr) - bits 15-12 = 0000, bits 11-10 = 10
        else if ((instruction.op16 & 0xFC00) == 0x0800)
            printf("SBC r%d, r%d\n", (instruction.op16 >> 4) & 0x1F, (instruction.op16 & 0x0F) | ((instruction.op16 >> 5) & 0x10));
        
        // ADC Rd, Rr (0001 11rd dddd rrrr) - bits 15-12 = 0001
        else if ((instruction.op16 & 0xFC00) == 0x1C00)
            printf("ADC r%d, r%d\n", (instruction.op16 >> 4) & 0x1F, (instruction.op16 & 0x0F) | ((instruction.op16 >> 5) & 0x10));

        // SUBI Rd, K (0101 KKKK dddd KKKK)
        else if ((instruction.op16 & 0xF000) == 0x5000)
            printf("SUBI r%d, 0x%X\n", 16 + instruction.type3.d4, (instruction.type3.op8 & 0x0F) << 4 | instruction.type3.K4);
        
        // ORI Rd, K (0110 KKKK dddd KKKK)
        else if ((instruction.op16 & 0xF000) == 0x6000)
            printf("ORI r%d, 0x%X\n", 16 + instruction.type3.d4, (instruction.type3.op8 & 0x0F) << 4 | instruction.type3.K4);
        
        // MOV Rd, Rr (0010 11rd dddd rrrr)
        else if ((instruction.op16 & 0xFC00) == 0x2C00)
            printf("MOV r%d, r%d\n", instruction.type2.d5, instruction.type2.r4 | (instruction.type2.r1 << 4));
        
        // LDS Rd, k (1001 000d dddd 0000) - 32 bits
        else if ((instruction.op16 & 0xFE0F) == 0x9000)
            printf("LDS r%d, 0x?????\n", (instruction.op16 >> 4) & 0x1F);
        
        // DEC Rd (1001 010d dddd 1010)
        else if ((instruction.op16 & 0xFE0F) == 0x940A)
            printf("DEC r%d\n", (instruction.op16 >> 4) & 0x1F);
        
        // INC Rd (1001 010d dddd 0011)
        else if ((instruction.op16 & 0xFE0F) == 0x9403)
            printf("INC r%d\n", (instruction.op16 >> 4) & 0x1F);
        
        // RET (1001 0101 0000 1000)
        else if (instruction.op16 == 0x9508)
            printf("RET\n");
        
        // NOP (0000 0000 0000 0000)
        else if (instruction.op16 == 0x0000)
            printf("NOP\n");
        
        else
            printf("UNKNOWN (0x%04X)\n", instruction.op16);
        
        inst_count++;
    }
    
    printf("\nTotal instructions: %d\n", inst_count);
    return 0;
}