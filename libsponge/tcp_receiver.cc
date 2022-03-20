#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    TCPHeader header = seg.header();

    //isnset false
    if (!_isnset){
        if (header.syn){
            _isn = header.seqno;
            _isnset = true;
            _finset = header.fin;
        } else if(_finset) return true; 
        else return false;
   
        _reassembler.push_substring(seg.payload().copy(), 0, _finset); 

        return true;
    } 
    else{ //isnset true

        uint64_t _checkpoint = _reassembler.stream_out().bytes_written(); //already written
        uint64_t abs_n = unwrap(header.seqno, _isn, _checkpoint); // absolute seqno

        // absolute seqno -1
        if (!header.syn) abs_n--;
        

       
        uint64_t seg_begin = unwrap(header.seqno, _isn, _checkpoint);
        uint64_t seg_end = seg_begin + seg.length_in_sequence_space() - 1;

 
        uint64_t win_begin= unwrap(ackno().value(), _isn, _checkpoint);
        uint64_t win_end = win_begin + window_size();
        if(window_size()) win_end--;
        

        bool inside;
        if(win_begin >= seg_end || seg_begin >= win_end) inside = false;
        else inside = true;

     
        _reassembler.push_substring(seg.payload().copy(), abs_n, header.fin);

        if(header.fin && !_finset) _finset = true;
        
        return inside;
    }
}


//Returns an optional<WrappingInt32> containing the sequence number of the first byte that the receiver doesn’t already know
optional<WrappingInt32> TCPReceiver::ackno() const { 
   if(_isnset){
    uint32_t isend = _finset && !_reassembler.unassembled_bytes();
    uint64_t ab_seqno = 1 + _reassembler.stream_out().bytes_written() + static_cast<uint64_t>(isend); // syn + already written bytes + (fin)
    return wrap(ab_seqno, _isn);

   }
   else return {};

}

size_t TCPReceiver::window_size() const { return stream_out().remaining_capacity(); }



