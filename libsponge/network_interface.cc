#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const size_t next_hop_ip = next_hop.ipv4_numeric();
    auto iter =ip_ethernet_map.find(next_hop_ip);

    EthernetFrame e_frame;
    e_frame.header().src =_ethernet_address;

    if ((_time - iter->second.time_ethernet) > 30000 || iter == ip_ethernet_map.end()){ // cant find or expired
        
        auto arp_iter = arp_req.begin();
        unsent.emplace_back(dgram, next_hop_ip);
        while (arp_iter->arp_addr != next_hop_ip && arp_iter != arp_req.end()) arp_iter++;


        if ((_time - arp_iter->time_arp) <= 5000){
            if (arp_iter != arp_req.end()) return ;
        }
        else{
            ARPMessage msg;
            msg.opcode = ARPMessage::OPCODE_REQUEST;
            msg.target_ip_address = next_hop_ip;
            msg.sender_ip_address = _ip_address.ipv4_numeric();
            msg.sender_ethernet_address = _ethernet_address;
          
            EthernetAddress temp = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
            e_frame.payload() = msg.serialize();
            e_frame.header().dst = temp;
            e_frame.header().type = EthernetHeader::TYPE_ARP;

            ARP req;
            req.time_arp = _time;
            req.arp_addr = next_hop_ip;
            arp_req.push_back(req);
        }
        _frames_out.push(e_frame);
    } 
    else{
        EthernetAddress eth_addr = iter->second.ethernet_address;
        e_frame.payload() = dgram.serialize();
        e_frame.header().type = EthernetHeader::TYPE_IPv4;
        e_frame.header().dst = eth_addr;
        _frames_out.push(e_frame);
    }
    
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    auto &header =frame.header();

    EthernetAddress temp = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if ((header.dst != temp) && (header.dst != _ethernet_address)) return {};
    else if (header.type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram datagrams;
        if (datagrams.parse(frame.payload()) == ParseResult::NoError) return datagrams;
        return {};
    }

    if (header.type == EthernetHeader::TYPE_ARP) {
        ARPMessage msg;
        if (msg.parse(frame.payload()) != ParseResult::NoError) return {};

        Ethernet _ethernet;
        _ethernet.ethernet_address = msg.sender_ethernet_address;
        _ethernet.time_ethernet = _time;
        ip_ethernet_map[msg.sender_ip_address] = _ethernet;

        bool isvalid_req = (msg.opcode == ARPMessage::OPCODE_REQUEST) && (msg.target_ip_address == _ip_address.ipv4_numeric());
        bool isvalid_res = (msg.opcode == ARPMessage::OPCODE_REPLY);
        if(isvalid_req){
            ARPMessage reply;
            EthernetFrame e_frame;

            reply.opcode = ARPMessage::OPCODE_REPLY;
            reply.target_ip_address = msg.sender_ip_address;
            reply.target_ethernet_address = msg.sender_ethernet_address;
            reply.sender_ip_address = _ip_address.ipv4_numeric();
            reply.sender_ethernet_address = _ethernet_address;
            
            e_frame.payload() = reply.serialize();
            e_frame.header().type = EthernetHeader::TYPE_ARP;
            e_frame.header().dst = header.src;
            e_frame.header().src = _ethernet_address;
            
            _frames_out.push(e_frame);
        } 
        if(isvalid_res){
            auto i_iter = unsent.begin();
            while (i_iter != unsent.end()){
                if (i_iter->second == msg.sender_ip_address){
                    EthernetFrame e_frame2;
                    e_frame2.payload() = i_iter->first.serialize();
                    e_frame2.header().type = EthernetHeader::TYPE_IPv4;
                    e_frame2.header().dst = msg.sender_ethernet_address;
                    e_frame2.header().src = _ethernet_address;  

                    _frames_out.push(e_frame2);

                    i_iter = unsent.erase(i_iter);
                }
                else i_iter++;
            }
            auto a_iter = arp_req.begin();
            while (a_iter != arp_req.end()){
                if (a_iter->arp_addr == msg.sender_ip_address) a_iter = arp_req.erase(a_iter);
                else a_iter++;
            }
        } 
    }

    return {};
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick){
     _time += ms_since_last_tick; 
}