#ifndef BLASTER_PACKET_HPP
#define BLASTER_PACKET_HPP

#include <stdint.h>
#include "Arduino.h"

enum TeamColor : uint8_t
{
  eNoTeam = 0b000,        // 0
  eTeamRex = 0b001,       // 1
  eTeamGiggle = 0b010,    // 2
  eTeamBuzz = 0b100,      // 4

  eTeamYellow = eTeamRex | eTeamGiggle, // 3
  eTeamMagenta = eTeamRex | eTeamBuzz, // 5
  eTeamCyan = eTeamGiggle | eTeamBuzz,  // 6

  eTeamWhite = eTeamRex | eTeamGiggle | eTeamBuzz // 7
};

enum CommandType : uint8_t
{
  eCommandNone = 0,
  eCommandShoot = 1,
  eCommandHeal = 2,
  eCommandSetChannel = 3,
  eCommandSetTriggerAction = 4,
  eCommandSetGameMode = 5,
  eCommandSetHitTimeout = 6,
  eCommandPlayAnimation = 7,
  eCommandTeamChange = 8,
  eCommandChatter = 9,
  eCommandPullTrigger = 10,
  eCommandSetSettings = 11,
  //eCommandSetFlagsB = 12,
  //eCommandReservedA = 13,
  //eCommandReservedB = 14,
  eCommandBlasterAck = 15,
};

enum GameMode : uint8_t
{
  eGMTimeout = 0,
  eGMZombie = 1,
  eGMSuddenDeath = 2,
  eGMChatterMaster = 3
};

enum AnimationNames: uint8_t {
  eAnimationBlasterStart = 1,
  eAnimationError = 2,
  eAnimationCrash = 3,
  eAnimationFireball = 4,
  eAnimationOneUp = 5,
  eAnimationCoin = 6,
  eAnimationVoice = 7,
  eAnimationWolfWhistle = 8,
  eAnimationChatter = 9,
  
  eAnimationBlinkTeamLed = 15,
};

enum ParameterNames: uint8_t {
  eParameterNone = 0,
  eParameterBlasterNotReady = 0,
  eParameterBlasterReady = 1,
  eParameterTeamChangeHardware = 0,
  eParameterTeamChangeZombie = 1
};

struct TimedRawData {
    int64_t time_micros = -1;
    uint16_t raw_data = 0;
};

class DataPacket
{
public:
  DataPacket(){}
  DataPacket(TimedRawData t) : m_t(t){}

  uint16_t get_raw() {
    return m_t.raw_data;
  }

  int64_t get_time_micros() {
    return m_t.time_micros;
  }

  TeamColor get_team() {
    return TeamColor((m_t.raw_data & 0b0000000000000111) >> 0);
  }
  
  const char* get_team_str() {
    switch (get_team()) {
      case eNoTeam:
        return "NO TEAM";
        break;
      case eTeamRex:
        return "REX";
        break;
      case eTeamGiggle:
        return "GIGGLE";
        break;
      case eTeamYellow:
        return "YELLOW";
        break;
      case eTeamBuzz:
        return "BUZZ";
        break;
      case eTeamMagenta:
        return "MAGENTA";
        break;
      case eTeamCyan:
        return "CYAN";
        break;
      case eTeamWhite:
        return "WHITE";
        break;
      default:
        return "";
    }
  }

  void set_team(TeamColor team) {
    m_t.raw_data &= ~0b111;
    m_t.raw_data |= (team & 0b111);	
  }

  bool get_trigger() {
    return (m_t.raw_data & 0b0000000000001000) >> 3;
  }

  void set_trigger(bool value) {
    m_t.raw_data &= ~(0b1 << 3);
    m_t.raw_data |= value << 3;
  }

  CommandType get_command() {
    return CommandType((m_t.raw_data & 0b0000000011110000) >> 4);
  }

  void set_command(CommandType command) {
    m_t.raw_data &= ~(0b1111 << 4);
    m_t.raw_data |= (command & 0b1111) << 4;
  }

  uint8_t get_parameter() {
    return (m_t.raw_data & 0b0000111100000000) >> 8;
  }

  void set_parameter(uint8_t value) {
    m_t.raw_data &= ~(0b1111 << 8);
    m_t.raw_data |= (value & 0b1111) << 8;
  }

  uint8_t get_crc() {
    return (m_t.raw_data & 0b1111000000000000) >> 12;
  }

  void set_crc(uint8_t value) {
    m_t.raw_data &= 0b0000111111111111;
    m_t.raw_data |= (value << 12);
  }

  uint8_t calculate_crc(bool apply = false)
  {
    bool crc[] = {0, 0, 0, 0};
    // makes computing the checksum a litle bit faster
    bool d0 = bitRead(m_t.raw_data, 0);
    bool d1 = bitRead(m_t.raw_data, 1);
    bool d2 = bitRead(m_t.raw_data, 2);
    bool d3 = bitRead(m_t.raw_data, 3);
    bool d4 = bitRead(m_t.raw_data, 4);
    bool d5 = bitRead(m_t.raw_data, 5);
    bool d6 = bitRead(m_t.raw_data, 6);
    bool d7 = bitRead(m_t.raw_data, 7);
    bool d8 = bitRead(m_t.raw_data, 8);
    bool d9 = bitRead(m_t.raw_data, 9);
    bool d10 = bitRead(m_t.raw_data, 10);
    bool d11 = bitRead(m_t.raw_data, 11);

    crc[0] = d11 ^ d10 ^ d9 ^ d8 ^ d6 ^ d4 ^ d3 ^ d0 ^ 0;
    crc[1] = d8 ^ d7 ^ d6 ^ d5 ^ d3 ^ d1 ^ d0 ^ 1;
    crc[2] = d9 ^ d8 ^ d7 ^ d6 ^ d4 ^ d2 ^ d1 ^ 1;
    crc[3] = d10 ^ d9 ^ d8 ^ d7 ^ d5 ^ d3 ^ d2 ^ 0;

    uint8_t calc_crc = (crc[3] << 3) + (crc[2] << 2) + (crc[1] << 1) + crc[0];
    if (apply) {
      set_crc(calc_crc);
    }
    return calc_crc;
  }

  /**
   * @brief create a String representation of the packet
   * 
   * @param dest  the character array where the string representation will be put (foresee 100+ chars)
   * @param size  the size of the character array
   */
  void print(char* dest, size_t size) {
    int len = snprintf(dest, size, "packet: time: %lld, raw: %x, CRC: %s, command: %u, team: %s, trigger: %s, parameter: %d", 
      get_time_micros(),
      get_raw(),
      calculate_crc()==get_crc()?"OK":"NOK",
      get_command(),
      get_team_str(),
      get_trigger()?"true":"false",
      get_parameter()
    );
  }

private:
  TimedRawData m_t = TimedRawData();
};


/**
 * @brief RingBuffer with `c_max_buffer_size` elements of type `T` with a separate reader and writer.
 *        writer always has preference and overflows the reader
 *        check `m_stat_overflow` to see if this happened
 * 
 * @tparam T the type of the elements in the RingBuffer
 */
template<typename T>
class RingBuffer {
public:

    void add_message(T message) {
        this->m_elements_buffer[this->m_buffer_writer] = message;
        this->m_buffer_writer += 1;
        if (this->m_buffer_writer >= c_max_buffer_size) {
            this->m_buffer_writer = 0;
        }
        if (this->m_buffer_writer == this->m_buffer_reader) {
            // buffer overflow
            this->m_stat_overflow += 1;
            this->m_buffer_reader += 1;
            if (this->m_buffer_reader >= c_max_buffer_size) {
                this->m_buffer_reader = 0;
            }
        }
    }

  bool message_available() {
    return this->m_buffer_reader != this->m_buffer_writer;
  }

  T pop_message() {
    if (this->m_buffer_reader != this->m_buffer_writer) {
      T element = this->m_elements_buffer[this->m_buffer_reader];
      this->m_buffer_reader += 1;
      if (this->m_buffer_reader >= c_max_buffer_size) {
        this->m_buffer_reader = 0;
      }
      return element;
    } else {
      T empty = T();
      return empty;
    }
  }

  uint8_t m_stat_overflow = 0;

private:
  static const uint8_t c_max_buffer_size = 10;
  T m_elements_buffer[c_max_buffer_size];
  uint8_t m_buffer_writer = 0;
  uint8_t m_buffer_reader = 0;
};





#endif