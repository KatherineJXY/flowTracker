/*******************************************************************************
 * BAREFOOT NETWORKS CONFIDENTIAL & PROPRIETARY
 *
 * Copyright (c) 2019-present Barefoot Networks, Inc.
 *
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property of
 * Barefoot Networks, Inc. and its suppliers, if any. The intellectual and
 * technical concepts contained herein are proprietary to Barefoot Networks, Inc.
 * and its suppliers and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.  Dissemination of
 * this information or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained from Barefoot Networks, Inc.
 *
 * No warranty, explicit or implicit is provided, unless granted under a written
 * agreement with Barefoot Networks, Inc.
 *
 ******************************************************************************/

#ifndef _HEADERS_
#define _HEADERS_

#define FLOW_FILTER_ENTRIES 4096
#define FLOW_FILTER_BIT_WIDTH 16
#define RECORDER_ENTRIES 8192
// #define PROC_TIME_QUERY_TABLE_SIZE 65534
#define RECORDER_BIT_WIDTH 32
#define FLOW_COUNT_BIT_WIDTH 16

typedef bit<48> mac_addr_t;
typedef bit<32> ipv4_addr_t;

typedef bit<16> ether_type_t;
const ether_type_t ETHERTYPE_IPV4 = 16w0x0800;
const ether_type_t ETHERTYPE_VLAN = 16w0x8100;
const ether_type_t ETHERTYPE_ARP = 16w0x0806;

typedef bit<8> ip_protocol_t;
const ip_protocol_t IP_PROTOCOLS_TCP = 6;
const ip_protocol_t IP_PROTOCOLS_UDP = 17;

const bit<8> TYPE_REPORT = 0x17;

typedef bit<32> ff_index_t;
typedef bit<16> rc_index_t;
typedef bit<16> reg_index_t;
typedef bit<FLOW_FILTER_BIT_WIDTH> ff_data_t;
typedef bit<RECORDER_BIT_WIDTH> rc_data_t;
typedef bit<FLOW_COUNT_BIT_WIDTH> count_t;

header ethernet_h {
    mac_addr_t dst_addr;
    mac_addr_t src_addr;
    bit<16> ether_type;
}

header vlan_h {
    bit<3> pri;
    bit<1> dei;
    bit<12> vlan_id;
    bit<16> etherType;
}

header arp_h {
    bit<16> hwType;
    bit<16> protoType;
    bit<8> hwAddrLen;
    bit<8> protoAddrLen;
    bit<16> opcode;
    bit<48> src_mac;
    bit<32> srcAddr;
    bit<48> dst_mac;
    bit<32> dstAddr;
}

header ipv4_h {
    bit<4> version;
    bit<4> ihl;
    bit<8> diffserv;
    bit<16> total_len;
    bit<16> identification;
    bit<3> flags;
    bit<13> frag_offset;
    bit<8> ttl;
    bit<8> protocol;
    bit<16> hdr_checksum;
    ipv4_addr_t src_addr;
    ipv4_addr_t dst_addr;
}

header tcp_h {
    bit<16> src_port;
    bit<16> dst_port;
    bit<32> seq_no;
    bit<32> ack_no;
    bit<4> data_offset;
    bit<4> res;
    bit<8> flags;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgent_ptr;
}

header udp_h {
    bit<16> src_port;
    bit<16> dst_port;
    bit<16> hdr_length;
    bit<16> checksum;
}



header report_h {
    bit<8> protocol;
    bit<48> ig_tstamp;
    bit<48> eg_tstamp;
}

struct header_t {
    ethernet_h ethernet;
    vlan_h vlan;
    arp_h arp;
    ipv4_h ipv4;
    report_h report;

    // Add more headers here.
}

header resubmit_h {
    bit<16> fcnt;
    bit<48> ingress_time;
}

struct ig_metadata_t {
    resubmit_h resubmit_data;
	
    // rc_data_t record;
    bit<16> ff_val_1;
    bit<16> ff_val_2;
}

struct eg_metadata_t {}


#endif /* _HEADERS_ */

