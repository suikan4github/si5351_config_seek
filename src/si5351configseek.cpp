#include <si5351configseek.hpp>
#define SI5351_DEBUG
#define SI5351_ASSERT(x)
#define SI5351_SYSLOG(x, ...)

Si5351Status Si5351ConfigSeek(
    const int32_t xtal_freq,
    const int32_t output_freq,
    const bool integer_mode,
    int32_t &stage1_a,
    int32_t &stage1_b,
    int32_t &stage1_c,
    int32_t &stage2_a,
    int32_t &stage2_b,
    int32_t &stage2_c,
    int32_t &div_by_4,
    int32_t &r)
{
    // Because the div_by_4 mode is tricky, keep second stage divider value here.
    int32_t second_stage_divider;

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
        stage2_a = 0;
        stage2_b = 0;
        stage2_c = 1;

        // For output freq > 150MHz, r is set to 1;
        r = 1;
        second_stage_divider = 4;
    }
    else if (output_freq > 100000000)
    {
        SI5351_SYSLOG(SI5351_DEBUG, "Frequency is higher than 100MHz");

        SI5351_SYSLOG(SI5351_DEBUG, "Set the stage 2 to divid by 6 mode")

        // divide by 6
        div_by_4 = 0; // requed by Si5351 data sheet
        stage2_a = 6;
        stage2_b = 0;
        stage2_c = 1;

        // r is set to 1;
        r = 1;
        second_stage_divider = 6;
    }
    else if (output_freq > 75000000)
    {
        SI5351_SYSLOG(SI5351_DEBUG, "Frequency is higher than 75MHz");

        SI5351_SYSLOG(SI5351_DEBUG, "Set the stage 2 to divid by 8 mode")

        // divide by 6
        div_by_4 = 0; // requed by Si5351 data sheet
        stage2_a = 8;
        stage2_b = 0;
        stage2_c = 1;

        // r is set to 1;
        r = 1;
        second_stage_divider = 8;
    }
    else // output freq <= 75MHz.
    {
        SI5351_SYSLOG(SI5351_DEBUG, "Frequency is lower than or equal to 75MHz");

        // Initial value of the output divider
        r = 1;

        // Seek the appropriate multisynth divider.

        // At first, roughly calculate the dividing value. 600MHz is the lowest VCO freq.
        second_stage_divider = 600000000 / output_freq;
        // round up. Now, second_stage_divider * r * output_freq is bigger than 600MHz.
        second_stage_divider++;

        // The second stage divider have to be lower than or equal to 2048

        while (second_stage_divider > 2048)
        {
            r *= 2;
            // At first, roughly calculate the dividing value. 600MHz is the lowest VCO freq.
            second_stage_divider = 600000000 / (r * output_freq);
            // round up. Now, second_stage_divider * r * output_freq is bigger than 600MHz.
            second_stage_divider++;
        }

        // divide by 6
        div_by_4 = 0; // requed by Si5351 data sheet
        stage2_a = second_stage_divider;
        stage2_b = 0;
        stage2_c = 1;
    }

    SI5351_SYSLOG(SI5351_DEBUG, "r = %d", r);
    SI5351_SYSLOG(SI5351_DEBUG, "stage2_a = %d", stage2_a);
    SI5351_SYSLOG(SI5351_DEBUG, "stage2_b = %d", stage2_b);
    SI5351_SYSLOG(SI5351_DEBUG, "stage2_c = %d", stage2_c);

    int32_t vco_freq = output_freq * second_stage_divider * r;
    SI5351_SYSLOG(SI5351_DEBUG, "VCO frequency = %d", vco_freq);

    // fvco = fxtal * a + mod
    //      = fxtal * ( a + mod / fxtal )
    //      = fxtal * ( a + b / c)

    stage1_a = vco_freq / xtal_freq;
    stage1_b = vco_freq % xtal_freq;
    stage1_c = xtal_freq;

    // Do the Euclidean Algorithm to reduce the b and c
    int gcd, remainder;
    int x = stage1_b;
    int y = stage1_c;

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

    stage1_b /= gcd;
    stage1_c /= gcd;

    SI5351_SYSLOG(SI5351_DEBUG, "stage1_a = %d", stage1_a);
    SI5351_SYSLOG(SI5351_DEBUG, "stage1_b = %d", stage1_b);
    SI5351_SYSLOG(SI5351_DEBUG, "stage1_c = %d", stage1_c);

    return s5351Ok;
}