/*******************************************************************************

 * This notice, including the copyright notice and permission notice, 
 * must be retained in all copies or substantial portions of the Software and 
 * in all derivative works.
 *
 * Software Name: CryptoDbSS-IPFS-Implementation
 * Copyright (C) 2025 Steeven J Salazar.
 * License: CryptoDbSS: Software Review and Audit License
 * 
 * https://github.com/CryptoDbSS/IPFS-Implementation
 *
 * IMPORTANT: Before using, compiling, or doing anything with this software,
 * you must read and accept the terms of the License provided with this software.
 *
 * If you do not have a copy of the License, you can obtain it at the following link:
 * https://github.com/CryptoDbSS/IPFS-Implementation/blob/main/LICENSE.md
 *
 * By using, compiling, or modifying this software, you implicitly accept
 * the terms of the License. If you do not agree with the terms,
 * do not use, compile, or modify this software.
 * 
 * This software is provided "as is," without warranty of any kind.
 * For more details, see the LICENSE file.

********************************************************************************/

#include <iostream>
#include <cstdio> 
#include <string>
#include <unistd.h>  
#include <sys/wait.h>
#include <array>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <signal.h>
#include <regex>
#include <fcntl.h>
#include <fstream>
#include <chrono>
#include <thread>



#ifndef IPFS_KUBO_WRAPPER_H
#define IPFS_KUBO_WRAPPER_H

using namespace std;

string KuboPatchBin = "ipfs";
pid_t ipfsDaemonPid = 0;

void terminateLockingProcess() {
    std::ifstream lockFile("~/.ipfs/repo.lock");
    if (!lockFile.good()) {
        std::cerr << "Error: Could not open repo.lock." << std::endl;
        return;
    }

    // Attempt to lock the file. This will fail if it's already locked.
    int fd = open("~/.ipfs/repo.lock", O_RDWR);
    if (fd == -1 && errno == EACCES) { // File is locked
        std::string line;
        while (getline(lockFile, line)) {
            //Regex to find the PID. This will need to be adjusted based on the contents of the lock file.
            std::regex pidRegex("PID: (\\d+)");
            std::smatch match;
            if(std::regex_search(line, match, pidRegex) && match.size() > 1){
                int pid = std::stoi(match[1].str());
                if (kill(pid, SIGTERM) == 0) {
                    std::cout << "Process with PID " << pid << " terminated successfully." << std::endl;
                    return;
                } else {
                    std::cerr << "Error: Failed to terminate process with PID " << pid << "." << std::endl;
                    return;
                }
            }
        }
    } else if (fd != -1){
        close(fd); //Close the file descriptor if it was opened.
    } else {
        std::cerr << "Error: repo.lock is not locked, or other error occured" << std::endl;
    }
}

void Stop_Daemon(){
    if (ipfsDaemonPid != 0) {
        if (kill(ipfsDaemonPid, SIGTERM) == 0) {
            std::cout << "IPFS daemon terminated successfully." << std::endl;
        } else {
            std::cerr << "Failed to terminate IPFS daemon." << std::endl;
        }

        int status;
        waitpid(ipfsDaemonPid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "IPFS daemon exited with status: " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cerr << "IPFS daemon terminated abnormally." << std::endl;
        }
        ipfsDaemonPid = 0;
    }

}

void Run_Daemon() {

    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Failed to fork process." << std::endl;
        return;
    }

    if (pid == 0) { // Proceso hijo (daemon)
        std::string command = KuboPatchBin + " daemon > /dev/null 2>&1 &";
        execl("/bin/bash", "bash", "-c", command.c_str(), nullptr);
        std::cerr << "Failed to execute IPFS daemon." << std::endl;
        exit(EXIT_FAILURE);
    } else { // Proceso padre
        ipfsDaemonPid = pid; // Guardar el PID

        // Espera un corto tiempo para que el daemon se inicie.
        // std::this_thread::sleep_for(std::chrono::seconds(5));

        std::cout << "IPFS daemon started with PID: " << pid<< std::endl;
        std::cout << "Seeding files: "  << std::endl;

        time_t timingthread= time(nullptr);
        while(time(nullptr)<timingthread+10){ 

        }

        Stop_Daemon();
        
    }
}

string cat(string &CID) {

    // Command to execute
    std::string command = KuboPatchBin+" cat " + CID;

    // Open a pipe to the command
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to open pipe." << std::endl;
        return "Failed to open pipe.";
    }

    // Read the output into a variable
    std::string data;
    char buffer[65536];

    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        data += buffer;
    }

    // Close the pipe
    int status = pclose(pipe);
    if (status == -1) {
        std::cerr << "Failed to close pipe." << std::endl;
        return "Failed to close pipe.";
    }

    // Check if the command succeeded
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
    } else {
        std::cerr << "Command failed with exit code: " << WEXITSTATUS(status) << std::endl;
    }

    return data;
}

string addFile(const string &patch) {

    int pipefd[2]; // File descriptors for the pipe
    if (pipe(pipefd) == -1) {
        return "Failed to create pipe.";
    }

    pid_t pid = fork(); // Create a child process
    if (pid == -1) {
        close(pipefd[0]); // Close pipe file descriptors
        close(pipefd[1]);
        return "Failed to fork.";
    }

    if (pid == 0) {

        // Child process: Redirect stdout to the pipe
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipefd[1]); // Close the original write end of the pipe

        // Execute the IPFS command using bash

        string commandtobash = KuboPatchBin + " add --pin=true " +patch;
        execl("/bin/bash", "bash", "-c", commandtobash.c_str() , nullptr);

        // If execl() fails
        std::cerr << "Failed to execute command." << std::endl;
        exit(EXIT_FAILURE); // Exit the child process with failure status
    } else {
        // Parent process: Read the output from the pipe
        close(pipefd[1]); // Close the write end of the pipe

        std::array<char, 128> buffer;
        std::string output;
        ssize_t bytesRead;

        while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
            output.append(buffer.data(), bytesRead);
        }

        close(pipefd[0]); // Close the read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus == 0) {

                // Extract the CID from the output (assuming the output format is "added <CID> <filename>")
                size_t cidStart = output.find("added ");
                if (cidStart == std::string::npos) {
                    return "Failed to extract CID from output: 'added ' not found. Command output:\n" + output;
                }
                cidStart += 6; // Skip "added "

                size_t cidEnd = output.find(' ', cidStart);
                if (cidEnd == std::string::npos) {
                    return "Failed to extract CID from output: Space after CID not found. Command output:\n" + output;
                }

                std::string cid = output.substr(cidStart, cidEnd - cidStart);

                return cid;
            } else {
                return "Command failed with exit code: " + std::to_string(exitStatus) + ". Command output:\n" + output;
            }

        } else {
            return "Command did not exit normally.";
        }
    }

    return "unexpected error";
}

string addStr(const string &MSG) {

    int pipefd[2]; // File descriptors for the pipe
    if (pipe(pipefd) == -1) {
        return "Failed to create pipe.";
    }

    pid_t pid = fork(); // Create a child process
    if (pid == -1) {
        close(pipefd[0]); // Close pipe file descriptors
        close(pipefd[1]);
        return "Failed to fork.";
    }

    if (pid == 0) {

        // Child process: Redirect stdout to the pipe
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipefd[1]); // Close the original write end of the pipe

        // Execute the IPFS command using bash

        string commandtobash =  "echo "+MSG+" | "+ KuboPatchBin + " add ";

        execl("/bin/bash", "bash", "-c", commandtobash.c_str() , nullptr);

        // If execl() fails
        std::cerr << "Failed to execute command." << std::endl;
        exit(EXIT_FAILURE); // Exit the child process with failure status
    } else {
        // Parent process: Read the output from the pipe
        close(pipefd[1]); // Close the write end of the pipe

        std::array<char, 128> buffer;
        std::string output;
        ssize_t bytesRead;

        while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
            output.append(buffer.data(), bytesRead);
        }

        close(pipefd[0]); // Close the read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus == 0) {

                // Extract the CID from the output (assuming the output format is "added <CID> <filename>")
                size_t cidStart = output.find("added ");
                if (cidStart == std::string::npos) {
                    return "Failed to extract CID from output: 'added ' not found. Command output:\n" + output;
                }
                cidStart += 6; // Skip "added "

                size_t cidEnd = output.find(' ', cidStart);
                if (cidEnd == std::string::npos) {
                    return "Failed to extract CID from output: Space after CID not found. Command output:\n" + output;
                }

                std::string cid = output.substr(cidStart, cidEnd - cidStart);

                return cid;
            } else {
                return "Command failed with exit code: " + std::to_string(exitStatus) + ". Command output:\n" + output;
            }

        } else {
            return "Command did not exit normally.";
        }
    }

    return "unexpected error";
}

string getHash(string &patch) {


    int pipefd[2]; // File descriptors for the pipe
    if (pipe(pipefd) == -1) {
        return "Failed to create pipe.";
    }

    pid_t pid = fork(); // Create a child process
    if (pid == -1) {
        close(pipefd[0]); // Close pipe file descriptors
        close(pipefd[1]);
        return "Failed to fork.";
    }

    if (pid == 0) {

        // Child process: Redirect stdout to the pipe
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipefd[1]); // Close the original write end of the pipe

        // Execute the IPFS command using bash

    string commandtobash = KuboPatchBin + " add -q --only-hash " +patch;

        execl("/bin/bash", "bash", "-c", commandtobash.c_str() , nullptr);

        // If execl() fails
        std::cerr << "Failed to execute command." << std::endl;
        exit(EXIT_FAILURE); // Exit the child process with failure status
    } else {
        // Parent process: Read the output from the pipe
        close(pipefd[1]); // Close the write end of the pipe

        std::array<char, 128> buffer;
        std::string output;
        ssize_t bytesRead;

        while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
            output.append(buffer.data(), bytesRead);
        }

        close(pipefd[0]); // Close the read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus == 0) {

                return output;
            } else {
                return "Command failed with exit code: " + std::to_string(exitStatus) + ". Command output:\n" + output;
            }

        } else {
            return "Command did not exit normally.";
        }
    }

    return "unexpected error";
}

string CIDToBase16(const string &patch) {


    int pipefd[2]; // File descriptors for the pipe
    if (pipe(pipefd) == -1) {
        return "Failed to create pipe.";
    }

    pid_t pid = fork(); // Create a child process
    if (pid == -1) {
        close(pipefd[0]); // Close pipe file descriptors
        close(pipefd[1]);
        return "Failed to fork.";
    }

    if (pid == 0) {

        // Child process: Redirect stdout to the pipe
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipefd[1]); // Close the original write end of the pipe

        // Execute the IPFS command using bash

    string commandtobash = KuboPatchBin + " cid format -b base16 " +patch;

        execl("/bin/bash", "bash", "-c", commandtobash.c_str() , nullptr);

        // If execl() fails
        std::cerr << "Failed to execute command." << std::endl;
        exit(EXIT_FAILURE); // Exit the child process with failure status
    } else {
        // Parent process: Read the output from the pipe
        close(pipefd[1]); // Close the write end of the pipe

        std::array<char, 128> buffer;
        std::string output;
        ssize_t bytesRead;

        while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
            output.append(buffer.data(), bytesRead);
        }

        close(pipefd[0]); // Close the read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus == 0) {

                return output;
            } else {
                return "Command failed with exit code: " + std::to_string(exitStatus) + ". Command output:\n" + output;
            }

        } else {
            return "Command did not exit normally.";
        }
    }

    return "unexpected error";
}

string CIDToBase58(const string &patch) {

    int pipefd[2]; // File descriptors for the pipe
    if (pipe(pipefd) == -1) {
        return "Failed to create pipe.";
    }

    pid_t pid = fork(); // Create a child process
    if (pid == -1) {
        close(pipefd[0]); // Close pipe file descriptors
        close(pipefd[1]);
        return "Failed to fork.";
    }

    if (pid == 0) {

        // Child process: Redirect stdout to the pipe
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipefd[1]); // Close the original write end of the pipe

        // Execute the IPFS command using bash

        string commandtobash = KuboPatchBin + " cid format -b base58btc " +patch;

        execl("/bin/bash", "bash", "-c", commandtobash.c_str() , nullptr);

        // If execl() fails
        std::cerr << "Failed to execute command." << std::endl;
        exit(EXIT_FAILURE); // Exit the child process with failure status
    } else {
        // Parent process: Read the output from the pipe
        close(pipefd[1]); // Close the write end of the pipe

        std::array<char, 128> buffer;
        std::string output;
        ssize_t bytesRead;

        while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
            output.append(buffer.data(), bytesRead);
        }

        close(pipefd[0]); // Close the read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus == 0) {

                return output;
            } else {
                return "Command failed with exit code: " + std::to_string(exitStatus) + ". Command output:\n" + output;
            }

        } else {
            return "Command did not exit normally.";
        }
    }

    return "unexpected error";
}

string getHashBytesVector(const std::string& cid) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "Error: Failed to create pipe." << std::endl;
        return {};
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Error: Failed to fork." << std::endl;
        close(pipefd[0]);
        close(pipefd[1]);
        return {};
    }

    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Assuming KuboPatchBin is defined elsewhere (e.g., "./BIN/ipfs")
        std::string commandtobash = KuboPatchBin + " dag get " + cid;
        std::cerr << "Executing command: " << commandtobash << std::endl;
        execl("/bin/bash", "bash", "-c", commandtobash.c_str(), nullptr);

        std::cerr << "Error: Failed to execute command." << std::endl;
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[1]);

        std::array<char, 4096> buffer;
        std::string output;
        ssize_t bytesRead;

        while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
            output.append(buffer.data(), bytesRead);
        }

        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            std::cerr << "ipfs dag get output: " << output << std::endl;

            // Parse JSON output to extract data bytes
            std::string dataKey = "\"bytes\":\"";
            size_t startPos = output.find(dataKey);
            if (startPos == std::string::npos) {
                std::cerr << "Error: Data bytes not found in JSON." << std::endl;
                return {};
            }

            startPos += dataKey.length();
            size_t endPos = output.find("\"", startPos);
            if (endPos == std::string::npos) {
                std::cerr << "Error: End of data bytes not found." << std::endl;
                return {};
            }

            std::string base64Data = output.substr(startPos, endPos - startPos);
            std::cerr << "Base64 Data: " << base64Data << std::endl;

            try {
                return base64Data;
            } catch (const std::runtime_error& e) {
                std::cerr << "Error: Base64 decoding failed: " << e.what() << std::endl;
                return {};
            }
        } else {
            std::cerr << "Error: ipfs dag get command failed." << std::endl;
            return {};
        }
    }

    return {};
}

std::string addBytesToIPFS(const std::vector<unsigned char>& bytes) {
    // Convertir el vector de bytes a una cadena hexadecimal
    std::stringstream hexStream;
    for (unsigned char byte : bytes) {
        hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::string hexString = hexStream.str();

    // Crear el comando para ipfs dag put
    std::string command = "echo -n '" + hexString + "' | xxd -r -p | "+KuboPatchBin+" dag put";

    // Ejecutar el comando utilizando popen
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "Error al ejecutar el comando.";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);

    // Extraer el CID del resultado
    //(el resultado de ipfs dag put es el CID)
    if(result.length() > 0){
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end()); //remove new lines.
    }

    return result;
}

#endif