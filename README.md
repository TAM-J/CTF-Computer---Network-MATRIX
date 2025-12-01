# 🤖 The Matrix CTF: Break Out of the Simulation

> "You take the blue pill... the story ends. You wake up in your bed and believe whatever you want to believe. You take the red pill... you stay in Wonderland, and I show you how deep the rabbit-hole goes."

Inspired by *The Matrix*, this Capture The Flag (CTF) challenge is a full-stack, multi-stage cybersecurity journey designed to test and teach a wide array of practical skills. Your goal is to follow in Neo's footsteps, break out of the digital confines, and uncover the **hidden Matrix Protocols**.

Are you ready to unplug? 💊

-----

## 🚀 The Journey: Stages of the Simulation

This CTF takes you through seven distinct stages, each requiring a different set of security analysis and exploitation skills.

### 1\. 🕳️ Entering the Rabbit Hole

  * **Initial Access:** The challenge begins at the provided URL, forcing the classic red pill or blue pill decision.
  * **Outcome:** Choosing the red pill initiates the simulation by downloading a **password-protected ZIP file**.

### 2\. 🔐 Cracking the ZIP

  * **Challenge:** The ZIP archive contains the first set of files but is locked.
  * **Skill:** **Password Cracking** and **Python Scripting** (using the hint file to generate a targeted dictionary).
  * **Solution:** `oraclesmith09`

### 3\. 🧵 Breaking the EXE

  * **Files:** `breaking.exe`, `the_source.pyc`, `hint.txt`.
  * **Challenge:** The `breaking.exe` program uses advanced **Anti-Debugging** techniques.
  * **Skill:** **Reverse Engineering (IDA/Ghidra)**, locating the password check, and **Binary Patching** (patching the conditional jump) to force the correct execution flow.
  * **Outcome:** Successful patching generates a new compiled Python file, `x.file`.

### 4\. 🐍 Analyzing the .pyc Client/Server

  * **Challenge:** Analyzing the included `the_source.pyc` file reveals its dual function: a small HTTP server and a **Client** sending out challenges.
  * **Skill:** **Python Bytecode Analysis** to understand the server's required 4-digit code and the client's network communication.
  * **Network Target:** The client sends **Hex-encoded UDP challenge packets** to `101.101.101.101:12345`.

### 5\. 📡 Spoofing UDP Responses

  * **Challenge:** The client requires specific, spoofed UDP responses from the *same IP* it contacted.
  * **Skill:** **Wireshark Traffic Inspection** (to sniff and decode the challenge packets) and **UDP Packet Spoofing (Scapy)**.
  * **Process:**
    1.  Sniff the outgoing UDP challenges.
    2.  Extract the Challenge ID.
    3.  Craft and send spoofed replies, pretending to be the remote server.
  * **Outcome:** Successful communication reveals the final piece of information: the HTTP server password is a 4-digit code.

### 6\. 🧨 Brute-Forcing the HTTP Server

  * **Challenge:** The client's HTTP server requires a 4-digit code for access.
  * **Skill:** **HTTP Client Automation** and **Brute-Forcing** (writing a script to POST all possible 4-digit combinations).
  * **Solution:** `1999` (The year *The Matrix* was released).
  * **Outcome:** Submitting the correct code returns the final link.

### 7\. 🧾 The Final Revelation

  * The final link takes the player to a page revealing the **Matrix Protocols**, completing the CTF and officially **breaking out of the simulation**.

-----

## 🛠️ Skills Tested & Learned

This CTF is a comprehensive training ground covering multiple domains of cybersecurity:

| Domain | Specific Skills |
| :--- | :--- |
| **Reverse Engineering** | Static analysis (IDA), Binary Patching, Anti-Debugging Bypass |
| **Network Security** | Wireshark & Packet Analysis, UDP Packet Crafting & Spoofing (Scapy) |
| **Scripting & Automation**| Python scripting, ZIP Password Cracking, HTTP Client Automation (POST requests) |
| **Programming** | Recognizing and Analyzing compiled **Python Bytecode (.pyc)** |
| **Methodology** | Logical reasoning, Information Gathering, and CTF-style chaining of exploits |

-----

## 📂 Repository Structure

The `matrix-ctf` repository contains all the challenge files and the write-up for players who have completed the challenge:

```
/matrix-ctf
│
├── Crack_rabbit_hole.py   # Python script used for ZIP password cracking
├── NetworkAnswer.py       # Scapy script for UDP sniffing and spoofing
├── http_cracker.py        # Script for brute-forcing the HTTP server
├── breaking.exe           # The anti-debugging challenge file
├── the_source.pyc         # The client/server Python bytecode file
├── x.file                 # The file generated after patching breaking.exe
├── writeup.pdf            # A detailed explanation of all steps and solutions
└── README.md              # You are here!
```

-----

## 🔑 Take the Red Pill

Ready to see how deep the rabbit-hole goes?

1.  **Clone the repository:**
    ```bash
    git clone [YOUR-REPO-URL]
    ```
2.  **Start at the beginning:**
    👉 `http://matrixctf.unaux.com`
3.  **Dive into the files\!**

Good luck, operator.
