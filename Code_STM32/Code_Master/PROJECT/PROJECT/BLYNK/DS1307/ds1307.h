
#ifndef INC_DS1307_H_
#define INC_DS1307_H_

#include "main.h"

  typedef struct
{
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint8_t year;
}DS1307_TIME;


#define DS1307_ADDR (0x68 << 1)



void I2C_CONFIG(I2C_HandleTypeDef *I2C);

uint8_t B2D(uint8_t num);
uint8_t D2B(uint8_t num);
void SET_TIME(DS1307_TIME *setTime);
void GET_TIME(DS1307_TIME *getTime);

#endif /* INC_DS1307_H_ */
