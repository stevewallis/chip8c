

//CPU tick
void C8_tick();

// Lookup tables
void C8_Lookup_0xxx();
void C8_Lookup_8xxx();
void C8_Lookup_Exxx();
void C8_Lookup_Fxxx();

// OP codes
void C8_OP_Invalid();
void C8_OP_00EE(); // RET
void C8_OP_1nnn(); // JMP
void C8_OP_2nnn(); // CALL
void C8_OP_3xnn(); // SE x nn
void C8_OP_4xnn(); // SNE x nn
void C8_OP_5xy0(); // SE x y
void C8_OP_6xnn(); // MOV x nn
void C8_OP_7xnn(); // ADD x nn
void C8_OP_8xy0(); // MOV x y
void C8_OP_8xy1(); // OR x y
void C8_OP_8xy2(); // AND x y
void C8_OP_8xy3(); // XOR x y
void C8_OP_8xy4(); // ADD x y
void C8_OP_8xy5(); // SUB x y
void C8_OP_8xy6(); // SHR x
void C8_OP_8xy7(); // SUBN x y
void C8_OP_8xyE(); // SHL x
void C8_OP_9xy0(); // SNE x y
void C8_OP_Annn(); // MOV I
void C8_OP_Bnnn(); // JMP V0
void C8_OP_Cxnn(); // MOV RND x
void C8_OP_Dxyn(); // DRW x y n
void C8_OP_Ex9E(); // SKP
void C8_OP_ExA1(); // SKNP
void C8_OP_Fx07(); // MOV DT x
void C8_OP_Fx0A(); // WAIT KEY
void C8_OP_Fx15(); // MOV x DT
void C8_OP_Fx18(); // MOV x ST
void C8_OP_Fx1E(); // ADD I x
void C8_OP_Fx29(); // LD sprite f
void C8_OP_Fx33(); // LD b
void C8_OP_Fx55(); // LD MEM
void C8_OP_Fx65(); // LD MEM

