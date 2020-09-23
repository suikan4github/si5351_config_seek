#ifndef SI5351_CONFIG_SEEK_HPP
#ifndef SI5351_CONFIG_SEEK_HPP

#include <stdint.h>

enum Si5351Status {
    s5351Ok,
    s5351SeekFailure
};

/**
 * @brief Seek the appropriate configuration of the Si5351.
 * @param xtal_freq Input : Xtal frequency in Hz. Must be from 10MHz to 40Mhz.
 * @param output_freq Input : Output frequency in Hz. Must be from 2.5kHz 200MHz.
 * @param integer_mode Pass true if the output divide ratio must be integer. 
 * @param stage1_a Output : Parameter "a" of the PLL feedback divider. [15, 90]
 * @param stage1_b Output : Parameter "b" of the PLL feedback divider. [0, 1048575]
 * @param stage1_b Output : Parameter "c" of the PLL feedback divider. [0, 1048575]
 * @param stage2_a Output : Parameter "a" of the stage 2 divider. [8, 2048]
 * @param stage2_b Output : Parameter "b" of the stage 2 divider. [0, 1048575]
 * @param stage2_c Output : Parameter "c" of the stage 2 divider. [0, 1048575]
 * @param div_by_4 Output : 0b11 : div by 4 mode. 0b00 : div by other mode.
 * @param r Output : Output divider
 * @details 
 * Calcurate an appropriate set of the PLL parameters for Si5351. See AN619 for detail of Si5351.
 * 
 * This function calcurate the formula only. So, the atuall register 
 * configuration have to be compiled from the given parameters. 
 * 
 * The configuration seek is doen for the Multisynth(TM) dividers of the
 * Si5351. In this fuction, we assume following configuraiton.
 * @li No prescalor in front of PLL
 * @li The first stage is PLL
 * @li The second starge is Multisynth divider. 
 * @li Output Divider. 
 * 
 * In this configuration, we have two Multisynth Dividers. One is the 
 * second stage. And the other is the feedback divider of the PLL. Because
 * the Multisynth divider is a fractional divider. 
 * 
 * This function seek the appropriate setting based on the following restriction:
 * @li Keep second stage as integer division, if the integer_mode parameter is true. 
 * @li Keep the PLL VCO frequency between 600 to 900MHz.
 * 
 * 
 * The output frequency is : 
 *  Fout = xtal_freq * ( stage1_a + stage1_b/stage1_c) /( (stage2_a + stage2_b/stage2_c) * r )
 * 
 * Also, there are several restriction by silicon.
 * @li If the output is below 500kHz, use r divider 1, 2, 4, 8, ... 128. 
 * @li If the output is over 150MHz, stage 2 is a=0, b=0, c=0, div_by_4=0b11
 */
Si5351Status Si5351ConfigSeek(
    const int32_t xtal_freq,
    const int32_t output_freq,
    const bool integer_mode,
    int32_t & stage1_a,
    int32_t & stage1_b,
    int32_t & stage1_c,
    int32_t & stage2_a,
    int32_t & stage2_b,
    int32_t & stage2_c,
    int32_t & div_by_4,
    int32_t & r
);


#endif // SI5351_CONFIG_SEEK_HPP
