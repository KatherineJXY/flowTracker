import pyshark
import numpy as np

class netFlow(object):
    def __init__(self, file_path):
        self.cap = pyshark.FileCapture(file_path, tshark_path=r"E:\Program Files\Wireshark\tshark.exe")
     
    # {ip_src, ip_dst, l4_src_port, l4_dst_port, protocol}
    def get_target_client(self):
        try:
            for pkt in self.cap:
                ip_src = pkt.ip.ip_src
                ip_dst = pkt.ip.ip_dst
                ip_proto = pkt.ip.proto

                l4_src_port = None
                l4_dst_port = None

                # Check transport layer protocol
                if 'TCP' in pkt:
                    l4_src_port = pkt.tcp.srcport
                    l4_dst_port = pkt.tcp.dstport
                elif 'UDP' in pkt:
                    l4_src_port = pkt.udp.srcport
                    l4_dst_port = pkt.udp.dstport
                yield [ip_src+ "," + ip_dst + "," + l4_src_port + "," + l4_dst_port + "," + ip_proto]

        except AttributeError as e:
            pass

if __name__ == '__main__':
    try:
        pcap_file = r"E:\code\Simulation_Experiment\traces\ISP_network\oc48-mfn.dirA.20020814-160000.UTC.anon.pcap\test_00000_20020815000000.pcap"
        
        net_flow = netFlow(pcap_file)
        target_clients = net_flow.get_target_client()
        # flows = []
        file = open(r".\pcap\oc48-mfn.csv", "a+")
        time_interv = np.random.exponential(1/5, 10000000)

        i = 0
        for target_client in target_clients:
            flow = ",".join(target_client)
            file.write(flow + " " + str(int(time_interv[i]*1000000)) + "\n")
            i += 1
        file.close()

    except Exception as e:
        print(e)