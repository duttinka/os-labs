#include <iostream>
#include <vector>
#include <algorithm> 

#define NOMINMAX

#include <windows.h>
#include <limits>

// A structure to hold all the data that will be shared between threads
struct ThreadData {
  int* arr;
  int size;
  int min_val;
  int max_val;
  double average_val;
};

// Thread function to find the min and max elements
DWORD WINAPI min_max_thread(LPVOID lpParam) {
  ThreadData* data = static_cast<ThreadData*>(lpParam);

  data->min_val = std::numeric_limits<int>::max();
  data->max_val = std::numeric_limits<int>::min();

  for (int i = 0; i < data->size; ++i) {
    if (data->arr[i] < data->min_val) {
      data->min_val = data->arr[i];
    }
    Sleep(7);  // Sleep for 7 milliseconds after each comparison
    if (data->arr[i] > data->max_val) {
      data->max_val = data->arr[i];
    }
    Sleep(7);  // Sleep for 7 milliseconds after each comparison
  }

  std::cout << "min_max_thread: Minimum element is " << data->min_val
            << std::endl;
  std::cout << "min_max_thread: Maximum element is " << data->max_val
            << std::endl;
  return 0;
}

// Thread function to calculate the average value
DWORD WINAPI average_thread(LPVOID lpParam) {
  ThreadData* data = static_cast<ThreadData*>(lpParam);
  double sum = 0;
  for (int i = 0; i < data->size; ++i) {
    sum += data->arr[i];
    Sleep(12);  // Sleep for 12 milliseconds after each summation
  }
  data->average_val = sum / data->size;

  std::cout << "average_thread: Average value is " << data->average_val
            << std::endl;
  return 0;
}

int main() {
  // 1. Create an array with size and elements from console input
  int n;
  std::cout << "Enter array size: ";
  std::cin >> n;

  std::vector<int> numbers(n);
  std::cout << "Enter " << n << " array elements: ";
  for (int i = 0; i < n; ++i) {
    std::cin >> numbers[i];
  }

  // Prepare the shared data structure
  ThreadData data;
  data.arr = numbers.data();
  data.size = n;

  // 2. Create the min_max and average threads
  HANDLE threads[2];
  threads[0] = CreateThread(NULL, 0, min_max_thread, &data, 0, NULL);
  threads[1] = CreateThread(NULL, 0, average_thread, &data, 0, NULL);

  if (threads[0] == NULL || threads[1] == NULL) {
    return GetLastError();
  }

  // 3. Wait for both threads to complete
  std::cout << "\nMain thread: Waiting for worker threads to finish...\n";
  WaitForMultipleObjects(2, threads, TRUE, INFINITE);
  std::cout << "Main thread: Worker threads finished.\n\n";

  CloseHandle(threads[0]);
  CloseHandle(threads[1]);

  // 4. Replace min and max elements with the average
  int min_index = -1;
  int max_index = -1;
  for (int i = 0; i < data.size; ++i) {
    if (data.arr[i] == data.min_val) min_index = i;
    if (data.arr[i] == data.max_val) max_index = i;
  }

  if (min_index != -1) {
    data.arr[min_index] = static_cast<int>(data.average_val);
  }
  if (max_index != -1) {
    data.arr[max_index] = static_cast<int>(data.average_val);
  }

  std::cout << "Resulting array: ";
  for (int i = 0; i < data.size; ++i) {
    std::cout << data.arr[i] << " ";
  }
  std::cout << std::endl;

  // 5. Terminate
  return 0;
}