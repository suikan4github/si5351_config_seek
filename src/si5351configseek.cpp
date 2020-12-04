#include <si5351configseek.hpp>
#define SI5351_DEBUG
#define SI5351_ASSERT(x)
#define SI5351_SYSLOG(x, ...)

Si5351Status Si5351ConfigSeek(
    const uint32_t xtal_freq,
    const uint32_t output_freq,
    const bool integer_mode,
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