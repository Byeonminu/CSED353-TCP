#include "tcp_sender.hh"

#include "tcp_config.hh"

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
    ,sendtimer(static_cast<size_t>(_initial_retransmission_timeout)) {}

uint64_t TCPSender::bytes_in_flight() const{
     return bytes_in_flight_v;
}

void TCPSender::fill_window() {
    if (check_fin) // already fiiled
        return;
   
    //when window size is zero, this method should act like the window size is one.
    if(win_sizeof_sender == 0) win_sizeof_sender = 1;
 

    uint64_t win_begin = _next_seqno;
    uint64_t win_end;
    uint64_t _cur_seqno = _next_seqno;
    size_t upper;

    if(unacked.size() > 0) win_begin = unwrap(unacked.front().header().seqno, _isn, _next_seqno);

    win_end = win_begin + win_sizeof_sender - 1;



    while((!check_syn || _stream.eof() || !_stream.buffer_empty()) && _cur_seqno <= win_end){

        TCPSegment temp;
        upper = min(TCPConfig::MAX_PAYLOAD_SIZE, win_end - _cur_seqno + 1);

        
        if (!check_syn) {
            check_syn = true;
            temp.header().syn = true;
        }
            
       
        size_t remained = upper - temp.length_in_sequence_space();
        string data = _stream.read(remained);
        if ((data.size() + 1 <= remained) && _stream.eof()) {
            check_fin = true;
            temp.header().fin = true;
        }

        temp.header().seqno = next_seqno();
        temp.payload() = Buffer(static_cast<string>(data));
        

        bytes_in_flight_v += temp.length_in_sequence_space();
         _cur_seqno += temp.length_in_sequence_space();
        _next_seqno += temp.length_in_sequence_space();
        

        unacked.push(temp);
         if (unacked.size() == 1)
            sendtimer.begin();

        _segments_out.push(temp);
    
        if (check_fin)
            break;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
bool TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size){

    bool flag = false;
    uint64_t abs_no_of_ackno = unwrap(ackno, _isn, _next_seqno);
    
    win_sizeof_sender = window_size;
    
    if (_next_seqno < abs_no_of_ackno)
        return false;
        
   

    while (unacked.size() > 0) {
        TCPSegment unacked_seg = unacked.front();
        uint64_t abs_unacked_seg = unwrap(unacked_seg.header().seqno, _isn, _next_seqno);

        if ( abs_no_of_ackno  > abs_unacked_seg) {
            bytes_in_flight_v -= unacked_seg.length_in_sequence_space();  // sent but not acked
            unacked.pop();
            flag = true;

        } else break;
        
    }

 

    // When the receiver gives the sender an ackno that acknowledges the successful receipt of new data 
    if (flag){
        
        sendtimer.end();
        sendtimer.rto_reset();       
        _consecutive_retransmissions_v = 0;  
        
    }
 
    fill_window();
    return true;
    
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick){ 

   if(sendtimer.timer_active()){
       if(sendtimer.tick(ms_since_last_tick)) {
        _consecutive_retransmissions_v++;
        sendtimer.rto_double();
        sendtimer.end();
        sendtimer.begin();

        _segments_out.push(unacked.front());
        }
   }
   return;
      

    
}

unsigned int TCPSender::consecutive_retransmissions() const{ 
    return _consecutive_retransmissions_v; 
}

void TCPSender::send_empty_segment(){
    TCPSegment seg;
    seg.header().seqno = _isn;
    _segments_out.push(seg);
}
