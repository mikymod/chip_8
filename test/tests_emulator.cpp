#include "chip-8.h"

#define CLOVE_SUITE_NAME Emulator
#include "clove-unit.h"

using namespace chipotto;

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

}

CLOVE_TEST(OpcodeB_JP_V0_addr)
{

}

CLOVE_TEST(OpcodeC_RND_Vx_byte)
{

}

CLOVE_TEST(OpcodeD_DRW_Vx_Vy_nibble)
{

}

CLOVE_TEST(OpcodeE_SKP_Vx)
{

}

CLOVE_TEST(OpcodeE_SKNP_Vx)
{

}

CLOVE_TEST(OpcodeF_LD_Vx_DT)
{

}

CLOVE_TEST(OpcodeF_LD_Vx_K)
{

}

CLOVE_TEST(OpcodeF_LD_DT_VX)
{

}

CLOVE_TEST(OpcodeF_LD_ST_VX)
{

}

CLOVE_TEST(OpcodeF_ADD_I_VX)
{

}

CLOVE_TEST(OpcodeF_LD_F_VX)
{

}

CLOVE_TEST(OpcodeF_LD_B_VX)
{

}

CLOVE_TEST(OpcodeF_LD_locI_VX)
{

}

CLOVE_TEST(OpcodeF_LD_Vx_locI)
{

}

//CLOVE_TEST(Opcode1)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode1(0x1000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(Opcode2)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode2(0x2000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::NotIncrementPC), static_cast<int>(status));
//
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    status = emulator.Opcode2(0x2000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::StackOverflow), static_cast<int>(status));
//}
//
//CLOVE_TEST(Opcode3)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode3(0x3000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(Opcode4)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode4(0x4000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(Opcode5)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode5(0x5000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(Opcode6)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode6(0x6000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(Opcode7)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode7(0x7000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(Opcode8)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.Opcode8(0x8000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.Opcode8(0x2);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.Opcode8(0x3);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.Opcode8(0x4);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.Opcode8(0xE);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(OpcodeA)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.OpcodeA(0xA000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(OpcodeC)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.OpcodeC(0xC000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(OpcodeD)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.OpcodeC(0xD000);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(OpcodeE)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.OpcodeE(0xA1);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeE(0x9E);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}
//
//CLOVE_TEST(OpcodeF)
//{
//    Emulator emulator;
//    OpcodeStatus status = emulator.OpcodeF(0x55);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x65);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x33);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x29);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x0A);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::WaitForKeyboard), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x1E);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x18);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x15);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//
//    status = emulator.OpcodeF(0x07);
//    CLOVE_INT_EQ(static_cast<int>(OpcodeStatus::IncrementPC), static_cast<int>(status));
//}