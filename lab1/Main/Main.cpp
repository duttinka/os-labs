#include <windows.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common.h"

// Helper function to run a process and wait for it
// This implements SRP (Single Responsibility Principle)
bool runProcess(const std::string& command) {
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  if (!CreateProcessA(NULL, const_cast<char*>(command.c_str()), NULL, NULL,
                      FALSE, 0, NULL, NULL, &si, &pi)) {
    std::cerr << "CreateProcess failed. Error code: " << GetLastError()
              << std::endl;
    return false;
  }

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles.
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return true;
}

void displayFileContent(const std::string& filename, bool isBinary) {
  std::ios_base::openmode mode = isBinary ? std::ios::binary : std::ios::in;
  std::ifstream file(filename, mode);
  if (!file) {
    std::cerr << "Error opening file for reading: " << filename << std::endl;
    return;
  }

  if (isBinary) {
    Employee emp;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
      std::cout << "ID: " << emp.num << ", Name: " << emp.name
                << ", Hours: " << emp.hours << std::endl;
    }
  } else {
    std::string line;
    while (std::getline(file, line)) {
      std::cout << line << std::endl;
    }
  }
  file.close();
}

int main() {
  // --- Creator Stage ---
  std::string binaryFileName;
  int numRecords;
  std::cout << "Enter binary file name: ";
  std::cin >> binaryFileName;
  std::cout << "Enter number of records: ";
  std::cin >> numRecords;

  std::string creatorCommand =
      "Creator.exe " + binaryFileName + " " + std::to_string(numRecords);
  std::cout << "\nStarting Creator process..." << std::endl;
  if (!runProcess(creatorCommand)) {
    return 1;  // Exit if Creator process failed
  }
  std::cout << "Creator process finished." << std::endl;

  // --- Display Binary Stage ---
  std::cout << "\n--- Contents of " << binaryFileName << " ---" << std::endl;
  displayFileContent(binaryFileName, true);

  // --- Reporter Stage ---
  std::string reportFileName;
  double hourlyRate;
  std::cout << "\nEnter report file name: ";
  std::cin >> reportFileName;
  std::cout << "Enter hourly rate: ";
  std::cin >> hourlyRate;

  std::string reporterCommand = "Reporter.exe " + binaryFileName + " " +
                                reportFileName + " " +
                                std::to_string(hourlyRate);
  std::cout << "\nStarting Reporter process..." << std::endl;
  if (!runProcess(reporterCommand)) {
    return 1;  // Exit if Reporter process failed
  }
  std::cout << "Reporter process finished." << std::endl;

  // --- Display Report Stage ---
  std::cout << "\n--- Contents of " << reportFileName << " ---" << std::endl;
  displayFileContent(reportFileName, false);

  std::cout << "\nMain process finished successfully." << std::endl;
  return 0;
}