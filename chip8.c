#include <stdio.h>
#include <stdint.h>

uint8_t memory[0xfff];
uint8_t V[16]; //data registers
uint16_t I; //index register
uint16_t PC; //program counter;

uint8_t display[64*32];
uint8_t timer_delay, timer_sound;

uint16_t stack[16],sp;

uint8_t keyboard[16];

uint8_t killFlag = 0;
/*
void (*C8_Table[16])(void) = {C8_SYSTEM, C8_JP_nnn, C8_CALL_nnn, C8_SE_x_nn,
                              C8_SNE_x_nn, C8_SE_x_y, C8_LD_x_nn, C8_ADD_x_nn,
                              C8_ARITHMETIC, C8_SNE_x_y, };
*/
void init() {
    I = 0, sp = 0;
	timer_delay = 0, timer_sound = 0;
	PC = 0x200; //program always starts here in memory
}

void cycle() {
	uint16_t op = memory[PC] << 8 | memory[PC + 1];
	
    
    
    printf("%04x ",op);
    PC+=2;
    if (PC >= 0xfff) { killFlag = 1; }

    switch(op&0xf000) {
		case 0x0000:
		break;

	}	
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
