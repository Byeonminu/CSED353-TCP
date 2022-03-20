#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.
#include <cmath>
template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    //absolute seqno -> seqno
    uint64_t temp = n + isn.raw_value();
    temp = temp % static_cast<uint64_t>(pow(2,32));
    WrappingInt32 ret((uint32_t(temp)));
    return ret;
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    // Convert seqno -> absolute seqno
    WrappingInt32 sqenocheck = wrap(checkpoint, isn); // absolute seqno인 checkpoint를 seqno로 바꾼다

    uint64_t difference1 = n.raw_value() - sqenocheck.raw_value();
    uint64_t difference2 = sqenocheck.raw_value() - n.raw_value();
    
    if (difference1 > difference2)
    {
        if(checkpoint >= difference2) return checkpoint - difference2;
        else return checkpoint + difference1;
       
    }
    
      return checkpoint + difference1;
        
   
}
