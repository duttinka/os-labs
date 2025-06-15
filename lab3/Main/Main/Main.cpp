#include <windows.h>

#include <iostream>
#include <vector>

CRITICAL_SECTION cs; 

// Структура для передачи данных в каждый поток marker
struct MarkerData {
  int id;
  int* arr;
  int size;
  bool terminate; // Флаг для завершения работы
  CRITICAL_SECTION* pCS;
};

// Функция потока marker
DWORD WINAPI marker_thread(LPVOID lpParam) {
  MarkerData* data = static_cast<MarkerData*>(lpParam);

  // Инициализируем генератор случайных чисел уникальным для потока значением
  srand(data->id);
  std::cout << "Marker thread " << data->id << " started." << std::endl;

  while (!data->terminate) {
    int random_index = rand() % data->size;

     EnterCriticalSection(data->pCS);
    if (data->arr[random_index] == 0) {
      Sleep(5);  // Пауза, чтобы увеличить вероятность гонки данных
      data->arr[random_index] = data->id;
      LeaveCriticalSection(data->pCS);
      Sleep(5);
    } else {
      // Если ячейка занята, просто выходим из критической секции
      LeaveCriticalSection(data->pCS);

      // Эта часть логики будет заменена событиями в следующих коммитах
      Sleep(100); 
    }
  }

  std::cout << "Marker thread " << data->id << " finished." << std::endl;
  return 0;
}

int main() {
  // 1. Создаем массив
  int size;
  std::cout << "Enter array size: ";
  std::cin >> size;
  std::vector<int> arr(size, 0);  // Инициализируем нулями

  // 2. Запрашиваем количество потоков
  int num_markers;
  std::cout << "Enter number of marker threads: ";
  std::cin >> num_markers;

  std::vector<HANDLE> thread_handles(num_markers);
  std::vector<MarkerData> marker_data(num_markers);

   // Инициализируем критическую секцию
  InitializeCriticalSection(&cs);

  // 3. Запускаем потоки
  for (int i = 0; i < num_markers; ++i) {
    marker_data[i] = {i + 1, arr.data(), size, false, &cs};
    thread_handles[i] =
        CreateThread(NULL, 0, marker_thread, &marker_data[i], 0, NULL);
  }

  // Ждем, пока пользователь не захочет завершить программу
  std::cout << "\nPress Enter to stop all threads...\n";
  std::cin.get();
  std::cin.get();

  // Сигнализируем всем потокам на завершение
  for (int i = 0; i < num_markers; ++i) {
    marker_data[i].terminate = true;
  }

  // Ожидаем завершения всех потоков
  WaitForMultipleObjects(num_markers, thread_handles.data(), TRUE, INFINITE);

  std::cout << "\nFinal array state:\n";
  for (int i = 0; i < size; ++i) {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;

  for (int i = 0; i < num_markers; ++i) {
    CloseHandle(thread_handles[i]);
  }

   DeleteCriticalSection(&cs);

  return 0;
}