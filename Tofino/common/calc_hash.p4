#include "headers.p4"

control calc_ipv4_hash(
        in header_t hdr,
        out bit<16> index)(bit<32> coeff)
{
    CRCPolynomial<bit<32>>(
        coeff = coeff,
        reversed = true,
        msb = false,
        extended = false,
        init = 0xFFFFFFFF,
        xor = 0xFFFFFFFF) poly;
    Hash<bit<16>>(HashAlgorithm_t.CUSTOM, poly) hash_algo;

    action do_hash() {
        index = hash_algo.get({
            hdr.ipv4.src_addr,
            hdr.ipv4.dst_addr,
            hdr.ipv4.protocol
        });
    }

    apply{
        do_hash();
    }
}

control calc_ipv4_hashes(
        in header_t hdr,
        inout rc_index_t rc_index,
        inout ff_index_t ff_index)
{
    calc_ipv4_hash(coeff=32w0x00390FC3) hash0;
    calc_ipv4_hash(coeff=32w0x04C11DB7) hash1;
    calc_ipv4_hash(coeff=32w0x34FD110C) hash2;

    apply {
        hash0.apply(hdr, rc_index);             // the orignial index of the queuing recorder
        hash1.apply(hdr, ff_index[15:0]);       // the first index within the flow filter
        hash2.apply(hdr, ff_index[31:16]);      // the second
    }
}    

