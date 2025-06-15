#include <windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "common.h"

int main() {
  // Ввести с консоли имя бинарного файла
  std::string filename;
  std::cout << "Enter binary file name: ";
  std::cin >> filename;

  HANDLE hFileMutex = CreateMutexA(NULL, FALSE, "FileMutex");
  // Для одного сообщения
  HANDLE hSemaphoreEmpty = CreateSemaphoreA(NULL, 1, 1, "SemaphoreEmpty");
  HANDLE hSemaphoreFull = CreateSemaphoreA(NULL, 0, 1, "SemaphoreFull");

  // Создать бинарный файл для сообщений
  std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
  if (!outFile) {
    std::cerr << "Receiver: Error creating file." << std::endl;
    return 1;
  }
  outFile.close();

  // Запустить заданное количество процессов Sender (пока 1)
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
  std::cout << "Receiver ready. Enter 'read' to get message or 'exit' to quit."
            << std::endl;
  std::string command;
  while (std::cin >> command && command != "exit") {
    if (command == "read") {
      std::cout << "Receiver: Waiting for a message..." << std::endl;
      WaitForSingleObject(hSemaphoreFull,
                          INFINITE);  // Ждать, пока Sender не запишет

      WaitForSingleObject(hFileMutex, INFINITE);  // Захватить мьютекс
      std::ifstream inFile(filename, std::ios::binary);
      Message msg;
      inFile.read(reinterpret_cast<char*>(&msg), sizeof(Message));
      std::cout << "Receiver: Message received: " << msg.text << std::endl;
      inFile.close();
      ReleaseMutex(hFileMutex);  // Освободить мьютекс

      ReleaseSemaphore(hSemaphoreEmpty, 1,
                       NULL);  // Сообщить Sender, что место освободилось
    }
  }

  // ... закрытие хендлов ...
  CloseHandle(hFileMutex);
  CloseHandle(hSemaphoreEmpty);
  CloseHandle(hSemaphoreFull);
  // ...
  return 0;
}