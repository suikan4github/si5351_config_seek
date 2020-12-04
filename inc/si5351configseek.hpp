#ifndef SI5351_CONFIG_SEEK_HPP
#define SI5351_CONFIG_SEEK_HPP

#include <stdint.h>

enum Si5351Status
{
    s5351Ok,
    s5351SeekFailure
};

/**
 * @brief Seek the appropriate configuration of the Si5351.
 * @param xtal_freq Input : Xtal frequency in Hz. Must be from 10MHz to 40Mhz.
 * @param output_freq Input : Output frequency in Hz. Must be from 2.5kHz 200MHz.
 * @param integer_mode Pass true if the output divide ratio must be integer. 
 * @param stage1_int Output : Parameter "a" of the PLL feedback divider. [15, 90]
 * @param stage1_num Output : Parameter "b" of the PLL feedback divider. [0, 1048575]
 * @param stage1_denom Output : Parameter "c" of the PLL feedback divider. [0, 1048575]
 * @param stage2_int Output : Parameter "a" of the stage 2 divider. [8, 2048]
 * @param stage2_num Output : Parameter "b" of the stage 2 divider. [0, 1048575]
 * @param stage2_denom Output : Parameter "c" of the stage 2 divider. [0, 1048575]
 * @param div_by_4 Output : 0b11 : div by 4 mode. 0b00 : div by other mode.
 * @param r_div Output : Output divider
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
 *  Fout = xtal_freq * ( stage1_int + stage1_num/stage1_denom) /( (stage2_int + stage2_num/stage2_denom) * r_div )
 * 
 * Also, there are several restriction by silicon.
 * @li If the output is below 500kHz, use r_div divider 1, 2, 4, 8, ... 128. 
 * @li If the output is over 150MHz, stage 2 is a=0, b=0, c=0, div_by_4=0b11
 */
Si5351Status Si5351ConfigSeek(
    const uint32_t xtal_freq,
    const uint32_t output_freq,
    const bool integer_mode,
    uint32_t &stage1_int,
    uint32_t &stage1_num,
    uint32_t &stage1_denom,
    uint32_t &stage2_int,
    uint32_t &stage2_num,
    uint32_t &stage2_denom,
    uint32_t &div_by_4,
    uint32_t &r_div);

/**
 * @brief Pack the given data for the Si5351 dividers.
 * @param integer The integer part of the divider. See following details. 
 * @param numarator The numerator part of the divider. See following details. 
 * @param denominator The denominator part of the divider. See following details. 
 * @param div_by_4 0 for normal mode, 3 for div by 4 mode. Must be 0 for the PLL multiplier.
 * @param r_div The output divider value. Must be zero for the PLL multiplier. 
 * @param reg The packed regster. 
 * @details
 * Deta packing for the Si5351 divider. The data will be packed as following :
 * @li reg[0] = denominator[15:8]
 * @li reg[1] = denominator[7:0]
 * @li reg[2] = formated(r_div)[2:0] << 4 | div_by_4[1:0] << 2| integer[17:16]
 * @li reg[3] = integer[15:8]
 * @li reg[4] = integer[7:0]
 * @li reg[5] = denominator[19:16] << 4 |  numerator[19:16] 
 * @li reg[6] = numerator[15:8]
 * @li reg[7] = numerator[7:0]
 * 
 * Note that the r_div is formated before packed. The format from r_div to the formatted value is as followings : 
 * @li 1 -> 0
 * @li 2 -> 1
 * @li N -> log2(N)     
 * @li 128 -> 7
 * 
 * So the r_div must be the power of 2. 
 */
void Si5351PackRegister(
    const uint32_t integer,
    const uint32_t numerator,
    const uint32_t denominator,
    const uint32_t div_by_4,
    const uint32_t r_div,
    uint8_t reg[8]);

#endif // SI5351_CONFIG_SEEK_HPP
