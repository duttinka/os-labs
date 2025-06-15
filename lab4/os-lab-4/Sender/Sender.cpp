#include <fstream>
#include <iostream>
#include <string>

#include "common.h"
#include "windows.h"

int main(int argc, char* argv[]) {
  // Имя файла получить из командной строки
  if (argc != 2) {
    std::cerr << "Usage: Sender.exe <filename>" << std::endl;
    return 1;
  }
  std::string filename = argv[1];

  HANDLE hFileMutex = OpenMutexA(SYNCHRONIZE, FALSE, "FileMutex");
  HANDLE hSemaphoreEmpty =
  OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "SemaphoreEmpty");
  HANDLE hSemaphoreFull =
      OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "SemaphoreFull");

  if (!hFileMutex || !hSemaphoreEmpty || !hSemaphoreFull) {
    std::cerr << "Sender: Error opening sync objects." << std::endl;
    return 1;
  }

  // Открыть файл для передачи сообщений
  std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
  if (!file) {
    std::cerr << "Sender: Error opening file." << std::endl;
    return 1;
  }

  std::cout << "Sender ready. Enter message to send (or 'exit'):" << std::endl;
  std::string input;
  while (std::getline(std::cin, input) && input != "exit") {
    Message msg;
    strncpy_s(msg.text, input.c_str(), _TRUNCATE);

    std::cout << "Sender: Waiting for empty slot..." << std::endl;
    WaitForSingleObject(hSemaphoreEmpty,
                        INFINITE);  // Ждать, пока Receiver не прочитает

    WaitForSingleObject(hFileMutex, INFINITE);  // Захватить мьютекс
    std::fstream file(filename,
                      std::ios::in | std::ios::out | std::ios::binary);
    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&msg), sizeof(Message));
    file.flush();
    std::cout << "Sender: Message sent: " << msg.text << std::endl;
    file.close();
    ReleaseMutex(hFileMutex);  // Освободить мьютекс

    ReleaseSemaphore(hSemaphoreFull, 1,
                     NULL);  // Сообщить Receiver, что сообщение готово
  }

  CloseHandle(hFileMutex);
  CloseHandle(hSemaphoreEmpty);
  CloseHandle(hSemaphoreFull);
  return 0;
}