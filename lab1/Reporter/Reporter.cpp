#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "employee.h"

// Comparator for sorting employees by ID
bool compareEmployees(const employee& a, const employee& b) {
  return a.num < b.num;
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: Reporter.exe <binary_file_name> <report_file_name> "
                 "<hourly_rate>"
              << std::endl;
    return 1;
  }

  // Get arguments from command line
  std::string binaryFileName = argv[1];
  std::string reportFileName = argv[2];
  double hourlyRate = std::stod(argv[3]);

  // Read all records from the binary file
  std::ifstream inFile(binaryFileName, std::ios::binary);
  if (!inFile) {
    std::cerr << "Error opening binary file: " << binaryFileName << std::endl;
    return 1;
  }

  std::vector<employee> employees;
  employee emp;
  while (inFile.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
    employees.push_back(emp);
  }
  inFile.close();

  // Sort employees by ID to create an ordered list
  std::sort(employees.begin(), employees.end(), compareEmployees);

  // Create and write the report file
  std::ofstream outFile(reportFileName);
  if (!outFile) {
    std::cerr << "Error creating report file: " << reportFileName << std::endl;
    return 1;
  }

  // Write report header
  outFile << "Report for file \"" << binaryFileName << "\"\n";
  outFile << std::left << std::setw(15) << "Employee ID" << std::setw(15)
          << "Name" << std::setw(10) << "Hours" << std::setw(10) << "Salary"
          << "\n";
  outFile << "--------------------------------------------------\n";

  // Write employee data and calculated salary
  for (const auto& record : employees) {
    double salary = record.hours * hourlyRate;
    outFile << std::left << std::setw(15) << record.num << std::setw(15)
            << record.name << std::setw(10) << std::fixed
            << std::setprecision(2) << record.hours << std::setw(10)
            << std::fixed << std::setprecision(2) << salary << "\n";
  }

  outFile.close();
  std::cout << "Report file '" << reportFileName << "' generated successfully."
            << std::endl;

  return 0;
}