#ifndef FRI3DBADGE_BLASTER24DATA_H
#define FRI3DBADGE_BLASTER24DATA_H

#include <Arduino.h>
#include <vector>

const int ir_bit_lenght = 32;
const int ir_start_high_time = 16;
const int ir_start_low_time = 8;
const int ir_zero_high_time = 1;
const int ir_zero_low_time = 1;
const int ir_one_high_time = 1;
const int ir_one_low_time = 3;
const int ir_stop_high_time = 1;
const int ir_stop_low_time = 1;
const int pulse_train_lenght =  2 + ir_bit_lenght * 2 + 2;
#define IR_IN1_PIN 11 // badge 2024

enum TeamColor : uint8_t
{
  eNoTeam = 0b000,
  eTeamRex = 0b001,
  eTeamGiggle = 0b010,
  eTeamBuzz = 0b100,
  eTeamYellow = eTeamRex | eTeamGiggle,
  eTeamMagenta = eTeamRex | eTeamBuzz,
  eTeamCyan = eTeamGiggle | eTeamBuzz,
  eTeamWhite = eTeamRex | eTeamGiggle | eTeamBuzz
};

enum Action : uint8_t
{
  eActionNone = 0,
  eActionDamage = 1,
  eActionHeal = 2,
};

/**
 * @brief IR data packet
 * holds the 32 ir bits in a private uint32_t
 * structure: return type - field name - nr of bits
 *  uint8_t channel: 1;
 *  uint8_t team: 3;
 *  uint8_t action: 2;
 *  uint8_t action_param: 4;
 *  uint16_t player_id: 12;
 *  uint8_t crc: 8;
 *  uint8_t unused: 2;
 */
class IrDataPacket
{
private:
  uint32_t raw;
public:
  IrDataPacket();
  IrDataPacket(uint32_t raw);
  uint32_t get_raw();
  void set_raw(uint32_t raw);
  uint8_t get_channel();
  void set_channel(uint8_t channel);
  uint8_t get_team();
  void set_team(uint8_t team);
  uint8_t get_action();
  void set_action(uint8_t action);
  uint8_t get_action_param();
  void set_action_param(uint8_t action_param);
  uint16_t get_player_id();
  void set_player_id(uint16_t player_id);
  uint8_t get_crc();
  void set_crc(uint8_t crc);
  uint8_t get_unused();
  void set_unused(uint8_t unused);
};


class DataReader
{
private:
  volatile unsigned long refTime;
  volatile bool oldState = 1;
  volatile uint32_t rawData = 0;
  volatile uint8_t bitsRead = 0;
  volatile bool dataReady = 0;

public:
  void handlePinChange();
  void reset();         // clear buffer
  bool isDataReady();   // check buffer, if valid True, if invalid ResetBuffer
  uint32_t getPacket(); // return packet and reset; Dataclass then needs to calculate CRC
};

class _data
{
private:
  _data();
  DataReader ir1_reader;

  void enableReceive();
  void disableReceive();

public:
  IrDataPacket readIr();

  static _data &getInstance();
  static uint32_t calculateCRC(uint32_t raw_packet);
  void receive_ISR(); // function called by ISR
  void init();
};

extern _data &Data;
#endif
