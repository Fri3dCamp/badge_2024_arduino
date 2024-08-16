#include <Arduino.h>
#include "pins_arduino.h"

#include "data.h"

IrDataPacket::IrDataPacket(){};
IrDataPacket::IrDataPacket(uint32_t raw){this->raw=raw;};

uint32_t IrDataPacket::get_raw()         { return this->raw; }
uint8_t IrDataPacket::get_channel()      { return (this->raw & 0b00000000000000000000000000000001) >> 0; }
uint8_t IrDataPacket::get_team()         { return (this->raw & 0b00000000000000000000000000001110) >> 1; }
uint8_t IrDataPacket::get_action()       { return (this->raw & 0b00000000000000000000000000110000) >> 4; }
uint8_t IrDataPacket::get_action_param() { return (this->raw & 0b00000000000000000000001111000000) >> 6; }
uint16_t IrDataPacket::get_player_id()   { return (this->raw & 0b00000000001111111111110000000000) >> 10; }
uint8_t IrDataPacket::get_crc()          { return (this->raw & 0b00111111110000000000000000000000) >> 22; }
uint8_t IrDataPacket::get_unused()       { return (this->raw & 0b11000000000000000000000000000000) >> 30; }

void IrDataPacket::set_raw(uint32_t raw)                  { this->raw = raw; }
void IrDataPacket::set_channel(uint8_t channel)           { this->raw &= ~(0b1 << 0); this->raw |= (channel & 0b1) << 0;}
void IrDataPacket::set_team(uint8_t team)                 { this->raw &= ~(0b111 << 1); this->raw |= (team & 0b111) << 1;}
void IrDataPacket::set_action(uint8_t action)             { this->raw &= ~(0b11 << 4); this->raw |= (action & 0b11) << 4;}
void IrDataPacket::set_action_param(uint8_t action_param) { this->raw &= ~(0b1111 << 6); this->raw |= (action_param & 0b1111) << 6;}
void IrDataPacket::set_player_id(uint16_t player_id)      { this->raw &= ~(0b111111111111 << 10); this->raw |= (player_id & 0b111111111111) << 10;}
void IrDataPacket::set_crc(uint8_t crc)                   { this->raw &= ~(0b11111111 << 22); this->raw |= (crc & 0b11111111) << 22;}
void IrDataPacket::set_unused(uint8_t unused)             { this->raw &= ~(0b11 << 30); this->raw |= (unused & 0b11) << 30;}

/* #region DataReader */
void IRAM_ATTR DataReader::handlePinChange()
{
    if (this->dataReady)
        return; // don't read more data until the "buffer" is empty

    unsigned long time = micros(); // check time passed since boot up.
    uint32_t delta_time = time - this->refTime;
    this->refTime = time;

    /* Check total pulse length (rising to rising edge) allow for some deviation*/
    if (delta_time > (uint32_t)(13500 * 0.8) && delta_time < (uint32_t)(13500 / 0.8))
    {
        this->bitsRead = 1;
        this->rawData = 0;
        return;
    }
    if (this->bitsRead == 0)
        return;

    if (delta_time > (uint32_t)(2250 * 0.8) && delta_time < (uint32_t)(2250 / 0.8))
    {
        this->rawData = this->rawData >> 1; // make room for an extra bit
        this->rawData |= 0x80000000;  // set left bit high
        if (++this->bitsRead == (ir_bit_lenght + 1))
        {
            this->dataReady = 1;
        }
    }
    else if (delta_time > (uint32_t)(1120 * 0.8) && delta_time < (uint32_t)(1120 / 0.8))
    {
        this->rawData = this->rawData >> 1; // make room for an extra bit
        if (++this->bitsRead == (ir_bit_lenght + 1))
        {
            this->dataReady = true;
        }
    }
}

void DataReader::reset()
{
    this->rawData = 0;
    this->bitsRead = 0;
    this->dataReady = 0;
}

bool DataReader::isDataReady()
{
    return this->dataReady;
}

uint32_t DataReader::getPacket()
{
    uint32_t p;
    p = this->rawData;
    this->reset();
    return p;
}

/* #endregion */

/* #region Data */

void isr_handler();

// Private
_data::_data()
{
    // ir1_reader.reset();

    // enableReceive();
}

void _data::enableReceive()
{
    pinMode(IR_IN1_PIN, INPUT);
    attachInterrupt(IR_IN1_PIN, isr_handler, FALLING);
}

void _data::disableReceive()
{
    detachInterrupt(IR_IN1_PIN);
    this->ir1_reader.reset();
}

// Public
IrDataPacket _data::readIr() // add overload to bypass command type validation?
{
    if (this->ir1_reader.isDataReady())
    {
        IrDataPacket p(ir1_reader.getPacket());
        // Serial.println(p.get_raw());
        p.set_raw(calculateCRC(p.get_raw()));
        if (p.get_crc() == 0)
        {
            return p;
        }
        else
        {
            Serial.printf("Wrong packet CRC: packet: %" PRIu32 ", CRC: %" PRIu8 "\n", p.get_raw(), p.get_crc());
        }
    }

    return IrDataPacket(0);
}

_data &_data::getInstance()
{
    static _data data;
    return data;
}

void _data::init()
{
    this->enableReceive();
}

uint32_t _data::calculateCRC(uint32_t raw_packet)
{
    uint32_t raw = raw_packet;
    uint32_t checksum = ((raw << 2) & 0b10000000111111110111111100) ^
                        ((raw << 1) & 0b01111111100000000111111110) ^
                        ((raw << 0) & 0b00000000111111111111111111) ^
                        ((raw >> 1) & 0b00000000100000000000000000) ^
                        ((raw >> 2) & 0b00000000011111110000000000) ^
                        ((raw >> 3) & 0b00000000111111111000000000) ^
                        ((raw >> 4) & 0b00000011100000001111111100) ^
                        ((raw >> 5) & 0b00000000111111111000000010);
    checksum = checksum ^ (checksum >> 8) ^ (checksum >> 16) ^ (checksum >> 24);
    checksum = checksum & 0xFF;
    raw ^= checksum << 24;
    return raw;
}

void IRAM_ATTR _data::receive_ISR()
{
    this->ir1_reader.handlePinChange();
}

/* #endregion */

_data &Data = Data.getInstance();


void IRAM_ATTR isr_handler()
{
    Data.receive_ISR();
}

