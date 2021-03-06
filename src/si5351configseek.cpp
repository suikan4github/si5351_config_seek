#include <si5351configseek.hpp>
#include <stdio.h>

#define SI5351_DEBUG
#define SI5351_ASSERT(x) \
    if (!(x))            \
        printf("Assertion failed : %s, at line %d\n", #x, __LINE__);
#define SI5351_SYSLOG(x, ...)

#define R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO 256

Si5351Status Si5351ConfigSeek(
    const uint32_t xtal_freq,
    const uint32_t output_freq,
    uint32_t &stage1_int,
    uint32_t &stage1_num,
    uint32_t &stage1_denom,
    uint32_t &stage2_int,
    uint32_t &stage2_num,
    uint32_t &stage2_c,
    uint32_t &div_by_4,
    uint32_t &r_div)
{
    // Because the div_by_4 mode is tricky, keep second stage divider value here.
    uint32_t second_stage_divider;

    SI5351_ASSERT(output_freq > 2);         // must be higher than 2Hz.
    SI5351_ASSERT(200000000 > output_freq); // must be lower than or equal to 200MHz

    SI5351_SYSLOG("Xtal Frequency is %d Hz", xtal_freq);
    SI5351_SYSLOG("Output Frequency is %d Hz", output_freq);

    // Si5351 requires to set the div-by-4 mode if the output is higher than 150MHz
    if (output_freq > 150000000) // If output freq is > 150MHz
    {
        SI5351_SYSLOG(SI5351_DEBUG, "Frequency is higher than 150MHz");

        SI5351_SYSLOG(SI5351_DEBUG, "Set the stage 2 to divid by 4 mode")
        // enforce divide by 4 mode.
        // Note that followings are required by Si5351 data sheet.
        // In case of divide by 4 mode, the second stage have to be integer mode.
        div_by_4 = 3; // requed by Si5351 data sheet
        stage2_int = 0;
        stage2_num = 0;
        stage2_c = 1;

        // For output freq > 150MHz, r_div is set to 1;
        r_div = 1;
        second_stage_divider = 4;
    }
    else if (output_freq > 100000000)
    {
        SI5351_SYSLOG(SI5351_DEBUG, "Frequency is higher than 100MHz");

        SI5351_SYSLOG(SI5351_DEBUG, "Set the stage 2 to divid by 6 mode")

        // divide by 6
        div_by_4 = 0; // requed by Si5351 data sheet
        stage2_int = 6;
        stage2_num = 0;
        stage2_c = 1;

        // r_div is set to 1;
        r_div = 1;
        second_stage_divider = 6;
    }
    else if (output_freq > 75000000)
    {
        SI5351_SYSLOG(SI5351_DEBUG, "Frequency is higher than 75MHz");

        SI5351_SYSLOG(SI5351_DEBUG, "Set the stage 2 to divid by 8 mode")

        // divide by 6
        div_by_4 = 0; // requed by Si5351 data sheet
        stage2_int = 8;
        stage2_num = 0;
        stage2_c = 1;

        // r_div is set to 1;
        r_div = 1;
        second_stage_divider = 8;
    }
    else // output freq <= 75MHz.
    {
        SI5351_SYSLOG(SI5351_DEBUG, "Frequency is lower than or equal to 75MHz");

        // Initial value of the output divider
        r_div = 1;

        // Seek the appropriate multisynth divider.

        // At first, roughly calculate the dividing value. 600MHz is the lowest VCO freq.
        second_stage_divider = 600000000 / output_freq;
        // round up. Now, second_stage_divider * r_div * output_freq is bigger than 600MHz.
        second_stage_divider++;

        // The second stage divider have to be lower than or equal to 2048

        while (second_stage_divider > 2048)
        {
            r_div *= 2;
            // At first, roughly calculate the dividing value. 600MHz is the lowest VCO freq.
            second_stage_divider = 600000000 / (r_div * output_freq);
            // round up. Now, second_stage_divider * r_div * output_freq is bigger than 600MHz.
            second_stage_divider++;
        }

        // divide by 6
        div_by_4 = 0; // requed by Si5351 data sheet
        stage2_int = second_stage_divider;
        stage2_num = 0;
        stage2_c = 1;
    }

    SI5351_SYSLOG(SI5351_DEBUG, "r_div = %d", r_div);
    SI5351_SYSLOG(SI5351_DEBUG, "stage2_int = %d", stage2_int);
    SI5351_SYSLOG(SI5351_DEBUG, "stage2_num = %d", stage2_num);
    SI5351_SYSLOG(SI5351_DEBUG, "stage2_c = %d", stage2_c);

    uint32_t vco_freq = output_freq * second_stage_divider * r_div;
    SI5351_SYSLOG(SI5351_DEBUG, "VCO frequency = %d", vco_freq);

    // fvco = fxtal * a + mod
    //      = fxtal * ( a + mod / fxtal )
    //      = fxtal * ( a + b / c)

    stage1_int = vco_freq / xtal_freq;
    stage1_num = vco_freq % xtal_freq;
    stage1_denom = xtal_freq;

    // Do the Euclidean Algorithm to reduce the b and c
    int gcd, remainder;
    int x = stage1_num;
    int y = stage1_denom;

    while (1)
    {
        remainder = x % y;

        if (remainder == 0)
        {
            gcd = y;
            break;
        }
        else
        {
            x = y;
            y = remainder;
        }
    }

    stage1_num /= gcd;
    stage1_denom /= gcd;

    SI5351_SYSLOG(SI5351_DEBUG, "stage1_a = %d", stage1_int);
    SI5351_SYSLOG(SI5351_DEBUG, "stage1_num = %d", stage1_num);
    SI5351_SYSLOG(SI5351_DEBUG, "stage1_denom = %d", stage1_denom);

    return s5351Ok;
}

void Si5351PackRegister(
    const uint32_t integer,
    const uint32_t numerator,
    const uint32_t denominator,
    const uint32_t div_by_4,
    const uint32_t r_div,
    uint8_t reg[8])
{
    // integer part must be no bigger than 18bits.
    SI5351_ASSERT((integer & 0xFFFC0000) == 0)
    // numarator and denominator part must be no bigger than 20bits.
    SI5351_ASSERT((numerator & 0xFFF00000) == 0)
    SI5351_ASSERT((denominator & 0xFFF00000) == 0)
    // Right value of div by four must be 0 or 3
    SI5351_ASSERT((div_by_4 == 0) || (div_by_4 == 3))

    reg[0] = (denominator >> 8) & 0x00FF;          // extract [15:8]
    reg[1] = (denominator)&0x00FF;                 // extract [7:0]
    reg[3] = (integer >> 8) & 0x00FF;              // extract [15:8]
    reg[4] = (integer)&0x00FF;                     // extract [7:0]
    reg[5] = (((denominator) >> 16) & 0x0F) << 4 | // extract [19:16]
             (numerator >> 16) & 0x0F;             // extract [19:16]
    reg[6] = (numerator >> 8) & 0x00FF;            // extract [15:8]
    reg[7] = (numerator)&0x00FF;                   // extract [7:0]

    reg[2] = 0;

    reg[2] |= (integer >> 16) & 0x03;
    reg[2] |= div_by_4 << 2;

    // Calcurate the field value for the r_div.
    // The r_div is true divider value. That is the value of the 2^x.
    // The field value mast be this "x". Following code seeks the x for r_div.
    uint32_t field = R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO; // 256 is an error value.

    int value = 1;
    for (int i = 0; i < 8; i++)
    {
        if (value == r_div)
        {
            field = i;
            break;
        }
        else
            value <<= 1;
    }

    SI5351_ASSERT(field != R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO) // Make sure the field value is not error.
    reg[2] |= field << 4;
}