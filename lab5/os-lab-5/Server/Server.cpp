#include <windows.h>

#include <iostream>

#include "common.h"

int main() {
  std::cout << "Server starting..." << std::endl;

  HANDLE hPipe = CreateNamedPipe(
      PIPE_NAME,  // Имя канала
      PIPE_ACCESS_DUPLEX,  // Двусторонний доступ (чтение и запись)
      PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,  // Режим сообщений
      PIPE_UNLIMITED_INSTANCES,  // Макс. количество экземпляров
      sizeof(Request),           // Размер выходного буфера
      sizeof(Request),           // Размер входного буфера
      0,                         // Таймаут по умолчанию
      NULL  // Атрибуты безопасности по умолчанию
  );

  if (hPipe == INVALID_HANDLE_VALUE) {
    std::cerr << "CreateNamedPipe failed, GLE=" << GetLastError() << std::endl;
    return 1;
  }

  std::cout << "Pipe created. Waiting for client..." << std::endl;

  // Ожидание подключения клиента
  if (!ConnectNamedPipe(hPipe, NULL)) {
    std::cerr << "ConnectNamedPipe failed, GLE=" << GetLastError() << std::endl;
    CloseHandle(hPipe);
    return 1;
  }

  std::cout << "Client connected. Reading request..." << std::endl;

  Request req;
  DWORD bytesRead;
  if (ReadFile(hPipe, &req, sizeof(Request), &bytesRead, NULL)) {
    std::cout << "Server received request with ID: " << req.employee_id
              << std::endl;
  } else {
    std::cerr << "ReadFile from pipe failed, GLE=" << GetLastError()
              << std::endl;
  }

  DisconnectNamedPipe(hPipe);
  CloseHandle(hPipe);
  std::cout << "Server shutting down." << std::endl;
  return 0;
}