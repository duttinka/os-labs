#include <windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "employee.h"

// Function to start a process and wait for it to complete
void runProcess(const std::string& command) {
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Start the child process
  if (!CreateProcessA(NULL,  
                      const_cast<char*>(command.c_str()),  // Command line
                      NULL,   // Process handle not inheritable
                      NULL,   // Thread handle not inheritable
                      FALSE,  // Set handle inheritance to FALSE
                      0,      // No creation flags
                      NULL,   // Use parent's environment block
                      NULL,   // Use parent's starting directory
                      &si,    // Pointer to STARTUPINFO structure
                      &pi)    // Pointer to PROCESS_INFORMATION structure
  ) {
    std::cerr << "CreateProcess failed. Error code: " << GetLastError()
              << std::endl;
    return;
  }

  // Wait until the child process exits, as required by the assignment
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

int main() {
  // --- Step 1: Run Creator ---
  std::string binaryFileName;
  int numRecords;
  std::cout << "Enter binary file name and number of records: " << std::endl;
  std::cin >> binaryFileName >> numRecords;

  // Build the command line for Creator
  std::string creatorCommand =
      "Creator.exe " + binaryFileName + " " + std::to_string(numRecords);
  std::cout << "\nStarting Creator process..." << std::endl;
  runProcess(creatorCommand);
  std::cout << "Creator process finished." << std::endl;

  // --- Step 2: Display the binary file contents ---
  std::cout << "\n--- Contents of " << binaryFileName << " ---" << std::endl;
  std::ifstream binInFile(binaryFileName, std::ios::binary);
  if (!binInFile) {
    std::cerr << "Error reading binary file." << std::endl;
  } else {
    employee emp;
    while (binInFile.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
      std::cout << "ID: " << emp.num << ", Name: " << emp.name
                << ", Hours: " << emp.hours << std::endl;
    }
    binInFile.close();
  }

  // --- Step 3: Run Reporter ---
  std::string reportFileName;
  double hourlyRate;
  std::cout << "\nEnter report file name and hourly rate: " << std::endl;
  std::cin >> reportFileName >> hourlyRate;

  // Build the command line for Reporter
  std::string reporterCommand = "Reporter.exe " + binaryFileName + " " +
                                reportFileName + " " +
                                std::to_string(hourlyRate);
  std::cout << "\nStarting Reporter process..." << std::endl;
  runProcess(reporterCommand);
  std::cout << "Reporter process finished." << std::endl;

  // --- Step 4: Display the report file contents ---
  std::cout << "\n--- Contents of " << reportFileName << " ---" << std::endl;
  std::ifstream reportInFile(reportFileName);
  if (!reportInFile) {
    std::cerr << "Error reading report file." << std::endl;
  } else {
    std::string line;
    while (getline(reportInFile, line)) {
      std::cout << line << std::endl;
    }
    reportInFile.close();
  }

  std::cout << "\nMain process finished successfully." << std::endl;
  return 0;
}