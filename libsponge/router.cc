#include "router.hh"

#include <iostream>

using namespace std;

// Dummy implementation of an IP router

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

// For Lab 6, please replace with a real implementation that passes the
// automated checks run by `make check_lab6`.

// You will need to add private members to the class declaration in `router.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    routing_table.push_back(Routingtable(route_prefix, prefix_length, next_hop, interface_num));

}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {

    
    size_t size = routing_table.size();
    int max = -1;
    size_t longest_prefix_match;
    size_t _dst = dgram.header().dst;
    
    for (size_t i = 0; i < size; i++){
        auto object = routing_table[i];
        size_t postfix_length = 32 - object.prefix_length;
        size_t prefix, dst_temp;
        if(object.prefix_length > 0) prefix = object.route_prefix >> postfix_length;
        else prefix = 0;
         if(object.prefix_length > 0) dst_temp = _dst >> postfix_length;
        else dst_temp = 0;

        if (prefix == dst_temp && max < object.prefix_length) {
            longest_prefix_match = i;
            max = object.prefix_length;
        }
    }

    if (max != -1){ // router matched
        if (dgram.header().ttl == 0 || (dgram.header().ttl - 1) == 0) // If the TTL was zero already, or hits zero after the decrement, the router should drop the datagram
            return;


        auto matched_object = routing_table[longest_prefix_match];
        if (matched_object.next_hop.has_value()) {
            Address next_hop_temp = matched_object.next_hop.value();
            interface(matched_object.interface_num).send_datagram(dgram, next_hop_temp);
        }
        else{
            Address next_hop_temp = Address::from_ipv4_numeric(_dst);
            interface(matched_object.interface_num).send_datagram(dgram, next_hop_temp);
        }
    }

    return;
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
