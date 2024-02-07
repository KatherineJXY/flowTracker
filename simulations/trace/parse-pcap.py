import scapy.all as scapy
import numpy as np
import os

def extract_five_tuple(packet):
    if scapy.IP in packet:
        src_ip = packet[scapy.IP].src
        dst_ip = packet[scapy.IP].dst
        src_port = None
        dst_port = None
        protocol = packet[scapy.IP].proto
        # 判断是否是 TCP 或 UDP 协议
        if scapy.TCP in packet:
            src_port = packet[scapy.TCP].sport
            dst_port = packet[scapy.TCP].dport
        elif scapy.UDP in packet:
            src_port = packet[scapy.UDP].sport
            dst_port = packet[scapy.UDP].dport
        return src_ip, dst_ip, src_port, dst_port, protocol
    else:
        return None

def generate_poisson_value():
    # Adjust the parameters as needed for your specific use cases
    # Here, lam (lambda) is the rate parameter of the Poisson distribution
    lam = 500 
    return np.random.poisson(lam)


def parse_pcap(file_path, output_file):
    with open(output_file, 'a+') as output:
        packets = scapy.rdpcap(file_path)
        for packet in packets:
            five_tuple = extract_five_tuple(packet)
            if five_tuple:
                poisson_value = generate_poisson_value()
                output_line = f"{five_tuple[0]},{five_tuple[1]},{five_tuple[2]},{five_tuple[3]},{five_tuple[4]} {poisson_value}\n"
                output.write(output_line)

if __name__ == '__main__':
    category_path = r'E:\jxy\campus'
    output_file = r'E:\jxy\campus\campus.csv'
    for filename in os.listdir(category_path):
        if filename.endswith("pcap"):
            file_path = os.path.join(category_path, filename)
            parse_pcap(file_path, output_file)