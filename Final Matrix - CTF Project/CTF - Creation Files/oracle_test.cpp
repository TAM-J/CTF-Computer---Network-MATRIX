#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <intrin.h>
#else
#include <unistd.h>
#include <sys/ptrace.h>
#endif

const std::string CORRECT_PASSWORD = "KNOWTHYSELF303";

// Anti-debugging functions
#ifdef _WIN32
bool isDebuggerPresent() {
    return IsDebuggerPresent();
}

bool checkHardwareBreakpoints() {
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    HANDLE hThread = GetCurrentThread();

    if (GetThreadContext(hThread, &ctx)) {
        if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3) {
            return true;
        }
    }
    return false;
}


std::string wcharToString(const WCHAR* wideString) {
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
    std::string result(bufferSize - 1, 0);  // -1 because WideCharToMultiByte includes the null terminator
    WideCharToMultiByte(CP_UTF8, 0, wideString, -1, &result[0], bufferSize, nullptr, nullptr);
    return result;
}

bool checkRunningProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) {
        do {
#ifdef UNICODE
            std::string processName = wcharToString(pe.szExeFile); // Convert wide string to std::string
#else
            std::string processName = pe.szExeFile; // ANSI string, no conversion needed
#endif
            if (processName == "ollydbg.exe" || processName == "x32dbg.exe" || processName == "x64dbg.exe") {
                return true;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return false;
}
#else
void antiDebugUnix() {
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
        std::cout << "Debugger detected! Exiting." << std::endl;
        exit(1);
    }
}
#endif

std::string base64_decode(const std::string& encoded_string) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    std::vector<unsigned char> ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_4[4], char_array_3[3];
    for (char c : encoded_string) {
        if (c == '=') break;
        char_array_4[i++] = c;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; i < 3; i++)
                ret.push_back(char_array_3[i]);
            i = 0;
        }
    }
    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;
        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; j < i - 1; j++)
            ret.push_back(char_array_3[j]);
    }
    return std::string(ret.begin(), ret.end());
}

std::string decrypt_python_code(const std::string& encrypted_code) {
    std::string decoded = base64_decode(encrypted_code);
    // Convert decoded bytes to a proper string representation
    std::string result;
    for (unsigned char c : decoded) {
        if (c >= 32 && c <= 126) {
            result += c;
        }
        else if (c == '\n' || c == '\r' || c == '\t') {
            result += c;
        }
        else {
            // For any other non-printable character, add its escaped representation
            char buf[5];
            snprintf(buf, sizeof(buf), "\\x%02x", c);
            result += buf;
        }
    }
    return result;
}

bool deleteDirectory(const std::string& path) {
#ifdef _WIN32
    std::string command = "rmdir /s /q \"" + path + "\"";
#else
    std::string command = "rm -rf \"" + path + "\"";
#endif
    int result = system(command.c_str());
    return (result == 0);
}

bool deletePycacheFolder() {
    std::string pycacheDir = "__pycache__";

#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(pycacheDir.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
#else
    struct stat st;
    if (stat(pycacheDir.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
#endif
        std::cout << "Deleting __pycache__ folder..." << std::endl;
        if (deleteDirectory(pycacheDir)) {
            std::cout << "__pycache__ folder deleted successfully." << std::endl;
            return true;
        }
        else {
            std::cerr << "Failed to delete __pycache__ folder." << std::endl;
            return false;
        }
    }
    else {
        std::cout << "__pycache__ folder not found." << std::endl;
        return true; // Not an error if the folder doesn't exist
    }
    }
std::string executeCommand(const std::string & command) {
    std::string result;
    char buffer[128];
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) {
        return "Error: Command execution failed.";
    }
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
    }
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return result;
}

bool isPythonInstalled() {
#ifdef _WIN32
    return (system("where python > nul 2>&1") == 0);
#else
    return (system("which python > /dev/null 2>&1") == 0);
#endif
}

bool compilePythonScript(const std::string & pyFilename) {
    if (!isPythonInstalled()) {
        std::cerr << "Error: Python is not installed or not in the system PATH." << std::endl;
        return false;
    }

    std::string compileCommand = "python -c \"import py_compile; py_compile.compile('" + pyFilename + "', '" + pyFilename + "c')\"";
    std::string compileResult = executeCommand(compileCommand);
    std::cout << "Compile command result: " << compileResult << std::endl;

    // Check if the .pyc file was created
    std::ifstream pycFile(pyFilename + "c");
    if (!pycFile.good()) {
        std::cerr << "Error: Failed to create .pyc file." << std::endl;
        return false;
    }
    pycFile.close();

    return true;
}
bool saveDecryptedScript(const std::string & decryptedScript) {
    std::string pyFilename = "temp_script.py";
    std::ofstream pythonFile(pyFilename);
  //  if (!pythonFile) {
    //    std::cerr << "Error: Failed to create the Python script file." << std::endl;
      //  return false;
    //}
    pythonFile << decryptedScript;
    pythonFile.close();
    //std::cout << "Temporary Python file created." << std::endl;

    // Print the content of the decrypted script for debugging
    //std::cout << "Decrypted script content:" << std::endl;
    //std::cout << decryptedScript << std::endl;

    if (!compilePythonScript(pyFilename)) {
        std::cerr << "Error: Failed to compile Python script." << std::endl;
        return false;
    }

    std::string renameCommand;
#ifdef _WIN32
    renameCommand = "move /Y " + pyFilename + "c oracle_test.pyc";
#else
    renameCommand = "mv -f " + pyFilename + "c oracle_test.pyc";
#endif
    std::string renameResult = executeCommand(renameCommand);
   // std::cout << "Rename command result: " << renameResult << std::endl;

    if (remove(pyFilename.c_str()) != 0) {
        std::cerr << "Warning: Failed to delete temporary Python file." << std::endl;
    }
    //else {
      //  std::cout << "Temporary Python file deleted." << std::endl;
    //}

    return true;
}
std::string clean_decoded_text(const std::string & decoded) {
    std::string cleaned;
    for (char c : decoded) {
        if ((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t') {
            cleaned += c;
        }
    }
    // Remove any non-ASCII characters
    cleaned = std::regex_replace(cleaned, std::regex("[^\x00-\x7F]+"), "");
    return cleaned;
}

// Main function
int main() {
    std::string encrypted_code = "CmltcG9ydCBzb2NrZXQKaW1wb3J0IHJhbmRvbQppbXBvcnQgdGltZQppbXBvcnQgc3lzCmltcG9ydCBoYXNobGliCgpzZXJ2ZXJfaXAgPSAiMTAxLjEwMS4xMDEuMTAxIiMnMTI3LjAuMC4xJyAgIyBVc2UgbG9jYWxob3N0IGZvciB0ZXN0aW5nCnNlcnZlcl9wb3J0ID0gMTIzNDUKZXhwZWN0ZWRfcmVzcG9uc2VfaXAgPSAiMTAxLjEwMS4xMDEuMTAxIgoKZGVmIHNlbmRfdWRwX21lc3NhZ2Uoc29jaywgbWVzc2FnZSwgYWRkcmVzcyk6CiAgICB0cnk6CiAgICAgICAgc29jay5zZW5kdG8obWVzc2FnZS5lbmNvZGUoKSwgYWRkcmVzcykKICAgICAgICAjcHJpbnQoZiJTZW50IHRvIHthZGRyZXNzfToge21lc3NhZ2V9IikKICAgIGV4Y2VwdCBzb2NrZXQuZXJyb3IgYXMgZToKICAgICAgICBwcmludChmIkVycm9yIHNlbmRpbmcgbWVzc2FnZToge2V9IikKICAgICAgICBzeXMuZXhpdCgxKQoKZGVmIHJlY2VpdmVfdWRwX21lc3NhZ2Uoc29jaywgYnVmZmVyX3NpemU9MTAyNCwgdGltZW91dD01KToKICAgIHNvY2suc2V0dGltZW91dCh0aW1lb3V0KQogICAgdHJ5OgogICAgICAgIGRhdGEsIGFkZHIgPSBzb2NrLnJlY3Zmcm9tKGJ1ZmZlcl9zaXplKQogICAgICAgIHJldHVybiBkYXRhLmRlY29kZSgpLCBhZGRyWzBdCiAgICBleGNlcHQgc29ja2V0LnRpbWVvdXQ6CiAgICAgICAgcHJpbnQoZiJSZWNlaXZlIHRpbWVvdXQgYWZ0ZXIge3RpbWVvdXR9IHNlY29uZHMuIE5vIHJlc3BvbnNlIHJlY2VpdmVkLiIpCiAgICAgICAgcmV0dXJuIE5vbmUsIE5vbmUKICAgIGV4Y2VwdCBzb2NrZXQuZXJyb3IgYXMgZToKICAgICAgICBwcmludChmIkVycm9yIHJlY2VpdmluZyBtZXNzYWdlOiB7ZX0iKQogICAgICAgIHJldHVybiBOb25lLCBOb25lCgpkZWYgZ2VuZXJhdGVfcmFuZG9tX21hdGhfcXVlc3Rpb24oKToKICAgIG9wZXJhdGlvbnMgPSBbJysnLCAnLScsICcqJ10KICAgIGEgPSByYW5kb20ucmFuZGludCgxMCwgNTApCiAgICBiID0gcmFuZG9tLnJhbmRpbnQoMTAsIDUwKQogICAgb3BlcmF0aW9uID0gcmFuZG9tLmNob2ljZShvcGVyYXRpb25zKQogICAgcXVlc3Rpb24gPSBmIldoYXQncyB7YX0ge29wZXJhdGlvbn0ge2J9PyIKICAgIGFuc3dlciA9IHN0cihldmFsKGYie2F9IHtvcGVyYXRpb259IHtifSIpKQogICAgcmV0dXJuIHF1ZXN0aW9uLCBhbnN3ZXIKCmRlZiBnZW5lcmF0ZV9yYW5kb21fbnVtYmVyKCk6CiAgICBudW1iZXIgPSByYW5kb20ucmFuZGludCgxMDAwLCA5OTk5KQogICAgcmV0dXJuIGYiUGxlYXNlIHJlcGVhdCB0aGlzIG51bWJlcjoge251bWJlcn0uIiwgc3RyKG51bWJlcikKCgpkZWYgbWFpbigpOgogI3NlcnZlcl9pcCAgIyBGb3IgbG9jYWwgdGVzdGluZywgZXhwZWN0IHJlc3BvbnNlcyBmcm9tIHRoZSBzYW1lIElQCgogICAgdHJ5OgogICAgICAgIHNvY2sgPSBzb2NrZXQuc29ja2V0KHNvY2tldC5BRl9JTkVULCBzb2NrZXQuU09DS19ER1JBTSkKICAgICAgICBjbGllbnRfaXAgPSAnMC4wLjAuMCcgICMgQmluZCB0byBhbGwgYXZhaWxhYmxlIGludGVyZmFjZXMKICAgICAgICBjbGllbnRfcG9ydCA9IDU0MzIxICAjIFVzZSBhIHJhbmRvbSBoaWdoIHBvcnQgbnVtYmVyCiAgICAgICAgc29jay5iaW5kKChjbGllbnRfaXAsIGNsaWVudF9wb3J0KSkKICAgICAgICBwcmludChmIkNsaWVudCBib3VuZCB0byB7Y2xpZW50X2lwfTp7Y2xpZW50X3BvcnR9IikKICAgIGV4Y2VwdCBzb2NrZXQuZXJyb3IgYXMgZToKICAgICAgICBwcmludChmIkVycm9yIGNyZWF0aW5nIHNvY2tldDoge2V9IikKICAgICAgICBzeXMuZXhpdCgxKQoKICAgIGNoYWxsZW5nZXMgPSBbCiAgICAgICAgKCJXaG8gaXMgdGhlIGFyY2hpdGVjdCBvZiB0aGUgTWF0cml4PyIsICJUaGUgQXJjaGl0ZWN0IiksCiAgICAgICAgKCJXaGF0IGlzIHRoZSBsYXN0IGh1bWFuIGNpdHk/IiwgIlppb24iKSwKICAgICAgICBnZW5lcmF0ZV9yYW5kb21fbnVtYmVyLAogICAgICAgICgiV2hhdCBpcyB0aGUgTUQ1IGhhc2ggb2YgJ3JlZHBpbGwnPyIsIGhhc2hsaWIubWQ1KCJyZWRwaWxsIi5lbmNvZGUoKSkuaGV4ZGlnZXN0KCkpLAogICAgICAgIGdlbmVyYXRlX3JhbmRvbV9tYXRoX3F1ZXN0aW9uCiAgICBdCgogICAgY29ycmVjdF9yZXNwb25zZXMgPSAwCgogICAgdHJ5OgogICAgICAgIGZvciBpLCBjaGFsbGVuZ2UgaW4gZW51bWVyYXRlKGNoYWxsZW5nZXMsIDEpOgogICAgICAgICAgICBpZiBjYWxsYWJsZShjaGFsbGVuZ2UpOgogICAgICAgICAgICAgICAgcXVlc3Rpb24sIGV4cGVjdGVkID0gY2hhbGxlbmdlKCkKICAgICAgICAgICAgZWxzZToKICAgICAgICAgICAgICAgIHF1ZXN0aW9uLCBleHBlY3RlZCA9IGNoYWxsZW5nZQoKICAgICAgICAgICAgc2VuZF91ZHBfbWVzc2FnZShzb2NrLCBmIkNoYWxsZW5nZSB7aX06IHtxdWVzdGlvbn0iLCAoc2VydmVyX2lwLCBzZXJ2ZXJfcG9ydCkpCiAgICAgICAgICAgIHJlc3BvbnNlLCByZXNwX2lwID0gcmVjZWl2ZV91ZHBfbWVzc2FnZShzb2NrKQogICAgICAgICAgICBpZiByZXNwX2lwICE9IGV4cGVjdGVkX3Jlc3BvbnNlX2lwIGFuZCByZXNwX2lwICE9IE5vbmU6CiAgICAgICAgICAgICAgICBwcmludChmIldhcm5pbmc6IFJlc3BvbnNlIHJlY2VpdmVkIGZyb20gdW5leHBlY3RlZCBJUDoge3Jlc3BfaXB9IikKICAgICAgICAgICAgICAgIGJyZWFrCiAgICAgICAgICAgIGlmIHJlc3BvbnNlIGlzIE5vbmU6CiAgICAgICAgICAgICAgICBwcmludChmIkNoYWxsZW5nZSB7aX0gZmFpbGVkOiBObyByZXNwb25zZSByZWNlaXZlZC4iKQogICAgICAgICAgICAgICAgY29udGludWUgICMgVHJ5IHRoZSBuZXh0IGNoYWxsZW5nZSBpbnN0ZWFkIG9mIGJyZWFraW5nCiAgICAgICAgICAgIAogICAgICAgICAgICBwcmludChmIlJlY2VpdmVkIGZyb20ge3Jlc3BfaXB9OiB7cmVzcG9uc2V9IikKCgoKICAgICAgICAgICAgaWYgcmVzcG9uc2Uuc3RyaXAoKS5sb3dlcigpID09IGV4cGVjdGVkLmxvd2VyKCk6CiAgICAgICAgICAgICAgICBjb3JyZWN0X3Jlc3BvbnNlcyArPSAxCiAgICAgICAgICAgICAgICBwcmludChmIkNoYWxsZW5nZSB7aX0gY29ycmVjdCEiKQogICAgICAgICAgICBlbHNlOgogICAgICAgICAgICAgICAgcHJpbnQoZiJDaGFsbGVuZ2Uge2l9IGluY29ycmVjdC4gRXhwZWN0ZWQ6IHtleHBlY3RlZH0sIEdvdDoge3Jlc3BvbnNlfSIpCiAgICAgICAgICAgIAogICAgICAgICAgICB0aW1lLnNsZWVwKDEpICAjIEFkZCBhIHNtYWxsIGRlbGF5IGJldHdlZW4gcmVxdWVzdHMKCiAgICAgICAgcHJpbnQoZiJDaGFsbGVuZ2VzIGNvbXBsZXRlZC4ge2NvcnJlY3RfcmVzcG9uc2VzfSBvdXQgb2Yge2xlbihjaGFsbGVuZ2VzKX0gd2VyZSBjb3JyZWN0LiIpCiAgICAgICAgaWYgY29ycmVjdF9yZXNwb25zZXMgPT0gbGVuKGNoYWxsZW5nZXMpOgogICAgICAgICAgICBwcmludCgiTU9SUEhFVVMgTElLRVMgQ09ERVMgV0lUSCBOVU1CRVJTLiBFU1BFQ0lBTExZIENPREVTIEFTIExPTkcgQVMgVEhFIE5BTUUgT0YgVEhFIExBU1QgSFVNRU4gQ0lUWSEiKQoKICAgIGV4Y2VwdCBFeGNlcHRpb24gYXMgZToKICAgICAgICBwcmludChmIkFuIHVuZXhwZWN0ZWQgZXJyb3Igb2NjdXJyZWQ6IHtlfSIpCiAgICAgICAgaW1wb3J0IHRyYWNlYmFjawogICAgICAgIHRyYWNlYmFjay5wcmludF9leGMoKQoKICAgIGZpbmFsbHk6CiAgICAgICAgc29jay5jbG9zZSgpCgppZiBfX25hbWVfXyA9PSAiX19tYWluX18iOgogICAgbWFpbigpCg==";
    std::string userPassword;
    int attempts = 0;
    const int maxAttempts = 3;

    // Anti-debugging checks
#ifdef _WIN32
    if (isDebuggerPresent() || checkHardwareBreakpoints() || checkRunningProcesses()) {
        std::cout << "Debugger detected! Exiting." << std::endl;
        return 1;
    }
#else
    antiDebugUnix();
#endif

    while (attempts < maxAttempts) {
        std::cout << "Enter the password: ";
        std::cin >> userPassword;

        if (userPassword == CORRECT_PASSWORD) {
            std::cout << "Correct password. Decrypting and compiling..." << std::endl;
            std::string decryptedScript = decrypt_python_code(encrypted_code);
            if (saveDecryptedScript(decryptedScript)) {
                std::cout << "Decryption and compilation process completed." << std::endl;
            }
            else {
                std::cout << "Error occurred during decryption and compilation." << std::endl;
            }
            return 0;
        }
        else {
            std::cout << "Incorrect password." << std::endl;
            attempts++;
        }
    }
    
    std::cout << "Access denied. Too many incorrect attempts." << std::endl;
    return 0;
}