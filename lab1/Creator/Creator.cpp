#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common.h"

int main(int argc, char* argv[]) {
  // Check for correct argument count
  if (argc != 3) {
    std::cerr << "Usage: Creator.exe <binary_file_name> <number_of_records>"
              << std::endl;
    return 1;
  }

  const std::string fileName = argv[1];
  int numRecords = 0;
  try {
    numRecords = std::stoi(argv[2]);
  } catch (const std::invalid_argument& e) {
    std::cerr << "Error: Invalid number of records provided." << std::endl;
    return 1;
  }

  // Open the file for binary writing
  std::ofstream outFile(fileName, std::ios::binary);
  if (!outFile) {
    std::cerr << "Error opening file for writing: " << fileName << std::endl;
    return 1;
  }

  std::cout << "Enter data for " << numRecords << " employees:" << std::endl;
  for (int i = 0; i < numRecords; ++i) {
    Employee emp;
    std::cout << "\nRecord " << i + 1 << ":" << std::endl;
    std::cout << "Enter employee ID: ";
    std::cin >> emp.num;
    std::cout << "Enter employee name (max 9 chars): ";
    std::cin >> emp.name;
    std::cout << "Enter hours worked: ";
    std::cin >> emp.hours;

    // Write the struct to the binary file
    outFile.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
  }

  outFile.close();
  std::cout << "\nBinary file '" << fileName << "' created successfully."
            << std::endl;

  return 0;
}