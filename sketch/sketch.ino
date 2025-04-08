#include <ModbusRtu.h>
#include <max6675.h>
#include <TimerOne.h>

// data array for modbus network sharing
uint16_t au16data[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1 };

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI
 *               or any pin number > 1 for RS-485
 */
Modbus slave(1, Serial, 0);  // this is slave @1 and RS-232 or USB-FTDI

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

const int relay = 9;

void setup() {
  Serial.begin(19200, SERIAL_8E1);  // 19200 baud, 8-bits, even, 1-bit stop
  slave.start();

  Timer1.initialize(1000000);  // 1000000us = 1s PWM cycle
  Timer1.attachInterrupt(setNewPWMDuty);
  delay(5);
  Timer1.pwm(relay, 0);

  au16data[2] = 500;
}

void setNewPWMDuty(void)
{
  // write relay value using pwm
  Timer1.pwm(relay, (au16data[4] / 100.0) * 1023);
}

void loop() {
  // write current thermocouple value
  au16data[2] = ((uint16_t)thermocouple.readCelsius() * 100);

  // poll modbus registers
  slave.poll(au16data, 16);
}
