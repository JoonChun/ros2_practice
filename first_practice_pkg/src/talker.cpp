#include <chrono>
#include <cstdio>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
using namespace std::chrono_literals;

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  printf("hello world first_practice_pkg package\n");
  return 0;
}
