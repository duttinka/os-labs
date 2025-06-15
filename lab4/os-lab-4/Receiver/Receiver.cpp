#include <windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "common.h"

int main() {
  // 1. Ввести с консоли имя бинарного файла
  std::string filename;
  std::cout << "Enter binary file name: ";
  std::cin >> filename;

  // 1. Создать бинарный файл для сообщений
  std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
  if (!outFile) {
    std::cerr << "Receiver: Error creating file." << std::endl;
    return 1;
  }
  outFile.close();

  // 3. Запустить заданное количество процессов Sender (пока 1)
  std::string command_line = "Sender.exe " + filename;
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  if (!CreateProcessA(NULL, const_cast<char*>(command_line.c_str()), NULL, NULL,
                      FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
    std::cerr << "Receiver: CreateProcess failed." << std::endl;
    return GetLastError();
  }

  std::cout << "Receiver: Waiting for Sender to write a message..."
            << std::endl;
  WaitForSingleObject(pi.hProcess,
                      INFINITE);  // Временно ждем завершения процесса Sender
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  // Читать сообщение из бинарного файла
  std::ifstream inFile(filename, std::ios::binary);
  Message msg;
  inFile.read(reinterpret_cast<char*>(&msg), sizeof(Message));
  std::cout << "Receiver: Message received: " << msg.text << std::endl;
  inFile.close();

  return 0;
}