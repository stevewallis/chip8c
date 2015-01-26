#include "chip8.h"
#include "chip8_fontset.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#define Vx V[(op&0x0f00)>>8]
#define Vy V[(op&0x00f0)>>4]

uint8_t memory[0xfff];
uint8_t V[16]; //data registers
uint16_t I; //index register
uint16_t PC; //program counter;
uint16_t op; //current opcode;

uint8_t vmem[256]; //64 x 32
uint8_t timer_delay, timer_sound;

uint16_t stack[16] ,sp;

uint8_t keyboard[16];

uint8_t killFlag = 0;
uint8_t drawFlag = 0;

void init() {
    I = 0, sp = 0;
	timer_delay = 0, timer_sound = 0;
	PC = 0x200; //program always starts here in memory
    op = 0;
    memcpy(&memory[C8_FONTSET_BASE], chip8_fontset, C8_FONTSET_SIZE);
}

void C8_OP_Invalid(){ killFlag = 1; printf("%04x : INVALID OP CODE!\n",op);}

void C8_Lookup_0xxx(){
    if (op&0x0f00) {

    }  else {
        if ((op&0x00ff) == 0xee) {C8_OP_00EE();}
    }
    if (op == 0x0000) C8_OP_Invalid();
}

void C8_Lookup_8xxx(){
    static void (*C8_ARITH_OPTable[16])(void) = {
        C8_OP_8xy0, C8_OP_8xy1, C8_OP_8xy2, C8_OP_8xy3,
        C8_OP_8xy4, C8_OP_8xy5, C8_OP_8xy6, C8_OP_8xy7,
        C8_OP_Invalid, C8_OP_Invalid, C8_OP_Invalid, C8_OP_Invalid, 
        C8_OP_Invalid, C8_OP_Invalid, C8_OP_8xyE, C8_OP_Invalid};

    C8_ARITH_OPTable[(op&0x000f)]();
}

void C8_Lookup_Exxx(){
  if ((op&0x00ff) == 0x9E) 
		C8_OP_Ex9E();
  else 
		C8_OP_ExA1();
}

void C8_Lookup_Fxxx(){
    switch(op&0x00ff) {
        case 0x07:
            C8_OP_Fx07();
            break;
        case 0x0A:
            C8_OP_Fx0A();
            break;
        case 0x15:
            C8_OP_Fx15();
            break;
        case 0x18:
            C8_OP_Fx18();
            break;
        case 0x1E:
            C8_OP_Fx1E();
            break;
        case 0x29:
            C8_OP_Fx29();
            break;
        case 0x33:
            C8_OP_Fx33();
            break;
        case 0x55:
            C8_OP_Fx55();
            break;
        case 0x65:
            C8_OP_Fx65();
            break;
        default:
            C8_OP_Invalid();
    }
}

void C8_OP_00EE() { // RET
	PC = stack[sp]; 
	sp--; 
}

void C8_OP_1nnn(){ // JMP
	PC = op&0x0fff; 
}

void C8_OP_2nnn(){ // CALL
	sp++; 
	stack[sp] = PC; 
	PC = op&0x0fff; 
}

void C8_OP_3xnn(){ //SE x nn
	if (V[(op&0x0f00)>>8] == (op&0x00ff)) 
		PC+=2; 
}

void C8_OP_4xnn(){ // SNE x nn
	if (V[(op&0x0f00)>>8] != (op&0x00ff)) 
		PC+=2;
}

void C8_OP_5xy0(){ // SE x y
	if (V[(op&0x0f00)>>8] == V[(op&0x00f0)>>4]) 
		PC+=2;
}

void C8_OP_6xnn(){ //MOV x nn
	V[(op&0x0f00)>>8] = (op&0x00ff);
}

void C8_OP_7xnn(){ // ADD x nn
	V[(op&0x0f00)>>8] += (op&0x00ff);
}

void C8_OP_8xy0() { // MOV x y
	V[(op&0x0f00)>>8] = V[(op&0x00f0)>>4];
}

void C8_OP_8xy1() { // OR x y
	V[(op&0x0f00)>>8] = V[(op&0x0f00)>>8]|V[(op&0x00f0)>>4];
}

void C8_OP_8xy2() { // AND x y
	V[(op&0x0f00)>>8] = V[(op&0x0f00)>>8]&V[(op&0x00f0)>>4];
}

void C8_OP_8xy3() { // XOR x y
	V[(op&0x0f00)>>8] = V[(op&0x0f00)>>8]^V[(op&0x00f0)>>4];
}

void C8_OP_8xy4() { // ADD x y 
    uint16_t sum = V[(op&0x0f00)>>8]+V[(op&0x00f0)>>4];
    V[(op&0x0f00)>>8] = (sum&0xff);
    V[0xf] = ((sum&0xff00)>>8)?1:0;
}

void C8_OP_8xy5() { // SUB x y
    V[0xf] = (V[(op&0x0f00)>>8] > V[(op&0x00f0)>>4]);
    V[(op&0x0f00)>>8] -= V[(op&0x00f0)>>4];
}

void C8_OP_8xy6() { // SHR x
    V[0xf] = (V[(op&0x0f00)>>8]&0x1);
    V[(op&0x0f00)>>8]>>=1; 
}

void C8_OP_8xy7() { // SUBN x y
    V[0xf] = (V[(op&0x0f00)>>8] < V[(op&0x00f0)>>4]);
    V[(op&0x0f00)>>8] = V[(op&0x00f0)>>4] - V[(op&0x0f00)>>8];
}

void C8_OP_8xyE() { // SHL x
    V[0xf] = (V[(op&0x0f00)>>8]>>7);
    V[(op&0x0f00)>>8]<<=1; 
}

void C8_OP_9xy0() { // SNE x y
	if (V[(op&0x0f00)>>8] != V[(op&0x00f0)>>4]) PC+=2;
}
	
void C8_OP_Annn() { // MOV I
	I = op&0x0fff; 
}

void C8_OP_Bnnn() { // JMP V0
	PC = op&0x0fff + V[0]; 
}

void C8_OP_Cxnn() { // RND
    V[(op&0x0f00)>>8] = (rand() % 0xff) & (op&0x00ff);
}

void C8_OP_Dxyn() { // DRW x y n
	//printf("%04x : DRW NOT DONE YET\n",op);
    uint8_t n,x,y;
    n = op&0xf; //number of bytes (ie height of sprite).
    x = V[(op&0x0f00)>>8];
    y = V[(op&0x0f00)>>4];
    V[0xf] = 0;

    for(int i=0; i<n; i++) {
        //vmem[] = 
    }
}

void C8_OP_Ex9E() { // SKP
	if (keyboard[V[(op&0x0f00)>>8]]) PC+=2; 
}

void C8_OP_ExA1() { // SKNP
	if (!keyboard[V[(op&0x0f00)>>8]]) PC+=2; 
}

void C8_OP_Fx07() { // MOV DT x
	V[(op&0x0f00)>>8] = timer_delay; 
}

void C8_OP_Fx0A() { // WAITKEY
	printf("%04x : WAITKEY NOT DONE YET\n",op);
}

void C8_OP_Fx15() { // MOV x DT 
	timer_delay = V[(op&0x0f00)>>8];
}

void C8_OP_Fx18() { //MOV x ST
	timer_sound = V[(op&0x0f00)>>8];
}

void C8_OP_Fx1E() { // MOV ADD I
	I += V[(op&0x0f00)>>8]; 
}

void C8_OP_Fx29() { // LD F
    I = memory[C8_FONTSET_BASE + ((V[(op&0x0f00)>>8]&0xf) * C8_FONTSET_CHAR_SIZE)];
}

void C8_OP_Fx33() { // LD B
	printf("%04x : LD B NOT DONE YET\n",op);
}

void C8_OP_Fx55() { // LD MEM[I] V0Vx
	uint8_t x = (op&0x0f00)>>8;
    for (uint8_t i=0;i<=x;i++) {
        memory[I+i] = V[i]; 
    }
}

void C8_OP_Fx65() { // LD V0Vx MEM[I]
	uint8_t x = (op&0x0f00)>>8;
    for (uint8_t i=0;i<=x;i++) {
        V[i] = memory[I+i]; 
    }
}

void C8_tick() {
    static void (*C8_OPTable[17])(void) = {
        C8_Lookup_0xxx, C8_OP_1nnn, C8_OP_2nnn, C8_OP_3xnn,
        C8_OP_4xnn, C8_OP_5xy0, C8_OP_6xnn, C8_OP_7xnn,
        C8_Lookup_8xxx, C8_OP_9xy0, C8_OP_Annn, C8_OP_Bnnn,
        C8_OP_Cxnn, C8_OP_Dxyn, C8_Lookup_Exxx, C8_Lookup_Fxxx,
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
	
	int W = 640;
	int H = 320;
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
	SDL_Surface* surface = SDL_GetWindowSurface(win);
	
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));
    SDL_UpdateWindowSurface(win);

    //SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	//SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	/**/
	SDL_Event e;	
    for(;;) {
        while(SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    killFlag = 1;
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_1: keyboard[0x0]=1; break;
                        case SDLK_2: keyboard[0x1]=1; break; 
                        case SDLK_3: keyboard[0x2]=1; break; 
                        case SDLK_4: keyboard[0x3]=1; break; 
                        case SDLK_q: keyboard[0x4]=1; break; 
                        case SDLK_w: keyboard[0x5]=1; break; 
                        case SDLK_e: keyboard[0x6]=1; break; 
                        case SDLK_r: keyboard[0x7]=1; break; 
                        case SDLK_a: keyboard[0x8]=1; break; 
                        case SDLK_s: keyboard[0x9]=1; break; 
                        case SDLK_d: keyboard[0xa]=1; break; 
                        case SDLK_f: keyboard[0xb]=1; break; 
                        case SDLK_z: keyboard[0xc]=1; break; 
                        case SDLK_x: keyboard[0xd]=1; break; 
                        case SDLK_c: keyboard[0xe]=1; break; 
                        case SDLK_v: keyboard[0xf]=1; break;
                        default: break; 
                    } 
                case SDL_KEYUP:
                    switch(e.key.keysym.sym) {
                        case SDLK_1: keyboard[0x0]=0; break;
                        case SDLK_2: keyboard[0x1]=0; break; 
                        case SDLK_3: keyboard[0x2]=0; break; 
                        case SDLK_4: keyboard[0x3]=0; break; 
                        case SDLK_q: keyboard[0x4]=0; break; 
                        case SDLK_w: keyboard[0x5]=0; break; 
                        case SDLK_e: keyboard[0x6]=0; break; 
                        case SDLK_r: keyboard[0x7]=0; break; 
                        case SDLK_a: keyboard[0x8]=0; break; 
                        case SDLK_s: keyboard[0x9]=0; break; 
                        case SDLK_d: keyboard[0xa]=0; break; 
                        case SDLK_f: keyboard[0xb]=0; break; 
                        case SDLK_z: keyboard[0xc]=0; break; 
                        case SDLK_x: keyboard[0xd]=0; break; 
                        case SDLK_c: keyboard[0xe]=0; break; 
                        case SDLK_v: keyboard[0xf]=0; break;
                        default: break; 
                    } 
                    break;          
                default:
                    break;
            }
        }

	    C8_tick();
        if (drawFlag) {

            drawFlag = 0;
        }
        if (killFlag) break;
        //SDL_Delay(10);
    }


    SDL_FreeSurface(surface);
    SDL_DestroyWindow(win);
    SDL_Quit();
	
	return 0;
}
