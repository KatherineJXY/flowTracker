/*************************************************************************
 ***************  E G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/

parser SwitchEgressParser(
        packet_in pkt,
        out header_t hdr,
        out eg_metadata_t eg_md,
        out egress_intrinsic_metadata_t eg_intr_md) {
	
	TofinoEgressParser() tofino_parser;

	state start {
		tofino_parser.apply(pkt, eg_intr_md);
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
    	transition select(hdr.ipv4.protocol) {
			TYPE_REPORT: parse_report;
			default: reject;
		}
    }

	state parse_report {
		pkt.extract(hdr.report);
		transition accept;
	}

}

control SwitchEgress(
		inout header_t hdr,
        inout eg_metadata_t eg_md,
        in egress_intrinsic_metadata_t eg_intr_md,
        in egress_intrinsic_metadata_from_parser_t eg_intr_from_prsr,
        inout egress_intrinsic_metadata_for_deparser_t eg_intr_md_for_dprsr,
        inout egress_intrinsic_metadata_for_output_port_t eg_intr_md_for_oport) {
	
	apply {
		// processing time
		hdr.report.eg_tstamp = eg_intr_from_prsr.global_tstamp;
	}
}

control SwitchEgressDeparser(
		packet_out pkt, 
        inout header_t hdr, 
        in eg_metadata_t eg_md,
        in egress_intrinsic_metadata_for_deparser_t eg_intr_md_for_dprsr){

	apply {
		pkt.emit(hdr.ethernet);
		pkt.emit(hdr.vlan);
		pkt.emit(hdr.arp);
		pkt.emit(hdr.ipv4);
		pkt.emit(hdr.report);
	}
}

