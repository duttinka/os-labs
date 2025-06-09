#include <iostream>
#include <vector>

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

  // 2. Create the min_max thread
  HANDLE hMinMaxThread = CreateThread(NULL, 0, min_max_thread, &data, 0, NULL);
  if (hMinMaxThread == NULL) {
    return GetLastError();
  }

  // Temporarily wait for the thread to finish for testing
  WaitForSingleObject(hMinMaxThread, INFINITE);
  CloseHandle(hMinMaxThread);

  std::cout << "Main thread finished." << std::endl;

  return 0;
}