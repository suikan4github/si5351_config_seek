

#define XTAL_FREQ 25000000
#define OUTPUT_FREQ 1250000

#include <si5351configseek.hpp>
#include <stdio.h>

int main()
{
    int32_t stage1_a, stage1_b, stage1_c;
    int32_t stage2_a, stage2_b, stage2_c;
    int32_t div_by_4;
    int32_t r;

    for (int i = 100000; i < 120000000; i += 1000)
    {

        Si5351ConfigSeek(
            XTAL_FREQ,
            i,
            true, // integer mode.
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

    return 0;
}