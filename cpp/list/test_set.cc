#include <set>
#include <iostream>

int main()
{
  std::set<int> numbers;
  std::cout << "Initially, numbers.empty(): " << numbers.empty() << '\n';

  numbers.insert(42);
  numbers.insert(13317);
  std::cout << "After adding elements, numbers.empty(): " << numbers.empty() << '\n';

  const std::set<int> numbers2;
  numbers2 = numbers;
}
