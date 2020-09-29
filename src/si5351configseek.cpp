#include <si5351configseek.hpp>

#define SI5351_ASSERT(x)
#define SI5351_SYSLOG(x, y)

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

    if (output_freq > 150000000) // If output freq is > 150MHz
    {
        SI5351_SYSLOG(nullptr, "Frequency is higher than 150MHz");

        SI5351_SYSLOG(nullptr, "Set the stage 2 to divid by 4 mode")
        // enforce divide by 4 mode.
        // Note that followings are required by Si5351 data sheet.
        // In case of divide by 4 mode, the second stage have to be integer mode.
        div_by_4 = 3; // requed by Si5351 data sheet
        stage2_a = 0;
        stage2_b = 0;
        stage2_c = 1;

        // For output freq > 150MHz, r is set to 1;
        SI5351_SYSLOG(nullptr, "Set r as 1");
        r = 1;

        second_stage_divider = 4;
    }
    else
    {
        // At first, roughly calculate the dividing value.
        second_stage_divider = 1 + 600000000 / output_freq;
        r = 1;
    }

    return s5351Ok;
}