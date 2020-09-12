#include "Sensor.h"
#include <iocc2530.h>

#define HAL_ADC_REF_115V        0X00
#define HAL_ADC_DEC_256         0X20
#define HAL_ADC_CHN_TEMP        0X0e

int8 readTemp(void)
{
  static uint16 reference_voltage;
  static uint8 bCalibrate = TRUE;
  uint16 value;
  int8 temp;

  ATEST         = 0X01;
  TR0          |= 0X01;
  ADCIF         = 0;
  ADCCON3       = (HAL_ADC_REF_115V | HAL_ADC_DEC_256 | HAL_ADC_CHN_TEMP);
  while( !ADCIF );
  ADCIF         = 0;
  value         = ADCL;
  value        |= ((uint16) ADCH) << 8;
  value       >>= 4;
  
  if(bCalibrate)
  {
    reference_voltage=value;
    bCalibrate = FALSE;
  }
  temp = 22 + ((value - reference_voltage)/4);
  return temp;
}