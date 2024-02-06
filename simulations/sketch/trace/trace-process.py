from scapy.all import *
import numpy as np

def extract_five_tuple(packet):
    if IP in packet and TCP in packet:
        ipv4_src = packet[IP].src
        ipv4_dst = packet[IP].dst
        l4_src_port = packet[TCP].sport
        l4_dst_port = packet[TCP].dport
        protocol = packet[IP].proto
        return ipv4_src, ipv4_dst, l4_src_port, l4_dst_port
    elif IP in packet and UDP in packet:
        ipv4_src = packet[IP].src
        ipv4_dst = packet[IP].dst
        l4_src_port = packet[UDP].sport
        l4_dst_port = packet[UDP].dport
        protocol = packet[IP].proto

        return ipv4_src, ipv4_dst, l4_src_port, l4_dst_port, protocol
    else:
        # Handle other protocols or edge cases as needed
        return None

def generate_poisson_value():
    # Adjust the parameters as needed for your specific use case
    # Here, lam (lambda) is the rate parameter of the Poisson distribution
    lam = 500  # You can adjust this value based on your requirements
    return np.random.poisson(lam)

def parse_pcap(file_path, output_file):
    with open(output_file, 'a+') as output:
        packets = rdpcap(file_path)
        for packet in packets:
            five_tuple = extract_five_tuple(packet)
            if five_tuple:
                poisson_value = generate_poisson_value()
                output_line = f"{five_tuple[0]},{five_tuple[1]},{five_tuple[2]},{five_tuple[3]},{five_tuple[4]} {poisson_value}\n"
                output.write(output_line)

# Replace 'your_pcap_file.pcap' with the actual path to your PCAP file
# Replace 'output_file.csv' with the desired output file path
parse_pcap(r"E:\code\Simulation_Experiment\traces\ISP_network\202312011400.pcap\truncate\mawi_00000_20231201130000.pcap", r".\pcap\mawi.csv")
