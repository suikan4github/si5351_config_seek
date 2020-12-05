# si5351_config_seek
Seek the appropriate configuration for the given input frequency and output frequency
# Description
Helper library to set up the Silicon Lab Si5351 PLL IC. This library allows to setup the Si5351 by the combination of any Input frequency and Output frequency. Both must be the integer [Hz].

The library is tested by the combination of : 
- 10, 15, 20, 25, 30MHz input
- 3Hz to 200MHz output ( 1Hz step )

# API
## Si5351ConfigSeek()
```C++
Si5351Status Si5351ConfigSeek(
    const uint32_t xtal_freq,
    const uint32_t output_freq,
    uint32_t &stage1_int,
    uint32_t &stage1_num,
    uint32_t &stage1_denom,
    uint32_t &stage2_int,
    uint32_t &stage2_num,
    uint32_t &stage2_denom,
    uint32_t &div_by_4,
    uint32_t &r_div);
```
 *  xtal_freq [IN] : Xtal frequency in Hz. Must be from 10MHz to 40Mhz.
 *  output_freq [IN] : Output frequency in Hz. Must be from 2.5kHz 200MHz.
 *  stage1_int [OUT] : Parameter "a" of the PLL feedback divider. [15, 90]
 *  stage1_num [OUT] : Parameter "b" of the PLL feedback divider. [0, 1048575]
 *  stage1_denom [OUT] : Parameter "c" of the PLL feedback divider. [0, 1048575]
 *  stage2_int [OUT] : Parameter "a" of the stage 2 divider. [8, 2048]
 *  stage2_num [OUT] : Parameter "b" of the stage 2 divider. [0, 1048575]
 *  stage2_denom [OUT] : Parameter "c" of the stage 2 divider. [0, 1048575]
 *  div_by_4 [OUT] : 0b11 : div by 4 mode. 0b00 : div by other mode.
 *  r_div [OUT] : Output divider

  Calcurate an appropriate set of the PLL parameters for Si5351. See AN619 for detail of Si5351.
  
  This function calcurate the formula only. So, the atuall register 
  configuration have to be compiled from the given parameters. 
  
  The configuration seek is doen for the Multisynth(TM) dividers of the
  Si5351. In this fuction, we assume following configuraiton.
 *  No prescalor in front of PLL
 *  The first stage is PLL
 *  The second starge is Multisynth divider. 
 *  Output Divider. 
  
  In this configuration, we have two Multisynth Dividers. One is the 
  second stage. And the other is the feedback divider of the PLL. 
  
  This function seek the appropriate setting based on the following restriction:
 *  Keep second stage as integer division.
 *  Keep the PLL VCO frequency between 600 to 900MHz.
  
  
  The output frequency is : 
   Fout = xtal_freq  ( stage1_int + stage1_num/stage1_denom) /( (stage2_int + stage2_num/stage2_denom)  r_div )
  
  Also, there are several restriction by silicon.
 *  If the output is below 500kHz, use r_div divider 1, 2, 4, 8, ... 128. 
 *  If the output is over 150MHz, stage 2 is int=0, num=0, denom=0, div_by_4=0b11

## Si5351PackRegister()
```C++
void Si5351PackRegister(
    const uint32_t integer,
    const uint32_t numerator,
    const uint32_t denominator,
    const uint32_t div_by_4,
    const uint32_t r_div,
    uint8_t reg[8]);
```


 *  integer [IN] The integer part of the divider. See following details. 
 *  numarator [IN] The numerator part of the divider. See following details. 
 *  denominator [IN] The denominator part of the divider. See following details. 
 *  div_by_4 [IN] 0 for normal mode, 3 for div by 4 mode. Must be 0 for the PLL multiplier.
 *  r_div [IN] The output divider value. Must be zero for the PLL multiplier. 
 *  reg [OUT] The packed regster. 

 Pack the given data for the Si5351 dividers.

  Deta packing for the Si5351 divider. The data will be packed as following :
 *  reg[0] = denominator[15:8]
 *  reg[1] = denominator[7:0]
 *  reg[2] = formated(r_div)[2:0] << 4 | div_by_4[1:0] << 2| integer[17:16]
 *  reg[3] = integer[15:8]
 *  reg[4] = integer[7:0]
 *  reg[5] = denominator[19:16] << 4 |  numerator[19:16] 
 *  reg[6] = numerator[15:8]
 *  reg[7] = numerator[7:0]
 
 Note that the r_div is formated before packed. The format from r_div to the formatted value is as followings : 
 *  1 -> 0
 *  2 -> 1
 *  N -> log2(N)     
 *  128 -> 7
 
 So the r_div must be the power of 2. 

# How to build
Use cmnake to build.
```shell
mkdir build
cd build
cmake ..
make
```
The artifact "Main" is the executable test program. It scans from 3Hz to 200MHz and report if there are some error. 

# License
This program is distributed under the [MIT License](LICENSE)