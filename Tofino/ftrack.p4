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

#include <core.p4>
#if __TARGET_TOFINO__ == 2
#include <t2na.p4>
#else
#include <tna.p4>
#endif

#include "common/calc_hash.p4"
#include "common/util.p4"
#include "common/API_common.p4"
#include "egress.p4"

// ---------------------------------------------------------------------------
// Ingress parser
// ---------------------------------------------------------------------------
parser SwitchIngressParser(
		packet_in pkt,
		out header_t hdr,
		out ig_metadata_t ig_md,
		out ingress_intrinsic_metadata_t ig_intr_md) {

    state start {
		ig_md.ff_val_1 = 0;
		ig_md.ff_val_2 = 0;
    	pkt.extract(ig_intr_md);
    	transition select(ig_intr_md.resubmit_flag) {
    	    1 : parse_resubmit;
    	    0 : parse_port_metadata;
    	}
    }
    
    state parse_resubmit {
    	pkt.extract(ig_md.resubmit_data);
    	transition parse_ethernet;
    }
    
    state parse_port_metadata {
    	pkt.advance(PORT_METADATA_SIZE);
    	transition parse_ethernet;
    }
    
    state parse_ethernet {
    	pkt.extract(hdr.ethernet);
    	transition select(hdr.ethernet.ether_type) {
    	    ETHERTYPE_IPV4 : parse_ipv4;
			ETHERTYPE_VLAN: parse_vlan;
    	    default : reject;
    	}
    }

	state parse_vlan {
        pkt.extract(hdr.vlan);
        transition select(hdr.vlan.etherType) {
            ETHERTYPE_IPV4: parse_ipv4;
			ETHERTYPE_ARP: parse_arp;
            default: reject;
        }
    }

	state parse_arp {
		pkt.extract(hdr.arp);
		transition select(hdr.arp.protoType) {
			ETHERTYPE_IPV4: parse_ipv4;
			default: reject;
		}
	}
    
    state parse_ipv4 {
    	pkt.extract(hdr.ipv4);
    	transition accept;
    }
}

// ---------------------------------------------------------------------------
// Ingress Deparser
// ---------------------------------------------------------------------------
control SwitchIngressDeparser(
	packet_out pkt,
	inout header_t hdr,
	in ig_metadata_t ig_md,
	in ingress_intrinsic_metadata_for_deparser_t ig_intr_dprsr_md) {
    
    Resubmit() resubmit;
    
    apply {
    	if (ig_intr_dprsr_md.resubmit_type == 1) {
    	    resubmit.emit(ig_md.resubmit_data);
    	}
    	pkt.emit(hdr.ethernet);
		pkt.emit(hdr.vlan);
		pkt.emit(hdr.arp);
		pkt.emit(hdr.ipv4);
		pkt.emit(hdr.report);
    }
}

control SwitchIngress(
	inout header_t hdr,
	inout ig_metadata_t ig_md,
	in ingress_intrinsic_metadata_t ig_intr_md,
	in ingress_intrinsic_metadata_from_parser_t ig_prsr_md,
	inout ingress_intrinsic_metadata_for_deparser_t ig_dprsr_md,
	inout ingress_intrinsic_metadata_for_tm_t ig_tm_md) {
    
    // Forwarding
    action drop() {
    	ig_dprsr_md.drop_ctl = 1;
    }
    
    action l3_switch(PortId_t port) {
	    ig_tm_md.ucast_egress_port = port;
        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
    }
    
    table forward{
        key = { ig_intr_md.ingress_port: exact; }
        actions = {
            drop; l3_switch;
            @defaultonly NoAction;
        }
        const default_action = NoAction();
        size = 250;
    }
    
    action resubmit_ctl(bit<16> fcnt) {
    	 ig_dprsr_md.resubmit_type = 1;
    	 ig_md.resubmit_data.fcnt = fcnt;
    	 ig_md.resubmit_data.ingress_time = hdr.report.ig_tstamp;
    }

	action init_telem_rpt() {
		hdr.report.setValid();
		hdr.report.protocol = hdr.ipv4.protocol;
		hdr.report.ig_tstamp = ig_prsr_md.global_tstamp;
		hdr.report.eg_tstamp = 0;
		hdr.ipv4.protocol = TYPE_REPORT;
	}
    
    // action drop_exit_ingress() {
    	// ig_dprsr_md.drop_ctl = 0x1;
    	// exit;
    //}

	Lpf<rc_data_t, rc_index_t>(RECORDER_ENTRIES) rc_lpf;
    
    FlowFilterUpdate() ff_update;
    FlowCountUpdate() fc_update;
	StepWidth() cal_step_width;
    RecorderIndex() index_update;
    
    apply{
    	if (hdr.ipv4.isValid()) {

			init_telem_rpt();
			ff_index_t ff_index = 0;
			rc_index_t rc_index = 0;
			rc_index_t sub_index;
    	    calc_ipv4_hashes.apply(hdr, rc_index, ff_index);

    	    count_t fcnt = 0;
			rc_index_t distance = 0;
			count_t place = 1;
    	    rc_data_t qdata = ig_prsr_md.global_tstamp[31:0];
	    if (ig_intr_md.resubmit_flag == 0) {	// no resubmitted packet 
    	    	ff_update.apply(ff_index, 0, place, ig_md.ff_val_1, ig_md.ff_val_2);
    	    	
    	    	if (ig_md.ff_val_1 == 0 || ig_md.ff_val_2 == 0) {	// new flow
    	    	    fc_update.apply(rc_index, true, fcnt);
    	    	    resubmit_ctl(fcnt);
					place = 0;
    	    	} else {
    	    	    fcnt = max<count_t>(ig_md.ff_val_1, ig_md.ff_val_2);
					cal_step_width.apply(fcnt, distance);
					sub_index = rc_index + distance;
					index_update.apply(sub_index, rc_index);
    	    	}	
    	        
    	    } else { // resubmitted packet
    	    	fcnt = ig_md.resubmit_data.fcnt;
				hdr.report.ig_tstamp = ig_md.resubmit_data.ingress_time;

				cal_step_width.apply(fcnt, distance);
				if (distance != 0) {
					sub_index = rc_index + distance;
					index_update.apply(sub_index, rc_index);
					fc_update.apply(rc_index, false, place);
				}
				
				ff_update.apply(ff_index, fcnt, place, ig_md.ff_val_1, ig_md.ff_val_2);

    	    }
	    
	    if (place != 0) {
				rc_lpf.execute(qdata, rc_index);
			} 
			
    	    forward.apply();
    	}  	
    }
}

Pipeline(SwitchIngressParser(),
    SwitchIngress(),
    SwitchIngressDeparser(),
    SwitchEgressParser(),
    SwitchEgress(),
    SwitchEgressDeparser()) pipe;

Switch(pipe) main;
