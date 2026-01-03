cmake_minimum_required(VERSION 3.8)
project(first_practice_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. 의존성 패키지 찾기
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(turtlesim REQUIRED)
# 2. 서비스 인터페이스 생성
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/ResetPath.srv"
  "action/DistTurtle.action"
)

# 3. 생성된 인터페이스를 같은 패키지 내에서 사용하기 위한 설정 (핵심)
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME} rosidl_typesupport_cpp)

# =========================================================================
# 4. 실행 파일(노드) 정의
# =========================================================================

# A. Original Talker
add_executable(talker src/talker.cpp)
ament_target_dependencies(talker rclcpp std_msgs)

# B. Listener
add_executable(listener src/listener.cpp)
ament_target_dependencies(listener rclcpp std_msgs)

# C. Turtle Command
add_executable(turtle_cmd src/turtle_cmd.cpp)
ament_target_dependencies(turtle_cmd rclcpp geometry_msgs)

# D. Reset Path Server (누락된 서버 추가)
add_executable(reset_path_server src/reset_path_server.cpp)
ament_target_dependencies(reset_path_server rclcpp)
target_link_libraries(reset_path_server "${cpp_typesupport_target}")

# E. Reset Path Client
add_executable(reset_path_client src/reset_path_client.cpp)
ament_target_dependencies(reset_path_client rclcpp)
target_link_libraries(reset_path_client "${cpp_typesupport_target}")

# F. Action Server
add_executable(dist_turtle_action_server src/dist_turtle_action_server.cpp)
ament_target_dependencies(dist_turtle_action_server 
  rclcpp 
  rclcpp_action 
  geometry_msgs
)
# 액션 인터페이스 라이브러리 연결
target_link_libraries(dist_turtle_action_server "${cpp_typesupport_target}")

# G. Action Client
add_executable(dist_turtle_action_client src/dist_turtle_action_client.cpp)
ament_target_dependencies(dist_turtle_action_client 
  rclcpp 
  rclcpp_action 
)
# 생성된 액션 인터페이스 라이브러리 연결 (필수!)
target_link_libraries(dist_turtle_action_client "${cpp_typesupport_target}")

# H. Turtle Patrol (심화: 순찰대)
add_executable(turtle_patrol src/turtle_patrol.cpp)
ament_target_dependencies(turtle_patrol 
  rclcpp 
  rclcpp_action 
  turtlesim  # Turtlesim 액션 사용을 위해 필요할 수 있음 (메시지 의존성)
)
target_link_libraries(turtle_patrol "${cpp_typesupport_target}")

# 5. 설치 규칙
install(TARGETS
  talker
  listener
  turtle_cmd
  reset_path_server
  reset_path_client
  dist_turtle_action_server
  dist_turtle_action_client
  turtle_patrol
  DESTINATION lib/${PROJECT_NAME}
)

# 6. Launch 파일 설치 (추가됨)
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
