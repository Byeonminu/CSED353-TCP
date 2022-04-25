#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const{
     return _time - _last_segment_time; 
}

void TCPConnection::segment_received(const TCPSegment &seg) {
    _last_segment_time = _time;

    const TCPHeader &header = seg.header();

     //if the RST (reset) flag is set, sets both the inbound and outbound streams to the error state and kills the connection permanently.
    if (header.rst) {
        _rst = true;
        _receiver.stream_out().set_error();
        _sender.stream_in().set_error();
        return;
    }

    if (!_receiver.ackno().has_value() && (seg.length_in_sequence_space() == 0)) return;
    

   
    if (_receiver.stream_out().input_ended() && !_sender.fin_sent()) _linger_after_streams_finish = false;


    bool ack_no_check = true;
    if (header.ack) ack_no_check = _sender.ack_received(header.ackno, header.win);
    else  _sender.fill_window();
    
    bool payload_check = _receiver.segment_received(seg);
    if ((seg.length_in_sequence_space() == 0) && payload_check && ack_no_check ) return;

    if (_sender.segments_out().empty() && _receiver.ackno().has_value()) _sender.send_empty_segment();
    
    _segment_out();
}

bool TCPConnection::active() const {
    if (_rst == true) return false;
    
    if (!_sender.fin_sent() || !_sender.fin_acked() || !_receiver.stream_out().input_ended()) return true;
    
    
    //! Should the TCPConnection stay active (and keep ACKing)
    //! for 10 * _cfg.rt_timeout milliseconds after both streams have ended,
    if (_linger_after_streams_finish) {
        if (time_since_last_segment_received() >= 10 * _cfg.rt_timeout) return false;  // connection closed
         else return true;
    } 
    return false;
}

size_t TCPConnection::write(const string &data) {
    size_t _size = _sender.stream_in().write(data);
    _sender.fill_window();
    _segment_out();
    return _size;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _sender.tick(ms_since_last_tick);
    _time += ms_since_last_tick;

    // abort the connection, and send a reset segment to the peer
    if (_sender.consecutive_retransmissions() > _cfg.MAX_RETX_ATTEMPTS) {
        _rst = true;
        _receiver.stream_out().set_error();
        _sender.stream_in().set_error();
        _rst_segment();
    }
    _segment_out();
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    _segment_out();
}

void TCPConnection::connect() {
    _sender.fill_window();
    _segment_out();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
            _rst_segment();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}


void TCPConnection::receiver_info(TCPSegment &seg) {
    if (_receiver.ackno().has_value()) {
        seg.header().ack = true;
        seg.header().ackno = _receiver.ackno().value();
        seg.header().win = _receiver.window_size();
    }
}

void TCPConnection::_segment_out(void) {
    while (!_sender.segments_out().empty()) {
        TCPSegment &seg = _sender.segments_out().front();
        receiver_info(seg);
        _segments_out.push(seg);
        _sender.segments_out().pop();
    }
}

void TCPConnection::_rst_segment(void) {
    if (_sender.segments_out().empty()) _sender.send_empty_segment();
    
    TCPSegment &segment = _sender.segments_out().front();

    receiver_info(segment);
    segment.header().rst = true;  

    _segments_out.push(segment);
    _sender.segments_out().pop();
}