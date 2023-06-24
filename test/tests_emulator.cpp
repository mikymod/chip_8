#include "chip-8.h"

#define CLOVE_SUITE_NAME Emulator
#include "clove-unit.h"

using namespace chipotto;

void pumpEvent()
{
    SDL_Event e;
    e.type = SDL_KEYDOWN;
    e.key.type = SDL_KEYDOWN;
    e.key.timestamp = 0;
    e.key.windowID = 0;
    e.key.state = SDL_PRESSED;
    e.key.repeat = 0;
    e.key.keysym.scancode = SDL_SCANCODE_W;
    e.key.keysym.sym = SDLK_w;
    e.key.keysym.mod = KMOD_NONE;
    SDL_PushEvent(&e);
}

CLOVE_TEST(LoadFromBuffer)
{
    Emulator emulator;
    uint16_t opcodes[1] = {0xE000};
    emulator.LoadFromBuffer(opcodes, 1);
    CLOVE_INT_EQ(0x00E0, emulator.GetCurrentOpcode())
}

CLOVE_TEST(Opcode0_CLS)
{
    Emulator emulator;
    uint16_t opcodes[1] = {0xE000};
    emulator.LoadFromBuffer(opcodes, 1);
    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);

    int* textureBlack = reinterpret_cast<int*>(SDL_calloc(emulator.GetHeight() * emulator.GetWidth(), sizeof(int)));
    int* texture = nullptr;
    int pitch;
    SDL_LockTexture(emulator.GetTexture(), nullptr, reinterpret_cast<void**>(&texture), &pitch);
    CLOVE_INT_EQ(SDL_memcmp(texture, textureBlack, emulator.GetHeight() * emulator.GetWidth() * sizeof(int)), 0);
    SDL_UnlockTexture(emulator.GetTexture());
}

CLOVE_TEST(Opcode0_RET)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x0222, 0xE000, 0xEE00 };
    emulator.LoadFromBuffer(opcodes, 3);

    uint16_t previousPC = emulator.GetPC();
    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC, emulator.GetStackTop());
    CLOVE_INT_EQ(0x202, emulator.GetPC());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(emulator.GetPC(), emulator.GetStackTop() + sizeof(uint16_t));
}

CLOVE_TEST(Opcode1_JUMP_addr)
{
    Emulator emulator;
    uint16_t opcodes[1] = {0x2211};
    emulator.LoadFromBuffer(opcodes, 1);
    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(emulator.GetPC(), 0x122);
}

CLOVE_TEST(Opcode2_CALL_addr)
{
    Emulator emulator;
    uint16_t opcodes[3] = {0x0222, 0xE000, 0xEE00};
    emulator.LoadFromBuffer(opcodes, 3);

    uint16_t previousPC = emulator.GetPC();
    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC, emulator.GetStackTop());
    CLOVE_INT_EQ(0x202, emulator.GetPC());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
}

CLOVE_TEST(Opcode3_SE_Vx_byte)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x5561, 0x5531, 0xE000 };
    emulator.LoadFromBuffer(opcodes, 3);
    
    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x55, emulator.GetRegisterValue(1));

    uint16_t previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + 2 * sizeof(uint16_t), emulator.GetPC());
}

CLOVE_TEST(Opcode4_SNE_Vx_byte)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x5561, 0x5441, 0xE000 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x55, emulator.GetRegisterValue(1));

    uint16_t previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + 2 * sizeof(uint16_t), emulator.GetPC());
}

CLOVE_TEST(Opcode5_SE_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[6] = { 0x5561, 0x5562, 0x2051, 0xE000, 0x5462, 0x2051 };
    emulator.LoadFromBuffer(opcodes, 6);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    emulator.Tick();
    CLOVE_IS_TRUE(success);

    uint16_t previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + 2 * sizeof(uint16_t), emulator.GetPC());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x54, emulator.GetRegisterValue(2));

    previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + sizeof(uint16_t), emulator.GetPC());
}

CLOVE_TEST(Opcode6_LD_Vx_byte)
{
    Emulator emulator;
    uint16_t opcodes[1] = { 0x5561 };
    emulator.LoadFromBuffer(opcodes, 1);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x55, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode7_ADD_Vx_byte)
{
    Emulator emulator;
    uint16_t opcodes[2] = { 0x5561, 0x171 };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x55, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x56, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode8_LD_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x5561, 0x4462, 0x2081 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x55, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x44, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(emulator.GetRegisterValue(1), emulator.GetRegisterValue(2));
}

CLOVE_TEST(Opcode8_OR_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0xA61, 0x562, 0x2181 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xA, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x5, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xF, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode8_AND_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0xA61, 0x562, 0x2281 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xA, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x5, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode8_XOR_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0xA61, 0x562, 0x2381 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xA, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x5, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xf, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode8_ADD_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x461, 0x462, 0x2481 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x4, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x4, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x8, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode8_SUB_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x461, 0x462, 0x2581 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x4, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x4, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode8_SHR_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[4] = { 0xff61, 0x2681, 0x7e61, 0x2681 };
    emulator.LoadFromBuffer(opcodes, 4);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xff, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x1, emulator.GetRegisterValue(0xf));
    CLOVE_INT_EQ(0x7f, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x7e, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(0xf));
    CLOVE_INT_EQ(0x3f, emulator.GetRegisterValue(1));
}

CLOVE_TEST(Opcode8_SUBN_Vx_Vy_NoBorrow)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x561, 0x462, 0x2781 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x5, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x4, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(0xf));
}

CLOVE_TEST(Opcode8_SUBN_Vx_Vy_Borrow)
{
    Emulator emulator;
    uint16_t opcodes[3] = { 0x461, 0x562, 0x2781 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x4, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x5, emulator.GetRegisterValue(2));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x1, emulator.GetRegisterValue(0xf));
}

CLOVE_TEST(Opcode8_SHL_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[4] = { 0x0f61, 0x2e81, 0x8061, 0x2e81 };
    emulator.LoadFromBuffer(opcodes, 4);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0f, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(0xf));
    CLOVE_INT_EQ(0x1e, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x80, emulator.GetRegisterValue(1));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x1, emulator.GetRegisterValue(0xf));
    CLOVE_INT_EQ(0x00, emulator.GetRegisterValue(1));  // Overflow
}

CLOVE_TEST(Opcode9_SNE_Vx_Vy)
{
    Emulator emulator;
    uint16_t opcodes[6] = { 0x5561, 0x5462, 0x2091, 0xE000, 0x5562, 0x2091 };
    emulator.LoadFromBuffer(opcodes, 6);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    emulator.Tick();
    CLOVE_IS_TRUE(success);

    uint16_t previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + 2 * sizeof(uint16_t), emulator.GetPC());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x55, emulator.GetRegisterValue(2));

    previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + sizeof(uint16_t), emulator.GetPC());
}

CLOVE_TEST(OpcodeA_LD_I_addr)
{
    Emulator emulator;
    uint16_t opcodes[2] = { 0x11A1, 0xFFAF };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x111, emulator.GetI());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xFFF, emulator.GetI());
}

CLOVE_TEST(OpcodeB_JP_V0_addr)
{
    Emulator emulator;
    uint16_t opcodes[2] = { 0x160, 0x11B1 };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x1, emulator.GetRegisterValue(0));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x112, emulator.GetPC());
}

CLOVE_TEST(OpcodeC_RND_Vx_byte)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x060, 0xffc0 };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(0));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_NE(0x0, emulator.GetRegisterValue(0));
}

CLOVE_TEST(OpcodeD_DRW_Vx_Vy_nibble)
{
    // TODO
}

CLOVE_TEST(OpcodeE_SKP_Vx)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x560, 0x9ee0, 0xe000, 0x1261 };
    emulator.LoadFromBuffer(opcodes, 4);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x5, emulator.GetRegisterValue(0));

    pumpEvent();

    uint16_t previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + 2 * sizeof(uint16_t), emulator.GetPC());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x12, emulator.GetRegisterValue(1));
}

CLOVE_TEST(OpcodeE_SKNP_Vx)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x660, 0x9ee0, 0xe000, 0x1261 };
    emulator.LoadFromBuffer(opcodes, 4);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x6, emulator.GetRegisterValue(0));

    pumpEvent();

    uint16_t previousPC = emulator.GetPC();
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(previousPC + 2 * sizeof(uint16_t), emulator.GetPC());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x12, emulator.GetRegisterValue(1));
}

CLOVE_TEST(OpcodeF_LD_Vx_DT)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0xff60, 0x07f0 };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xff, emulator.GetRegisterValue(0));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(emulator.GetDelayTimer(), emulator.GetRegisterValue(0));
}

CLOVE_TEST(OpcodeF_LD_Vx_K)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x060, 0x0AF0, 0xff61 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(0));

    pumpEvent();

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x5, emulator.GetRegisterValue(0));
    CLOVE_INT_EQ(0xff, emulator.GetRegisterValue(1));
}

CLOVE_TEST(OpcodeF_LD_DT_VX)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0xff60, 0x15f0 };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xff, emulator.GetRegisterValue(0));
    CLOVE_INT_NE(emulator.GetRegisterValue(0), emulator.GetDelayTimer());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(emulator.GetRegisterValue(0), emulator.GetDelayTimer());
}

CLOVE_TEST(OpcodeF_LD_ST_VX)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0xff60, 0x18f0 };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0xff, emulator.GetRegisterValue(0));
    CLOVE_INT_NE(emulator.GetRegisterValue(0), emulator.GetSoundTimer());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(emulator.GetRegisterValue(0), emulator.GetSoundTimer());
}

CLOVE_TEST(OpcodeF_ADD_I_VX)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x160, 0x11a1, 0x1ef0 };
    emulator.LoadFromBuffer(opcodes, 3);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x1, emulator.GetRegisterValue(0));

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x111, emulator.GetI());

    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0X112, emulator.GetI());
}

CLOVE_TEST(OpcodeF_LD_F_VX)
{
    // TODO
}

CLOVE_TEST(OpcodeF_LD_B_VX)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x8060, 0x33f0 };
    emulator.LoadFromBuffer(opcodes, 2);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x80, emulator.GetRegisterValue(0));

    uint16_t index = emulator.GetI(); // Init at 0x0
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x1, emulator.GetMemoryLocValue(index));
    CLOVE_INT_EQ(0x2, emulator.GetMemoryLocValue(index + 1));
    CLOVE_INT_EQ(0x8, emulator.GetMemoryLocValue(index + 2));
}

CLOVE_TEST(OpcodeF_LD_locI_VX)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x160, 0x261, 0x362, 0x463, 0x55f3 };
    emulator.LoadFromBuffer(opcodes, 5);

    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);

    uint16_t index = emulator.GetI(); // Init at 0x0
    CLOVE_INT_EQ(0x1, emulator.GetMemoryLocValue(index));
    CLOVE_INT_EQ(0x2, emulator.GetMemoryLocValue(index + 1));
    CLOVE_INT_EQ(0x3, emulator.GetMemoryLocValue(index + 2));
    CLOVE_INT_EQ(0x4, emulator.GetMemoryLocValue(index + 3));
}

CLOVE_TEST(OpcodeF_LD_Vx_locI)
{
    Emulator emulator;
    uint16_t opcodes[] = { 0x160, 0x261, 0x362, 0x463, 0x55f3, 0x060, 0x061, 0x062, 0x063, 0x65f3 };
    emulator.LoadFromBuffer(opcodes, 10);

    // Writes values from register to memory
    bool success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);

    uint16_t index = emulator.GetI(); // Init at 0x0
    CLOVE_INT_EQ(0x1, emulator.GetMemoryLocValue(index));
    CLOVE_INT_EQ(0x2, emulator.GetMemoryLocValue(index + 1));
    CLOVE_INT_EQ(0x3, emulator.GetMemoryLocValue(index + 2));
    CLOVE_INT_EQ(0x4, emulator.GetMemoryLocValue(index + 3));

    // Cleanup
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(0));
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(1));
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(2));
    CLOVE_INT_EQ(0x0, emulator.GetRegisterValue(3));

    // Reads values from memory to registers
    success = emulator.Tick();
    CLOVE_IS_TRUE(success);
    CLOVE_INT_EQ(0x1, emulator.GetRegisterValue(0));
    CLOVE_INT_EQ(0x2, emulator.GetRegisterValue(1));
    CLOVE_INT_EQ(0x3, emulator.GetRegisterValue(2));
    CLOVE_INT_EQ(0x4, emulator.GetRegisterValue(3));
}