import base64



def encrypt_python_code(code):
    # Convert the code to bytes
    code_bytes = code.encode('utf-8')
    
    # Encode the bytes using base64
    encoded_bytes = base64.b64encode(code_bytes)
    
    # Convert the encoded bytes back to a string
    encrypted_code = encoded_bytes.decode('utf-8')
    
    return encrypted_code

# Example usage
if __name__ == "__main__":
    original_code = '''
import socket
import random
import time
import sys
import hashlib

server_ip = "101.101.101.101"#'127.0.0.1'  # Use localhost for testing
server_port = 12345
expected_response_ip = "101.101.101.101"

def send_udp_message(sock, message, address):
    try:
        sock.sendto(message.encode(), address)
        #print(f"Sent to {address}: {message}")
    except socket.error as e:
        print(f"Error sending message: {e}")
        sys.exit(1)

def receive_udp_message(sock, buffer_size=1024, timeout=5):
    sock.settimeout(timeout)
    try:
        data, addr = sock.recvfrom(buffer_size)
        return data.decode(), addr[0]
    except socket.timeout:
        print(f"Receive timeout after {timeout} seconds. No response received.")
        return None, None
    except socket.error as e:
        print(f"Error receiving message: {e}")
        return None, None

def generate_random_math_question():
    operations = ['+', '-', '*']
    a = random.randint(10, 50)
    b = random.randint(10, 50)
    operation = random.choice(operations)
    question = f"What's {a} {operation} {b}?"
    answer = str(eval(f"{a} {operation} {b}"))
    return question, answer

def generate_random_number():
    number = random.randint(1000, 9999)
    return f"Please repeat this number: {number}.", str(number)


def main():
 #server_ip  # For local testing, expect responses from the same IP

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        client_ip = '0.0.0.0'  # Bind to all available interfaces
        client_port = 54321  # Use a random high port number
        sock.bind((client_ip, client_port))
        print(f"Client bound to {client_ip}:{client_port}")
    except socket.error as e:
        print(f"Error creating socket: {e}")
        sys.exit(1)

    challenges = [
        ("Who is the architect of the Matrix?", "The Architect"),
        ("What is the last human city?", "Zion"),
        generate_random_number,
        ("What is the MD5 hash of 'redpill'?", hashlib.md5("redpill".encode()).hexdigest()),
        generate_random_math_question
    ]

    correct_responses = 0

    try:
        for i, challenge in enumerate(challenges, 1):
            if callable(challenge):
                question, expected = challenge()
            else:
                question, expected = challenge

            send_udp_message(sock, f"Challenge {i}: {question}", (server_ip, server_port))
            response, resp_ip = receive_udp_message(sock)
            if resp_ip != expected_response_ip and resp_ip != None:
                print(f"Warning: Response received from unexpected IP: {resp_ip}")
                break
            if response is None:
                print(f"Challenge {i} failed: No response received.")
                continue  # Try the next challenge instead of breaking
            
            print(f"Received from {resp_ip}: {response}")



            if response.strip().lower() == expected.lower():
                correct_responses += 1
                print(f"Challenge {i} correct!")
            else:
                print(f"Challenge {i} incorrect. Expected: {expected}, Got: {response}")
            
            time.sleep(1)  # Add a small delay between requests

        print(f"Challenges completed. {correct_responses} out of {len(challenges)} were correct.")
        if correct_responses == len(challenges):
            print("MORPHEUS LIKES CODES WITH NUMBERS. ESPECIALLY CODES AS LONG AS THE NAME OF THE LAST HUMEN CITY!")

    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        import traceback
        traceback.print_exc()

    finally:
        sock.close()

if __name__ == "__main__":
    main()
'''
    encrypted = encrypt_python_code(original_code)
    print("Encrypted code:")
    print(encrypted)