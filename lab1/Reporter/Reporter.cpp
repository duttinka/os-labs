#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "common.h"

// Comparator for sorting employees by ID
bool compareEmployees(const Employee& a, const Employee& b) {
  return a.num < b.num;
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: Reporter.exe <binary_file> <report_file> <hourly_rate>"
              << std::endl;
    return 1;
  }

  const std::string binaryFileName = argv[1];
  const std::string reportFileName = argv[2];
  double hourlyRate = 0.0;
  try {
    hourlyRate = std::stod(argv[3]);
  } catch (const std::invalid_argument& e) {
    std::cerr << "Error: Invalid hourly rate provided." << std::endl;
    return 1;
  }

  std::ifstream inFile(binaryFileName, std::ios::binary);
  if (!inFile) {
    std::cerr << "Error opening binary file: " << binaryFileName << std::endl;
    return 1;
  }

  std::vector<Employee> employees;
  Employee emp;
  while (inFile.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
    employees.push_back(emp);
  }
  inFile.close();

  std::sort(employees.begin(), employees.end(), compareEmployees);

  std::ofstream outFile(reportFileName);
  if (!outFile) {
    std::cerr << "Error creating report file: " << reportFileName << std::endl;
    return 1;
  }

  outFile << "Report for file \"" << binaryFileName << "\"\n";
  outFile << std::left << std::setw(15) << "Employee ID" << std::setw(15)
          << "Name" << std::setw(10) << "Hours" << std::setw(10) << "Salary"
          << "\n";
  outFile << "--------------------------------------------------\n";

  outFile << std::fixed << std::setprecision(2);
  for (const auto& record : employees) {
    double salary = record.hours * hourlyRate;
    outFile << std::left << std::setw(15) << record.num << std::setw(15)
            << record.name << std::setw(10) << record.hours << std::setw(10)
            << salary << "\n";
  }

  outFile.close();
  std::cout << "Report file '" << reportFileName << "' generated successfully."
            << std::endl;

  return 0;
}