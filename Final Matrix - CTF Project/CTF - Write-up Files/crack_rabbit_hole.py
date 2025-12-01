import math
import zipfile
import itertools
import string
import os
import pyzipper
import random

def guess_password(word_list, num_words=2, num_digits=2):
    """
    Attempt to guess the password using combinations of words and digits.
    """
    word_combinations = list(itertools.combinations(word_list, num_words))
    digit_combinations = list(itertools.product(string.digits, repeat=num_digits))

    total_combinations = len(word_combinations) * len(digit_combinations) * math.factorial(num_words + num_digits)
    attempts = 0

    for words in word_combinations:
        for digits in digit_combinations:
            parts = list(words) + list(digits)
            for permutation in itertools.permutations(parts):
                attempts += 1
                guess = ''.join(permutation)
                
                yield guess  # Use a generator to yield each guess
                
                if attempts % 1000 == 0:
                    progress = (attempts / total_combinations) * 100
                    print(f"\rProgress: {progress:.2f}% ({attempts}/{total_combinations})", end="", flush=True)

def crack_zip(zip_filename, wordlist):
    #with open(wordlist, 'r') as f:
    word_list = wordlist
    
    print(f"Wordlist contents: {word_list}")
    
    for password in guess_password(word_list):
        try:
            with pyzipper.AESZipFile(zip_filename) as zf:
                zf.extractall(pwd=password.encode())
            print(f"\nSuccessfully extracted with password: {password}")
            return password
        except RuntimeError as e:
            if "Bad password" in str(e):
                pass  # Don't print for every incorrect password
            else:
                print(f"\nRuntime error for password {password}: {str(e)}")
        except Exception as e:
            print(f"\nUnexpected error for password {password}: {str(e)}")
    
    return None

def main():
    zip_filename = "The_Rabbit_hole.zip"
    wordlist = ['neo', 'morpheus', 'trinity', 'oracle', 'zion', 'nebuchadnezzar', 'sentinel', 'agent', 'smith', 'architect']

    print("Welcome, Neo. It's time to breach the entrance of the Matrix.")
    print("Initiating dictionary attack on the zip file...")

    try:
        password = crack_zip(zip_filename, wordlist)

        if password:
            print(f"Success! The password is: {password}")
            if os.path.exists('network_info.txt'):
                print("Successfully extracted network_info.txt")
                with open('network_info.txt', 'r') as f:
                    print("Content of network_info.txt:")
                    print(f.read())
            else:
                print("Error: network_info.txt not found after extraction.")
        else:
            print("Failed to crack the password. The Matrix's defenses are strong.")
    
    except FileNotFoundError:
        print(f"Error: The file '{zip_filename}' or '{wordlist}' was not found.")
    except Exception as e:
        print(f"An unexpected error occurred: {str(e)}")

if __name__ == "__main__":
    main()