#include <windows.h>

#include <iostream>

#include "common.h"

int main() {
  std::cout << "Client starting..." << std::endl;

  // Подключение к существующему каналу
  HANDLE hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                            OPEN_EXISTING, 0, NULL);

  if (hPipe == INVALID_HANDLE_VALUE) {
    std::cerr << "CreateFile pipe failed, GLE=" << GetLastError() << std::endl;
    return 1;
  }

  std::cout << "Connected to server. Sending request..." << std::endl;

  // Отправляем тестовый запрос
  Request req;
  req.type = Request::Type::READ;
  req.employee_id = 123;

  DWORD bytesWritten;
  if (!WriteFile(hPipe, &req, sizeof(Request), &bytesWritten, NULL)) {
    std::cerr << "WriteFile to pipe failed, GLE=" << GetLastError()
              << std::endl;
  } else {
    std::cout << "Request sent." << std::endl;
  }

  CloseHandle(hPipe);
  return 0;
}