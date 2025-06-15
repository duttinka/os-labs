#include <fstream>
#include <iostream>
#include <string>

#include "common.h"

int main(int argc, char* argv[]) {
  // 1. Имя файла получить из командной строки
  if (argc != 2) {
    std::cerr << "Usage: Sender.exe <filename>" << std::endl;
    return 1;
  }
  std::string filename = argv[1];

  // 1. Открыть файл для передачи сообщений
  std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
  if (!file) {
    std::cerr << "Sender: Error opening file." << std::endl;
    return 1;
  }

  std::cout << "Sender ready. Enter message to send:" << std::endl;
  Message msg;
  std::cin.getline(msg.text,
                   MESSAGE_MAX_LEN);  // Текст сообщения вводится с консоли

  file.seekp(0);
  file.write(reinterpret_cast<const char*>(&msg), sizeof(Message));
  file.flush();
  std::cout << "Sender: Message sent." << std::endl;

  file.close();
  return 0;
}