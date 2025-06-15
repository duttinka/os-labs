#include <windows.h>

#include <iostream>
#include <vector>

CRITICAL_SECTION cs; 

// Структура для передачи данных в каждый поток marker
struct MarkerData {
  int id;
  int* arr;
  int size;
  HANDLE start_event;
  HANDLE stop_event;
  HANDLE continue_event;
  HANDLE blocked_event;
  int marked_count;
};

// Функция потока marker
DWORD WINAPI marker_thread(LPVOID lpParam) {
  MarkerData* data = static_cast<MarkerData*>(lpParam);
  data->marked_count = 0;

  // 1. Ждать сигнала на старт от main
  WaitForSingleObject(data->start_event, INFINITE);

  // 2. Инициализировать генератор случайных чисел
  srand(data->id);

  // 3. Основной цикл работы
  while (true) {
    int random_index = rand() % data->size;

    EnterCriticalSection(&cs);
    if (data->arr[random_index] == 0) {
      // 3.3. Если элемент свободен
      Sleep(5);
      data->arr[random_index] = data->id;
      data->marked_count++;
      LeaveCriticalSection(&cs);
      Sleep(5);
    } else {
      // 3.4. Если элемент занят
      LeaveCriticalSection(&cs);

      std::cout << "Marker #" << data->id
                << ": Marked count: " << data->marked_count
                << ", can't mark element with index " << random_index
                << std::endl;

      // 3.4.2. Сообщить main о блокировке
      SetEvent(data->blocked_event);

      // 3.4.3. Ждать сигнала от main (продолжить или завершить)
      HANDLE events_to_wait[] = {data->continue_event, data->stop_event};
      DWORD wait_result =
          WaitForMultipleObjects(2, events_to_wait, FALSE, INFINITE);

      if (wait_result == WAIT_OBJECT_0 + 1) {  // Получен сигнал на завершение
        // 4. Завершение работы
        EnterCriticalSection(&cs);
        for (int i = 0; i < data->size; ++i) {
          if (data->arr[i] == data->id) {
            data->arr[i] = 0;
          }
        }
        LeaveCriticalSection(&cs);
        std::cout << "Marker #" << data->id << " cleaned up and is terminating."
                  << std::endl;
        return 0;  // Завершить поток
      }
      // 5. Если получен сигнал на продолжение, цикл начнется заново
    }
  }
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
  std::vector<HANDLE> blocked_events(num_markers);
  std::vector<HANDLE> stop_events(num_markers);

   // Инициализируем критическую секцию
  InitializeCriticalSection(&cs);

   // Создаем события
  HANDLE start_event = CreateEvent(NULL, TRUE, FALSE, L"StartEvent");
  HANDLE continue_event = CreateEvent(NULL, TRUE, FALSE, L"ContinueEvent");

  // 3. Запускаем потоки
  for (int i = 0; i < num_markers; ++i) {
    stop_events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    blocked_events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    marker_data[i] = {
        i + 1,          arr.data(),        size, start_event, stop_events[i],
        continue_event, blocked_events[i], 0};
    thread_handles[i] =
        CreateThread(NULL, 0, marker_thread, &marker_data[i], 0, NULL);
  }

  // 5. Дать сигнал на начало работы
  SetEvent(start_event);

  int running_threads = num_markers;
  while (running_threads > 0) {
    // 6.1. Ждать, пока все потоки не заблокируются
    WaitForMultipleObjects(running_threads, blocked_events.data(), TRUE,
                           INFINITE);

    // 6.2. Вывести массив
    std::cout << "\nArray state:\n";
    for (int i = 0; i < size; ++i) {
      std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    // 6.3. Запросить номер потока для завершения
    int thread_to_stop_id;
    std::cout << "Enter marker ID to stop: ";
    std::cin >> thread_to_stop_id;

    int thread_index = -1;
    for (int i = 0; i < num_markers; ++i) {
      if (marker_data[i].id == thread_to_stop_id) {
        thread_index = i;
        break;
      }
    }

    if (thread_index != -1 && thread_handles[thread_index] != NULL) {
      // 6.4. Подать сигнал на завершение
      SetEvent(stop_events[thread_index]);

      // 6.5. Ждать завершения
      WaitForSingleObject(thread_handles[thread_index], INFINITE);
      CloseHandle(thread_handles[thread_index]);
      thread_handles[thread_index] = NULL;

      // Удаляем "мертвые" события и хендлы из векторов, чтобы
      // WaitForMultipleObjects работал корректно
      thread_handles.erase(thread_handles.begin() + thread_index);
      blocked_events.erase(blocked_events.begin() + thread_index);
      stop_events.erase(stop_events.begin() + thread_index);
      marker_data.erase(marker_data.begin() + thread_index);

      running_threads--;

      // 6.6. Вывести массив после очистки
      std::cout << "\nArray state after cleanup:\n";
      for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
      }
      std::cout << std::endl;
    }

    if (running_threads > 0) {
      // 6.7. Подать сигнал на продолжение работы остальным
      ResetEvent(continue_event);  // Сначала сбрасываем
      SetEvent(
          continue_event);  // Потом устанавливаем, чтобы "разбудить" все потоки
    }
  }

  // 7. Завершение
  std::cout << "All marker threads have finished. Exiting." << std::endl;
  DeleteCriticalSection(&cs);
  CloseHandle(start_event);
  CloseHandle(continue_event);

  return 0;
}