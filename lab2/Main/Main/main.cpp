#include <windows.h>

#include <iostream>
#include <vector>

// A structure to hold all the data that will be shared between threads
struct ThreadData {
  int* arr;
  int size;
  int min_val;
  int max_val;
  double average_val;
};

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

  std::cout << "Main thread finished." << std::endl;

  return 0;
}#include <iostream>
#include <vector>
#include <windows.h>

// A structure to hold all the data that will be shared between threads
struct ThreadData {
    int* arr;
    int size;
    int min_val;
    int max_val;
    double average_val;
};

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

    std::cout << "Main thread finished." << std::endl;

    return 0;
}