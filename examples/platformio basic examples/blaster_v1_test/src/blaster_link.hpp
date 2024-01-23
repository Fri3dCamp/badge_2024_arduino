#ifndef BLASTER_HPP
#define BLASTER_HPP

#include <stdint.h>
#include "Arduino.h"
#include "blaster_packet.hpp"

// define this to record the received bit timings
// #define BLASTER_LINK_DEBUG_TIMINGS

enum LinkState : uint8_t
{
    eLinkStateIdle = 0,
    eLinkStateReceiving = 1,
    eLinkStateSending = 2,
    eLinkStateWaitForAck = 3,
    eLinkStateWaitForAckReady = 4
};

enum ReaderState : uint8_t
{
    eReaderStateWaitForStart = 1,
    eReaderStateReadBits = 2,
    eReaderStateWaitForStop = 3,
    eReaderStateComplete = 4,
    eReaderStateError = 5
};

enum ReaderDecode : char
{
    eReaderDecodeStart = 'S',
    eReaderDecodeStop = 'P',
    eReaderDecodeOne = '1',
    eReaderDecodeZero = '0',
    eReaderDecodeUnknown = 'U'
};

//spinlock to protect critical sections
static portMUX_TYPE blaster_reader_raw_spinlock = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE blaster_reader_buffer_spinlock = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE blaster_reader_messages_spinlock = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE link_state_spinlock = portMUX_INITIALIZER_UNLOCKED;

volatile LinkState link_state = eLinkStateIdle;

/**
 * @brief Read bits from the blaster_link and transform them into a DataPacket
 * 
 *  interrupt driven on RISING edge, will put bits into m_raw_data and count in m_bits_read
 *  when a packet is complete it is timestamped and moved to m_raw_buffer (still in the interrupt)
 * 
 *  outside interrupt `process_buffer()` should be called to transform the timestamped packets into DataPackets and move them to m_messages_buffer
 *  from m_messages_buffer the DataPackets can then be fetched with `message_available` and `pop_message`.
 *  Some shortcuts are available for received Ack and Ack Ready messages when sending by checking link_state
 * 
 */
class BlasterReader {
public:
    BlasterReader() {}

    static const int total_bits = 16;

    void reset()
    {
        
        m_state = eReaderStateWaitForStart;

        m_raw_data = 0;
        m_bits_read = 0;

#ifdef BLASTER_LINK_DEBUG_TIMINGS
        m_decoded_writer = 0;
        m_delta_writer = 0;
#endif
    }

    void move_to_buffer()
    {
        TimedRawData t;
        t.raw_data = m_raw_data;
        t.time_micros = esp_timer_get_time();
        this->m_raw_buffer.add_message(t);

        m_stat_received += 1;

        reset();
    }

    /**
     * @brief process from the raw buffer and store valid messages in the message buffer.
     *        Also sets the ACK flag if an ack message was found
     */
    void process_buffer()
    {
        taskENTER_CRITICAL(&blaster_reader_buffer_spinlock);
        bool message_available = this->m_raw_buffer.message_available();
        taskEXIT_CRITICAL(&blaster_reader_buffer_spinlock);
        
        while (message_available)
        {
            taskENTER_CRITICAL(&blaster_reader_buffer_spinlock);
            DataPacket packet = DataPacket(this->m_raw_buffer.pop_message());
            taskEXIT_CRITICAL(&blaster_reader_buffer_spinlock);
        
            // char packet_str[120];
            // packet.print(packet_str, 120);
            // log_d("BLASTER_LINK: %s", packet_str);
            if (packet.calculate_crc() == packet.get_crc())
            {
                if (packet.get_command() == eCommandBlasterAck && packet.get_parameter() == eParameterBlasterNotReady)
                {
                    m_ack_state = true;
                    taskENTER_CRITICAL(&link_state_spinlock);
                    link_state = eLinkStateWaitForAckReady;
                    taskEXIT_CRITICAL(&link_state_spinlock);

                }
                else if (packet.get_command() == eCommandBlasterAck && packet.get_parameter() == eParameterBlasterReady)
                {
                    m_ack_state = true;
                    taskENTER_CRITICAL(&link_state_spinlock);
                    link_state = eLinkStateIdle;
                    taskEXIT_CRITICAL(&link_state_spinlock);
                }
                else if (packet.get_command() == eCommandTeamChange && packet.get_parameter() == eParameterTeamChangeHardware)
                {
                    m_hardware_team = packet.get_team();
                }
                else
                {
                    taskENTER_CRITICAL(&blaster_reader_messages_spinlock);
                    this->m_messages_buffer.add_message(packet);
                    taskEXIT_CRITICAL(&blaster_reader_messages_spinlock);
                }
            }
            else
            {
                // crc failed
                m_stat_crc_failed += 1;
            }

            taskENTER_CRITICAL(&blaster_reader_buffer_spinlock);
            message_available = this->m_raw_buffer.message_available();
            taskEXIT_CRITICAL(&blaster_reader_buffer_spinlock);
        }
    }

    bool message_available() {
        taskENTER_CRITICAL(&blaster_reader_messages_spinlock);
        bool message_available = this->m_messages_buffer.message_available();
        taskEXIT_CRITICAL(&blaster_reader_messages_spinlock);

        return message_available;
    }

    DataPacket pop_message() {
        taskENTER_CRITICAL(&blaster_reader_messages_spinlock);
        DataPacket message = this->m_messages_buffer.pop_message();
        taskEXIT_CRITICAL(&blaster_reader_messages_spinlock);

        return message;
    }

#ifdef BLASTER_LINK_DEBUG_TIMINGS
    void save_deltas(bool save)
    {
        m_save_deltas = save;
    }

    void add_delta(int64_t delta)
    {
        if (m_save_deltas)
        {
            m_deltas[m_delta_writer] = delta;
            m_delta_writer += 1;
        }
    }

    void print_deltas(char* dest, size_t size)
    {
        if (m_save_deltas)
        {
            strcat(dest, "deltas: ");
            for (uint8_t i = 0; i < m_delta_writer; i++)
            {
                snprintf(dest + strlen(dest), size - strlen(dest), "%jd", m_deltas[i]);
                if (i < m_delta_writer - 1)
                {
                    strcat(dest, ", ");
                }
            }
        }
    }

    void save_decoded(bool save)
    {
        m_save_decoded = save;
    }

    void add_decoded(char decoded)
    {
        if (m_save_decoded)
        {
            m_decoded[m_decoded_writer] = decoded;
            m_decoded_writer += 1;
        }
    }

    void print_decoded(char* dest, size_t size)
    {
        if (m_save_decoded)
        {
            strcat(dest, "pa: ");
            for (uint8_t i = 0; i < m_decoded_writer; i++)
            {
                snprintf(dest + strlen(dest), size - strlen(dest), "%jd", m_decoded[i]);
                if (i < m_decoded_writer - 1)
                {
                    strcat(dest, ", ");
                }
            }
        }
    }
#endif
    // used for checking if a send was successful
    bool m_ack_state = false; // if an ACK was present in the last processed messages

    TeamColor m_hardware_team = eNoTeam;

    // to fetch the raw bits from the blaster-link
    ReaderState m_state = eReaderStateWaitForStart;
    int64_t m_ref_time = 0;
    uint16_t m_raw_data = 0;
    uint8_t m_bits_read = 0;

    uint16_t m_stat_received = 0;
    uint16_t m_stat_crc_failed = 0;

private:
    RingBuffer<TimedRawData> m_raw_buffer = RingBuffer<TimedRawData>();

    RingBuffer<DataPacket> m_messages_buffer = RingBuffer<DataPacket>();

#ifdef BLASTER_LINK_DEBUG_TIMINGS
    // deltas can be used to debug the timings
    // saved for 1 packet
    bool m_save_deltas = false;
    int64_t m_deltas[total_bits + 2];
    uint8_t m_delta_writer = 0;

    // decoded contains deltas decoded to start, stop, 0, 1
    // saved for 1 packet
    bool m_save_decoded = false;
    char m_decoded[total_bits + 2];
    uint8_t m_decoded_writer = 0;
#endif
};

BlasterReader br = BlasterReader();

// timings JVC remote
// 1 packet: 1 start + 16 bits + stop
//
// 1 timeslot is 38kHz * 20 = 526us
//
// start = 16 high 8 low
// 1 = 1 high 3 low
// 0 = 1 high 1 low
// stop = 1 high 1 low
//
//*****************************************************************//
//             ISR                                                 //
// measure the timing between the RISING edges                     //
// when full JVC remote code is received, move it to buffer        //
//*****************************************************************//
void IRAM_ATTR handle_blaster_isr()
{
    int64_t t = esp_timer_get_time();

    taskENTER_CRITICAL_ISR(&link_state_spinlock);
    link_state = eLinkStateReceiving;
    taskEXIT_CRITICAL_ISR(&link_state_spinlock);

    taskENTER_CRITICAL_ISR(&blaster_reader_raw_spinlock);
    int64_t delta = t - br.m_ref_time;
    br.m_ref_time = t;
    taskEXIT_CRITICAL_ISR(&blaster_reader_raw_spinlock);

    ReaderDecode decoded = eReaderDecodeUnknown;

    if ((delta > 421) && (delta < 632)) // 1 timeslot 526 µs +/- 20% ==> stop
    {
        decoded = eReaderDecodeStop;
    }
    else if ((delta) > 840 && (delta < 1313)) // 2 timeslot 1052 µs +/- 20% ==> 0
    {
        decoded = eReaderDecodeZero;
        taskENTER_CRITICAL_ISR(&blaster_reader_raw_spinlock);
        if (br.m_state == eReaderStateReadBits)
        {
            br.m_raw_data = br.m_raw_data >> 1;
            br.m_bits_read += 1;
        }
        taskEXIT_CRITICAL_ISR(&blaster_reader_raw_spinlock);
    }
    else if ((delta > 1680) && (delta < 2625)) // 4 timeslots 2100 µs +/- 20% ==> 1
    {
        decoded = eReaderDecodeOne;
        taskENTER_CRITICAL_ISR(&blaster_reader_raw_spinlock);
        if (br.m_state == eReaderStateReadBits)
        {
            br.m_raw_data = (1 << 15) | (br.m_raw_data >> 1);
            br.m_bits_read += 1;
        }
        taskEXIT_CRITICAL_ISR(&blaster_reader_raw_spinlock);
    }
    else if (delta > 3789) // 8+1 timeslot 4737 µs - 20% ==> start
    {
        decoded = eReaderDecodeStart;
    }
    else
    {
        decoded = eReaderDecodeUnknown;
    }

#ifdef BLASTER_LINK_DEBUG_TIMINGS
    br.add_delta(delta);
    br.add_decoded(decoded);
#endif

    taskENTER_CRITICAL_ISR(&blaster_reader_raw_spinlock);
    switch (br.m_state)
    {
    case eReaderStateWaitForStart:
        if (decoded == eReaderDecodeStart)
        {
            br.m_state = eReaderStateReadBits;
        }
        else
        {
            br.m_state = eReaderStateError;
        }
        break;
    case eReaderStateReadBits:
        if (decoded == eReaderDecodeOne || decoded == eReaderDecodeZero)
        {
            if (br.m_bits_read == br.total_bits)
            {
                br.m_state = eReaderStateWaitForStop;
            }
        }
        else
        {
            br.m_state = eReaderStateError;
        }
        break;
    case eReaderStateWaitForStop:
        if (decoded == eReaderDecodeStop)
        {
            br.m_state = eReaderStateComplete;
        }
        else
        {
            br.m_state = eReaderStateError;
        }
        break;
    } // switch

    if (br.m_state == eReaderStateError)
    {
        br.reset();
        taskENTER_CRITICAL_ISR(&link_state_spinlock);
        link_state = eLinkStateIdle;
        taskEXIT_CRITICAL_ISR(&link_state_spinlock);
    }

    if (br.m_state == eReaderStateComplete)
    {
        br.move_to_buffer();
        taskENTER_CRITICAL_ISR(&link_state_spinlock);
        link_state = eLinkStateIdle;
        taskEXIT_CRITICAL_ISR(&link_state_spinlock);
    }
    taskEXIT_CRITICAL_ISR(&blaster_reader_raw_spinlock);
}



/**
 * @brief uses BlasterReader to read raw bits and make DataPackets
 *        `start_listen` to start listening for incoming blaster raw_messages and call regulary `process_buffer` to transform them to DataPackets
 *        the DataPackets can then be fetched with `message_available` and `pop_message`
 */
class BlasterLink
{
public:
    /**
     * @brief Construct a new Blaster Link object
     * 
     * @param blaster_link_pin the pin to use for blaster communication
     */
    BlasterLink(const uint8_t blaster_link_pin): blaster_link_pin(blaster_link_pin) {}

    /**
     * @brief Start listening for messages on the `blaster_link_pin` by attaching the interrupt.
     *        You need to call `process_buffer` periodically to process the messages from the raw buffer to the message buffer.
     */
    void start_listen()
    {
        pinMode(this->blaster_link_pin, INPUT_PULLUP);
        attachInterrupt(this->blaster_link_pin, handle_blaster_isr, RISING);
    }

    /**
     * @brief Stop listening by detaching the interrupt service routine from `blaster_link_pin`
     */
    void stop_listen()
    {
        while (br.m_state != eReaderStateWaitForStart)
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        detachInterrupt(this->blaster_link_pin);
    }

    /**
     * @brief process from the raw buffer and store valid messages in the message buffer.
     *        Also sets the ACK flag if an ack message was found
     */
    static void process_buffer() {
        br.process_buffer();
    }

    /**
     * @brief Are there messages available in the ringbuffer.
     */
    static bool message_available() {
        return br.message_available();
    }

    /**
     * @brief Return the first available message from the ringbuffer.
     *        Also makes a spot available in the ringbuffer
     */
    static DataPacket pop_message() {
        return br.pop_message();
    }

    /**
     * @brief Sets the IR channel (0..15).
     * Only blasters on the same IR channel can communicate via IR.
     *
     * @param channel_id the ir-channel to use
     * @return true setting channel acknowledged by blaster
     * @return false setting channel not acknowledged by blaster
     */
    bool set_channel(uint8_t channel_id)
    {
        if (channel_id > 15)
            return false;

        DataPacket p = DataPacket();
        p.set_command(eCommandSetChannel);
        p.set_parameter(channel_id);
        p.calculate_crc(true);

        bool success = send_to_blaster_retry(p);
        return success;
    }

    /**
     * @brief Sets various trigger action flags
     * 
     * @param stealth silent shooting
     * @param single_shot only 1 shot allowed
     * @param healing true: healing shot, false damage shot
     * @param disable true: no shooting allowed, false shooting allowed (damage or healing)
     * @return true acknowledged by blaster
     * @return false not acknowledged by blaster
     */
    bool set_trigger_action(bool stealth=false, bool single_shot=false, bool healing=false, bool disable=false) {
        DataPacket p = DataPacket();
        p.set_command(eCommandSetTriggerAction);
        uint8_t parameter = 0;
        if (disable) parameter += 1;
        if (healing) parameter += 2;
        if (single_shot) parameter += 4;
        if (stealth) parameter += 8;
        p.set_parameter(parameter);
        p.calculate_crc(true);

        bool success = send_to_blaster_retry(p);
        return success;
    }

    /**
     * @brief Sets and locks the blaster team
     * 
     * @param team 0 (eNoTeam): release team lock
     *             1..15: lock blaster team. When this is set the blaster team will be locked to this team.
     *                    This means that the hardware team switch is not working anymore.
     *                    The blaster team can still change when playing in zombi mode.
     * @return true acknowledged by blaster
     * @return false not acknowledged by blaster
     */
    bool set_team(TeamColor team) {
        DataPacket p = DataPacket();
        p.set_command(eCommandTeamChange);
        p.set_team(team);
        p.calculate_crc(true);

        bool success = send_to_blaster_retry(p);
        return success;
    }

    /**
     * @brief Set the game mode
     *        Mode 0: Timeout mode. (Default)
     *                When this mode is set the blaster will go in timeout mode when being shot.
     *                After a set time the blaster will auto heal and can continue the game.
     *                If the blaster receives a healing shot this timeout can be shortened.
     *        Mode 1: Zombie mode
     *                In this mode the blaster team will change to the color of the team that shot it.
     *                The game is over when all players are of the same color.
     *        Mode 2: Sudden death mode
     *                The blaster is very weak and will stop working after being shot once.
     *                The winner is the last player with a working blaster
     * 
     * @param mode the GameMode
     * @return true acknowledged by blaster
     * @return false not acknowledged by blaster
     */
    bool set_game_mode(GameMode mode) {
        // TODO: check if team is changed to 0 on blaster when not set in this packet
        DataPacket p = DataPacket();
        p.set_command(eCommandSetGameMode);
        p.calculate_crc(true);
        
        bool success = send_to_blaster_retry(p);
        return success;
    }

    /**
     * @brief play an animation on the blaster
     * 
     * @param animation the animation to play
     * @return true acknowledged by blaster
     * @return false not acknowledged by blaster
     */
    bool play_animation(AnimationNames animation) {
        DataPacket p = DataPacket();
        p.set_command(eCommandPlayAnimation);
        p.calculate_crc(true);

        bool success = send_to_blaster_retry(p);
        return success;
    }

    /**
     * @brief Set the hit timeout: the timeout in seconds the blaster is not able to shoot or get shot
     * 
     * @param timeout in seconds
     * @return true acknowledged by blaster
     * @return false not acknowledged by blaster
     */
    bool set_hit_timeout(uint8_t timeout) {
        if (timeout > 15) return false;
        DataPacket p = DataPacket();
        p.set_command(eCommandSetHitTimeout);
        p.set_parameter(timeout);
        p.calculate_crc(true);

        bool success = send_to_blaster_retry(p);
        return success;
    }

    /**
     * @brief This is a gimmick and not a game mechanic
     *        When the badge sends this message to the blaster it initiates a blaster chatter session.
     *        The blaster that receives this message will start "talking" blaster language then transmit the chatter message over IR.
     *        Blasters who receive this message will also start "talking" and retransmitting.
     * 
     *        Every time a blaster retransmits a message is decreases the time to live field. When the TTL == 0 it will stop.
     *        A blaster will also ignore any packages with a higher TTL than the lowest TTL it has received until now.
     * 
     * @return true acknowledged by blaster
     * @return false not acknowledged by blaster
     */
    bool start_shatter() {
        DataPacket p = DataPacket();
        p.set_command(eCommandChatter);
        p.set_parameter(9);
        p.calculate_crc(true);

        bool success = send_to_blaster_retry(p);
        return success;
    }

    /**
     * @brief Set settings: mute and brightness
     * 
     * @param mute When set the blaster will be in mute mode
     * @param brightness Brightness (default = 7 (0b111)) This will change the brightness of all LEDS on the blaster.
     * @return `true` acknowledged by blaster
     * @return `false` not acknowledged by blaster
     */
    bool set_settings(bool mute = false, uint8_t brightness = 7) {
        if (brightness > 7) brightness = 7;
        DataPacket p = DataPacket();
        p.set_command(eCommandSetSettings);
        uint8_t parameter = (brightness << 1) | mute;
        p.set_parameter(parameter);
        p.calculate_crc(true);

        bool success = send_to_blaster_retry(p);
        return success;        
    }

private:

    uint8_t blaster_link_pin;

    static const uint16_t JVC_TIMESLOT_MICROS = 526; // timings JVC remote: 1 timeslot is 1 / 38kHz * 20 = 526us
    static const uint16_t ir_start_high_time = JVC_TIMESLOT_MICROS * 16;
    static const uint16_t ir_start_low_time = JVC_TIMESLOT_MICROS * 8;
    static const uint16_t ir_zero_high_time = JVC_TIMESLOT_MICROS * 1;
    static const uint16_t ir_zero_low_time = JVC_TIMESLOT_MICROS * 1;
    static const uint16_t ir_one_high_time = JVC_TIMESLOT_MICROS * 1;
    static const uint16_t ir_one_low_time = JVC_TIMESLOT_MICROS * 3;
    static const uint16_t ir_stop_high_time = JVC_TIMESLOT_MICROS * 1;
    static const uint16_t ir_stop_low_time = JVC_TIMESLOT_MICROS * 1;

    bool send_to_blaster(DataPacket packet)
    {
        uint8_t waitCount = 0;

        taskENTER_CRITICAL(&link_state_spinlock);
        bool link_is_not_idle = link_state != eLinkStateIdle;
        taskEXIT_CRITICAL(&link_state_spinlock);

        while (link_is_not_idle)
        {
            // minimal packet transmit time is 58 * 526us = 30508us
            // wait for half = 15ms
            log_d("send_to_blaster :: link not idle");
            vTaskDelay(15 / portTICK_PERIOD_MS);
            waitCount += 1;
            if (waitCount > 3)
            {
                log_d("send_to_blaster :: failed getting idle blaster link");
                return false;
            }

            taskENTER_CRITICAL(&link_state_spinlock);
            link_is_not_idle = link_state != eLinkStateIdle;
            taskEXIT_CRITICAL(&link_state_spinlock);
        }
        // stop listening on the blaster link, shared pin
        stop_listen();
        taskENTER_CRITICAL(&link_state_spinlock);
        link_state = eLinkStateSending;
        taskEXIT_CRITICAL(&link_state_spinlock);

        // configure the pin for sending
        pinMode(this->blaster_link_pin, OUTPUT);

        // initialize esp32 rmt
        rmt_obj_t* rmt_send = NULL;
        if ((rmt_send = rmtInit(this->blaster_link_pin, RMT_TX_MODE, RMT_MEM_64)) == NULL)
        {
            log_d("send_to_blaster :: init sender failed");
            
            taskENTER_CRITICAL(&link_state_spinlock);
            link_state = eLinkStateIdle;
            taskEXIT_CRITICAL(&link_state_spinlock);
            
            start_listen();
            return false;
        }

        // set timing interval to 1 microsecond
        const float one_micro_second_tick = 1000.0f;
        float realTick = rmtSetTick(rmt_send, one_micro_second_tick); // 1us tick
        if (realTick != one_micro_second_tick)
        {
            log_e("send_to_blaster :: rmtSetTick failed expected: %fns realTick: %fns", one_micro_second_tick, realTick);
            rmtDeinit(rmt_send);

            taskENTER_CRITICAL(&link_state_spinlock);
            link_state = eLinkStateIdle;
            taskEXIT_CRITICAL(&link_state_spinlock);

            start_listen();
            return false;
        }

        // prepare the data
        static const uint8_t NR_OF_ALL_BITS = 18; // 1 start + 16 bit data + 1 stop
        rmt_data_t ir_data[NR_OF_ALL_BITS];
        ir_data[0].duration0 = ir_start_high_time;
        ir_data[0].level0 = 1;
        ir_data[0].duration1 = ir_start_low_time;
        ir_data[0].level1 = 0;

        for (uint8_t i = 0; i < 16; i++)
        {
            if (bitRead(packet.get_raw(), i))
            {
                ir_data[1 + i].duration0 = ir_one_high_time;
                ir_data[1 + i].level0 = 1;
                ir_data[1 + i].duration1 = ir_one_low_time;
                ir_data[1 + i].level1 = 0;
            }
            else
            {
                ir_data[1 + i].duration0 = ir_zero_high_time;
                ir_data[1 + i].level0 = 1;
                ir_data[1 + i].duration1 = ir_zero_low_time;
                ir_data[1 + i].level1 = 0;
            }
        }
        ir_data[17].duration0 = ir_stop_high_time;
        ir_data[17].level0 = 1;
        ir_data[17].duration1 = ir_stop_low_time;
        ir_data[17].level1 = 0;

        // send the prepared data
        char s[120];
        packet.print(s, 120);
        log_d("send_to_blaster :: Sending data %s", s);
        bool success = rmtWriteBlocking(rmt_send, ir_data, NR_OF_ALL_BITS);
        if (!success)
        {
            log_e("send_to_blaster :: rmtWriteBlocking failed.");
            rmtDeinit(rmt_send);

            taskENTER_CRITICAL(&link_state_spinlock);
            link_state = eLinkStateIdle;
            taskEXIT_CRITICAL(&link_state_spinlock);

            start_listen();
            return false;
        }

        // check for ACK within 100ms
        // clear the br ack_state
        br.m_ack_state = false;

        // listen on the blaster link again
        taskENTER_CRITICAL(&link_state_spinlock);
        link_state = eLinkStateWaitForAck;
        taskEXIT_CRITICAL(&link_state_spinlock);
    
        start_listen();

        // clean up the esp32 rmt
        rmtDeinit(rmt_send);

        // wait at least 100ms for Ack
        vTaskDelay(135 / portTICK_PERIOD_MS);
        br.process_buffer();
        
        if (br.m_ack_state == false)
        {
            log_w("send_to_blaster :: no Ack received.");

            taskENTER_CRITICAL(&link_state_spinlock);
            link_state = eLinkStateIdle;
            taskEXIT_CRITICAL(&link_state_spinlock);

            return false;
        }
        else
        {
            bool send_message = false;
            taskENTER_CRITICAL(&link_state_spinlock);
            if (link_state == eLinkStateWaitForAckReady)
            {
                send_message = true;
                link_state = eLinkStateIdle;
            }
            taskEXIT_CRITICAL(&link_state_spinlock);

            if (send_message) {
                // console print outside CRITICAL section
                log_w("send_to_blaster :: no Ack Ready received.");
            }
            // packet was Acked, but blaster did not send Ack Ready
            return true;
        }
    }

    bool send_to_blaster_retry(DataPacket packet, uint8_t retries = 5)
    {
        uint8_t retryCount = 0;
        while (retryCount < retries)
        {
            bool success = send_to_blaster(packet);
            if (!success)
            {
                retryCount += 1;
                vTaskDelay(50 * retryCount / portTICK_PERIOD_MS); // sleep
                log_d("send_to_blaster :: retrying: %d/%d", retryCount, retries);
            }
            else
            {
                return true;
            }
        }
        log_e("send_to_blaster :: retryCount exceeded");
        return false;
    }
};


#endif