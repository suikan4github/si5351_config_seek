#define SI5351_ASSERT(x) \
    if (!(x))            \
        printf("Assertion failed : %s, at line %d\n", #x, __LINE__);

#include <si5351configseek.hpp>
#include <stdio.h>
#include <math.h>

int main()
{
    uint32_t stage1_a, stage1_b, stage1_c;
    uint32_t stage2_a, stage2_b, stage2_c;
    uint32_t div_by_4;
    uint32_t r;

    printf("The Si5351ConfigSeek() test starts \n");

    // From input frequency 10Mhz to 30MHz by 5MHz step
    for (int xfreq = 10000000; xfreq < 35000000; xfreq += 5000000)
        // test from 3Hz to 200MHz
        for (int i = 3; i < 200000000; i += 1)
        {

            Si5351ConfigSeek(
                xfreq, // MHz
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
                output = xfreq * (stage1_a + double(stage1_b) / stage1_c) / 4;
            else if (div_by_4 == 0)
                output = xfreq * (stage1_a + double(stage1_b) / stage1_c) / (stage2_a) / r;
            else
                printf("logic error \n");

            // The mantissa of the double is 15digit. Accept 2digit error.
            if (abs(i - output) > double(i) * 1e-13)
            {
                printf("Error at %dHz, difference is %eHz \n", i, output - i);
                break;
            }
        }

    printf("The Si5351ConfigSeek() test finished \n");

    printf("The Si5351PackRegister() test starts \n");
    uint32_t inte, num, denom, div_4, r_div;
    uint8_t registers[8];
    // first test
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
    SI5351_ASSERT(registers[5] == 0x4A)
    SI5351_ASSERT(registers[6] == 0xBC)
    SI5351_ASSERT(registers[7] == 0xDE)

    SI5351_ASSERT((registers[2] & 0x03) == 0x01)
    SI5351_ASSERT(((registers[2] >> 2) & 0x03) == 0x03)
    SI5351_ASSERT(((registers[2] >> 4) & 0x07) == 0x03)

    // Second test as negative bit pattern of first one.
    inte = 0x0003FFFF & ~inte;
    denom = 0x000FFFFF & ~denom;
    num = 0x000FFFFF & ~num;
    div_4 = 0;
    r_div = 128;
    Si5351PackRegister(inte, num, denom, div_4, r_div, registers);

    SI5351_ASSERT(registers[0] == (0xFF & ~0x56))
    SI5351_ASSERT(registers[1] == (0xFF & ~0x78))
    SI5351_ASSERT(registers[3] == (0xFF & ~0x23))
    SI5351_ASSERT(registers[4] == (0xFF & ~0x9F))
    SI5351_ASSERT(registers[5] == (0xFF & ~0x4A))
    SI5351_ASSERT(registers[6] == (0xFF & ~0xBC))
    SI5351_ASSERT(registers[7] == (0xFF & ~0xDE))

    SI5351_ASSERT((registers[2] & 0x03) == (0x03 & ~0x01))
    SI5351_ASSERT(((registers[2] >> 2) & 0x03) == 0x00)
    SI5351_ASSERT(((registers[2] >> 4) & 0x07) == 0x07)

    printf("The Si5351PackRegister() test finished \n");

    return 0;
}