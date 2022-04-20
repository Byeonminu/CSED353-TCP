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

size_t TCPConnection::time_since_last_segment_received() const { return _time - _last_segment_time; }

void TCPConnection::segment_received(const TCPSegment &seg) { 
    
    //if the RST (reset) flag is set, sets both the inbound and outbound streams to the error state and kills the connection permanently.
    if(seg.header().rst == true){
        _rst = true;
        _receiver.stream_out().set_error();
        _sender.stream_in().set_error();
        return;
    }

    bool ackno_check = true;
    if (seg.header().ack) {
        ackno_check = _sender.ack_received(seg.header().ackno, seg.header().win);
    } else {
        _sender.fill_window();
    }

    if (_receiver.stream_out().input_ended() && !_sender.fin_sent()) {
        _linger_after_streams_finish = false;
    }

    if ((seg.length_in_sequence_space() == 0) && _receiver.segment_received(seg) && ackno_check ) {
        return;
    }

    if (_receiver.ackno().has_value() && (seg.length_in_sequence_space() == 0) && seg.header().seqno == _receiver.ackno().value() - 1) {
        _sender.send_empty_segment(); 
    }
}

bool TCPConnection::active() const {
    if(_rst == true){
         return false; 
    }
   
}

size_t TCPConnection::write(const string &data) {
    DUMMY_CODE(data);
    return {};
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) { 
   _sender.tick(ms_since_last_tick);
    _time += ms_since_last_tick;

    // abort the connection, and send a reset segment to the peer
    if (_sender.consecutive_retransmissions() > _cfg.MAX_RETX_ATTEMPTS) {
        _reset = true;
        _receiver.stream_out().set_error();
         _sender.stream_in().set_error();

        if (_sender.segments_out().empty()) {
            _sender.send_empty_segment();
        }
        TCPSegment &seg = _sender.segments_out().front();
        
        seg.header().rst = true; 
        _segments_out.push(seg);
        _sender.segments_out().pop();
        
    }
    //segment.out push function
}

void TCPConnection::end_input_stream() {}

void TCPConnection::connect() {}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
