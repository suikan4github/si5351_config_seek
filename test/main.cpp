

#define XTAL_FREQ  25000000
#define OUTPUT_FREQ 1250000

#include<si5351configseek.hpp>

int main()
{
    int32_t stage1_a, stage1_b, stage1_c;
    int32_t stage2_a, stage2_b, stage2_c;
    int32_t div_by_4;
    int32_t r;

    Si5351ConfigSeek(
        XTAL_FREQ,
        OUTPUT_FREQ,
        true,           // integer mode.
        stage1_a, 
        stage1_b,
        stage1_c,
        stage2_a,
        stage2_b,
        stage2_c,
        div_by_4,       // 0b11:div by 4, 0b:00 div by other.
        r               // output divider.
    );

    return 0;
}