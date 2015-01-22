#include <stdio.h>
#include <stdint.h>

#define Vx V[(op&0x0f00)>>8]
#define Vy V[(op&0x00f0)>>4]

uint8_t memory[0xfff];
uint8_t V[16]; //data registers
uint16_t I; //index register
uint16_t PC; //program counter;
uint16_t op; //current opcode;

uint8_t display[64*32];
uint8_t timer_delay, timer_sound;

uint16_t stack[16],sp;

uint8_t keyboard[16];

uint8_t killFlag = 0;

void init() {
    I = 0, sp = 0;
	timer_delay = 0, timer_sound = 0;
	PC = 0x200; //program always starts here in memory
    op = 0;
}

void C8_OP_Invalid(){ killFlag = 1; printf("%04x : INVALID OP CODE!\n",op);}
void C8_OP_RET_00EE() { PC = stack[sp]; sp--; }
void C8_SYS_Lookup_0xxx(){
    if (op&0x0f00) {

    }  else {
        if ((op&0x00ff) == 0xee) {C8_OP_RET_00EE();}
    }
    if (op == 0x0000) C8_OP_Invalid();
}



void C8_OP_JMP_1nnn(){ PC = op&0x0fff; }
void C8_OP_CALL_2nnn(){ sp++; stack[sp] = PC; PC = op&0x0fff; }
void C8_OP_SE_3xnn(){ if (V[(op&0x0f00)>>8] == (op&0x00ff)) PC+=2; }
void C8_OP_SNE_4xnn(){ if (V[(op&0x0f00)>>8] != (op&0x00ff)) PC+=2;}
void C8_OP_SE_5xy0(){ if (V[(op&0x0f00)>>8] == V[(op&0x00f0)>>4]) PC+=2;}
void C8_OP_MOV_6xnn(){ V[(op&0x0f00)>>8] = (op&0x00ff);}
void C8_OP_ADD_7xnn(){ V[(op&0x0f00)>>8] += (op&0x00ff);}

void C8_OP_MOV_8xy0() { V[(op&0x0f00)>>8] = V[(op&0x00f0)>>4];}
void C8_OP_OR_8xy1() { V[(op&0x0f00)>>8] = V[(op&0x0f00)>>8]|V[(op&0x00f0)>>4];}
void C8_OP_AND_8xy2() { V[(op&0x0f00)>>8] = V[(op&0x0f00)>>8]&V[(op&0x00f0)>>4];}
void C8_OP_XOR_8xy3() { V[(op&0x0f00)>>8] = V[(op&0x0f00)>>8]^V[(op&0x00f0)>>4];}
void C8_OP_ADD_8xy4() { 
    uint16_t sum = V[(op&0x0f00)>>8]+V[(op&0x00f0)>>4];
    V[(op&0x0f00)>>8] = (sum&0xff);
    V[0xf] = ((sum&0xff00)>>8)?1:0;
}
void C8_OP_SUB_8xy5() {
    V[0xf] = (V[(op&0x0f00)>>8] > V[(op&0x00f0)>>4]);
    V[(op&0x0f00)>>8] -= V[(op&0x00f0)>>4];
}
void C8_OP_SHR_8xy6() {
    V[0xf] = (V[(op&0x0f00)>>8]&0x1);
    V[(op&0x0f00)>>8]>>=1; 
}
void C8_OP_SUBN_8xy7() {
    V[0xf] = (V[(op&0x0f00)>>8] < V[(op&0x00f0)>>4]);
    V[(op&0x0f00)>>8] = V[(op&0x00f0)>>4] - V[(op&0x0f00)>>8];
}
void C8_OP_SHL_8xyE() {
    V[0xf] = (V[(op&0x0f00)>>8]>>7);
    V[(op&0x0f00)>>8]<<=1; 
}

void C8_ARITH_Lookup_8xxx(){
    static void (*C8_ARITH_OPTable[16])(void) = {
        C8_OP_MOV_8xy0, C8_OP_OR_8xy1, C8_OP_AND_8xy2, C8_OP_XOR_8xy3,
        C8_OP_ADD_8xy4, C8_OP_SUB_8xy5, C8_OP_SHR_8xy6, C8_OP_SUBN_8xy7,
        C8_OP_Invalid, C8_OP_Invalid, C8_OP_Invalid, C8_OP_Invalid, 
        C8_OP_Invalid, C8_OP_Invalid, C8_OP_SHL_8xyE, C8_OP_Invalid};

    C8_ARITH_OPTable[(op&0x000f)]();
}
void C8_OP_SNE_9xy0(){ if (V[(op&0x0f00)>>8] != V[(op&0x00f0)>>4]) PC+=2;}
void C8_OP_MOV_I_Annn(){ I = op&0x0fff; }
void C8_OP_JP_V0_Bnnn(){ PC = op&0x0fff + V[0]; }
void C8_OP_RND_Cxnn(){printf("%04x : RND NOT DONE YET\n",op);}
void C8_OP_DRW_Dxyn(){printf("%04x : DRW NOT DONE YET\n",op);}
void C8_OP_SKP_Ex9E(){ if (keyboard[V[(op&0x0f00)>>8]]) PC+=2; }
void C8_OP_SKNP_ExA1(){ if (!keyboard[V[(op&0x0f00)>>8]]) PC+=2; }  
void C8_KP_Lookup_Exxx(){
    if ((op&0x00ff) == 0x9E) C8_OP_SKP_Ex9E();
    else C8_OP_SKNP_ExA1();
}

void C8_OP_MOVDTX_Fx07() {}
void C8_OP_WAITKEY_Fx0A() {}
void C8_OP_MOVXDT_Fx15() {}
void C8_OP_MOVXST_Fx18() {}
void C8_OP_ADDI_Fx1E() {}
void C8_OP_LD_F_Fx29() {}
void C8_OP_LD_B_Fx33() {}
void C8_OP_LD_MEM_Fx55() {}
void C8_OP_LD_MEM_Fx65() {}
void C8_EXT_Lookup_Fxxx(){
    switch(op&0x00ff) {
        case 0x07:
            C8_OP_MOVDTX_Fx07();
            break;
        case 0x0A:
            C8_OP_WAITKEY_Fx0A();
            break;
        case 0x15:
            C8_OP_MOVXDT_Fx15();
            break;
        case 0x18:
            C8_OP_MOVXST_Fx18();
            break;
        case 0x1E:
            C8_OP_ADDI_Fx1E();
            break;
        case 0x29:
            C8_OP_LD_F_Fx29();
            break;
        case 0x33:
            C8_OP_LD_B_Fx33();
            break;
        case 0x55:
            C8_OP_LD_MEM_Fx55();
            break;
        case 0x65:
            C8_OP_LD_MEM_Fx65();
            break;
        default:
            C8_OP_Invalid();
    }
}

void cycle() {
    static void (*C8_OPTable[17])(void) = {
        C8_SYS_Lookup_0xxx, C8_OP_JMP_1nnn, C8_OP_CALL_2nnn, C8_OP_SE_3xnn,
        C8_OP_SNE_4xnn, C8_OP_SE_5xy0, C8_OP_MOV_6xnn, C8_OP_ADD_7xnn,
        C8_ARITH_Lookup_8xxx, C8_OP_SNE_9xy0, C8_OP_MOV_I_Annn, C8_OP_JP_V0_Bnnn,
        C8_OP_RND_Cxnn, C8_OP_DRW_Dxyn, C8_KP_Lookup_Exxx, C8_EXT_Lookup_Fxxx,
        C8_OP_Invalid};

	op = memory[PC] << 8 | memory[PC + 1];
    PC+=2;
	C8_OPTable[(op&0xf000)>>12]();
}

int load(char *file) {
    FILE *fp = fopen(file, "rb");
    if (!fp) return 1;

    fread(memory+0x200, sizeof(uint8_t), 0xdff, fp);
    fclose(fp);

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("NO ROM SPECIFIED\n");
        return 1;
    }
    init();	
	load(argv[1]);

    for(;;) {
	    cycle();
        if (killFlag) break;
    }
	
	return 0;
}
