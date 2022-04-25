#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <iostream>
#include <random>
// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , sendtimer(retx_timeout) {}

uint64_t TCPSender::bytes_in_flight() const{
     return byte_sent - byte_received; 
}

void TCPSender::fill_window() {
    TCPSegment seg;

    if (_eof) {
        return;
    }
    if (byte_sent == 0) {
        seg.header().syn = true;
    }

    size_t sequence_size = seg.length_in_sequence_space();
    uint16_t window_size = _window_size > 0 ? _window_size : 1;
    if (bytes_in_flight() + sequence_size > window_size || bytes_in_flight() >= window_size) {
        return;
    }
    size_t payload = window_size - bytes_in_flight() - sequence_size;
    if (payload > _stream.buffer_size()) {
        payload = _stream.buffer_size();
    }
    if (payload > TCPConfig::MAX_PAYLOAD_SIZE) {
        payload = TCPConfig::MAX_PAYLOAD_SIZE;
    }

    if (!_stream.eof() && sequence_size + payload == 0 ) {
        return;
    }

    seg.header().seqno = next_seqno();
    seg.payload() = Buffer(_stream.read(payload));
   

    if (window_size > bytes_in_flight() + seg.length_in_sequence_space()) {
        seg.header().fin = _stream.eof();
        _eof = _stream.eof();
        
    }

    _segments_out.push(seg);
    unacked.push(seg);
    byte_sent += seg.length_in_sequence_space();

    if (!sendtimer.active()) {
        sendtimer.rto_reset(_initial_retransmission_timeout);
        sendtimer.start(_time);
    }

    fill_window();
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
//! \returns `false` if the ackno appears invalid (acknowledges something the TCPSender hasn't sent yet)
bool TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t abs_ack_no = unwrap(ackno, _isn, byte_received);
    _window_size = window_size;
    if (abs_ack_no > byte_sent) {
        return false;
    }
    if (abs_ack_no < byte_received) {
        return true;
    }

    if (abs_ack_no == byte_received) {
        fill_window();
        return true;
    }
    
    sendtimer.rto_reset(_initial_retransmission_timeout);
    _consecutive_retrans_v = 0;
    byte_received = abs_ack_no;

    while (!unacked.empty()) {
        TCPSegment &seg = unacked.front();
        uint64_t abs_seq_no = unwrap(seg.header().seqno, _isn, byte_received);
        if (abs_seq_no + seg.length_in_sequence_space() <= abs_ack_no) {
            unacked.pop();
        } 
        else {
            break;
        }
    }

    if (!unacked.empty()) {
        sendtimer.start(_time);
    }
    fill_window();

    return true;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    _time += ms_since_last_tick;

    if (sendtimer.active()) {
    
        sendtimer.update_time(_time);
        if (sendtimer.timeout()) {
            _segments_out.push(unacked.front());
            _consecutive_retrans_v++;
            sendtimer.rto_double();
            sendtimer.start(_time);
        }
    }
    return;
}

unsigned int TCPSender::consecutive_retransmissions() const{
     return _consecutive_retrans_v; 
}

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    seg.header().seqno = next_seqno();
    _segments_out.push(seg);
}