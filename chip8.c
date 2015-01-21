#include <stdio.h>
#include <stdint.h>

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
void C8_OP_RET_00EE() { PC = stack[sp]; sp--; printf("%04x : RET\n",op);}
void C8_SYS_Lookup_0xxx(){
    if (op&0x0f00) {

    }  else {
        if ((op&0x00ff) == 0xee) {C8_OP_RET_00EE();}
    }
    if (op == 0x0000) C8_OP_Invalid();
}



void C8_OP_JP_1nnn(){ PC = op&0x0fff; printf("%04x : JMP\n",op);}
void C8_OP_CALL_2nnn(){ sp++; stack[sp] = PC; PC = op&0x0fff; printf("%04x : CALL\n",op);}
void C8_OP_SE_3xnn(){ if (V[(op&0x0f00)>>8] == (op&0x00ff)) PC+=2; printf("%04x : SExnn\n",op);}
void C8_OP_SNE_4xnn(){ if (V[(op&0x0f00)>>8] != (op&0x00ff)) PC+=2; printf("%04x : SNExnn\n",op);}
void C8_OP_SE_5xy0(){ if (V[(op&0x0f00)>>8] == V[(op&0x00f0)>>4]) PC+=2; printf("%04x : SExy\n",op);}
void C8_OP_LD_6xnn(){ V[(op&0x0f00)>>8] = (op&0x00ff); printf("%04x : LDxnn\n",op);}
void C8_OP_ADD_7xnn(){ V[(op&0x0f00)>>8] += (op&0x00ff); printf("%04x : ADDxnn\n",op);}
void C8_ARITH_Lookup_8xxx(){printf("%04x : ARITH NOT DONE YET\n",op);}
void C8_OP_SNE_9xy0(){ if (V[(op&0x0f00)>>8] != V[(op&0x00f0)>>4]) PC+=2; printf("%04x : SNExy\n",op);}
void C8_OP_LD_I_Annn(){ I = op&0x0fff; printf("%04x : LDI\n",op);}
void C8_OP_JP_V0_Bnnn(){ PC = op&0x0fff + V[0]; printf("%04x : JPV0\n",op);}
void C8_OP_RND_Cxnn(){printf("%04x : RND NOT DONE YET\n",op);}
void C8_OP_DRW_Dxyn(){printf("%04x : DRW NOT DONE YET\n",op);}
void C8_SKIP_Lookup_Exxx(){printf("%04x : SKIP NOT DONE YET\n",op);}
void C8_EXT_Lookup_Fxxx(){printf("%04x : EXT NOT DONE YET\n",op);}

void cycle() {
    static void (*C8_OPTable[17])(void) = {C8_SYS_Lookup_0xxx, C8_OP_JP_1nnn, C8_OP_CALL_2nnn, C8_OP_SE_3xnn,
                                    C8_OP_SNE_4xnn, C8_OP_SE_5xy0, C8_OP_LD_6xnn, C8_OP_ADD_7xnn,
                                    C8_ARITH_Lookup_8xxx, C8_OP_SNE_9xy0, C8_OP_LD_I_Annn, C8_OP_JP_V0_Bnnn,
                                    C8_OP_RND_Cxnn, C8_OP_DRW_Dxyn, C8_SKIP_Lookup_Exxx, C8_EXT_Lookup_Fxxx,
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
