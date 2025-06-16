#pragma once

// »м€ именованного канала
#define PIPE_NAME L"\\\\.\\pipe\\lab5_pipe"

// —труктура дл€ записи в файле
struct employee {
  int num;  // идентификационный номер сотрудника
  char name[10];  // им€ сотрудника
  double hours;   // количество отработанных часов
};

// —труктура дл€ запроса от клиента к серверу
struct Request {
  enum class Type { READ, MODIFY, RELEASE, EXIT } type;
  int employee_id;
  employee record;
};