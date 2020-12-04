#define SI5351_ASSERT(x) \
    if (!(x))            \
        printf("Assertion failed : %s, at line %d\n", #x, __LINE__);

#define XTAL_FREQ 25000000
#define OUTPUT_FREQ 1250000

#include <si5351configseek.hpp>
#include <stdio.h>

int main()
{
    uint32_t stage1_a, stage1_b, stage1_c;
    uint32_t stage2_a, stage2_b, stage2_c;
    uint32_t div_by_4;
    uint32_t r;

    printf("The Si5351ConfigSeek() test starts \n");

    for (int i = 100000; i < 120000000; i += 1000)
    {

        Si5351ConfigSeek(
            XTAL_FREQ,
            i,
            stage1_a,
            stage1_b,
            stage1_c,
            stage2_a,
            stage2_b,
            stage2_c,
            div_by_4, // 0b11:div by 4, 0b:00 div by other.
            r         // output divider.
        );

        double output;
        if (div_by_4 == 3)
            output = XTAL_FREQ * (stage1_a + double(stage1_b) / stage1_c) / 4;
        else if (div_by_4 == 0)
            output = XTAL_FREQ * (stage1_a + double(stage1_b) / stage1_c) / (stage2_a) / r;
        else
            printf("logic error \n");

        if ((double)i != output)
            printf("Error at %dkHz, output is %ekHz \n", i / 1000, output / 1000);
    }

    printf("The Si5351ConfigSeek() test finished \n");

    printf("The Si5351PackRegister() test starts \n");
    uint32_t inte, num, denom, div_4, r_div;
    uint8_t registers[8];

    inte = 0x0001239F;
    denom = 0x00045678;
    num = 0x000ABCDE;
    div_4 = 3;
    r_div = 8;
    Si5351PackRegister(inte, num, denom, div_4, r_div, registers);

    SI5351_ASSERT(registers[0] == 0x56)
    SI5351_ASSERT(registers[1] == 0x78)
    SI5351_ASSERT(registers[3] == 0x23)
    SI5351_ASSERT(registers[4] == 0x9F)
    SI5351_ASSERT((registers[5] & 0xFF) == 0x4A)
    SI5351_ASSERT(registers[6] == 0xBC)
    SI5351_ASSERT(registers[7] == 0xDE)

    SI5351_ASSERT((registers[2] & 0x03) == 0x01)
    SI5351_ASSERT(((registers[2] >> 2) & 0x03) == 0x03)
    SI5351_ASSERT(((registers[2] >> 4) & 0x07) == 0x03)

    printf("The Si5351PackRegister() test finished \n");

    return 0;
}