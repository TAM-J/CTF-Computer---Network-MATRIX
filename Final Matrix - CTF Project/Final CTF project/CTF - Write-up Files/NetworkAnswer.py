from scapy.all import IP, UDP, send, sniff
import hashlib
import re

SERVER_IP = "101.101.101.101"
#CLIENT_IP = "127.0.0.1"
SERVER_PORT = 12345
CLIENT_PORT = 54321

def send_udp_message(message, dst_ip, dst_port):
    packet = IP(src=SERVER_IP, dst=dst_ip) / UDP(sport=SERVER_PORT, dport=dst_port) / message
    #packet = IP(src='1.1.1.1', dst=dst_ip) / UDP(sport=SERVER_PORT, dport=dst_port) / message
    send(packet, verbose=False)
    print(f"Attempted to send to {dst_ip}:{dst_port}: {message}")

def solve_math_question(question):
    match = re.search(r"What's (\d+) ([+\-*]) (\d+)\?", question)
    if match:
        a, op, b = match.groups()
        print (str(eval(f"{a} {op} {b}")))
        return str(eval(f"{a} {op} {b}"))

    return None

def handle_packet(packet):
    print("in handle")
    if packet.haslayer(UDP)  and packet[UDP].sport == CLIENT_PORT: 
        print("after if")
        message = packet[UDP].payload.load.decode()
        print(f"Received message from {packet[IP].src}: {message}")

        if "architect of the Matrix" in message:
            response = "The Architect"
        elif "last human city" in message:
            response = "Zion"
        elif "repeat this number" in message:
            number = message.split(":")[-1].strip().split('.')[0]
            response = number
        elif "MD5 hash of 'redpill'" in message:
            response = hashlib.md5("redpill".encode()).hexdigest()
        elif "What's" in message and any(op in message for op in ['+', '-', '*']):
            print("in math challenge")
            response= solve_math_question(message)
        else:
            response = "Unknown question"

        send_udp_message(response, packet[IP].src, packet[UDP].sport)

def main():
    print(f"UDP server listening on {SERVER_IP}:{SERVER_PORT}")
    sniff(filter=f"udp and dst host {SERVER_IP} and dst port {SERVER_PORT}", prn=handle_packet)

if __name__ == "__main__":
    main()
