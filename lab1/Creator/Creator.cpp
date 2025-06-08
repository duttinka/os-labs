#include <fstream>
#include <iostream>
#include <string>

#include "employee.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: Creator.exe <binary_file_name> <number_of_records>"
              << std::endl;
    return 1;
  }

  // Get file name and number of records from command line
  std::string fileName = argv[1];
  int numRecords = std::stoi(argv[2]);

  // Open the file for binary writing
  std::ofstream outFile(fileName, std::ios::binary | std::ios::trunc);
  if (!outFile) {
    std::cerr << "Error opening file for writing: " << fileName << std::endl;
    return 1;
  }

  std::cout << "Enter " << numRecords << " employee records:" << std::endl;

  for (int i = 0; i < numRecords; ++i) {
    employee emp;
    std::cout << "\nRecord " << i + 1 << ":" << std::endl;
    std::cout << "Enter employee ID: ";
    std::cin >> emp.num;
    std::cout << "Enter employee name (max 9 chars): ";
    std::cin >> emp.name;
    std::cout << "Enter hours worked: ";
    std::cin >> emp.hours;

    // Write the struct to the binary file
    outFile.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
  }

  outFile.close();
  std::cout << "\nBinary file '" << fileName << "' created successfully."
            << std::endl;

  return 0;
}