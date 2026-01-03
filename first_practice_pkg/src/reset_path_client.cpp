#include "first_practice_pkg/srv/reset_path.hpp"
#include "rclcpp/rclcpp.hpp"
#include <chrono>
using namespace std::chrono_literals;

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  // 노드 생성 (클라이언트 노드 이름)
  std::shared_ptr<rclcpp::Node> node =
      rclcpp::Node::make_shared("reset_path_service_client");

  // 1. 서비스 클라이언트 생성: 서버와 동일한 서비스 이름 ("reset_path") 지정
  rclcpp::Client<first_practice_pkg::srv::ResetPath>::SharedPtr client =
      node->create_client<first_practice_pkg::srv::ResetPath>("reset_path");

  // 2. 요청 메시지 생성
  auto request =
      std::make_shared<first_practice_pkg::srv::ResetPath::Request>();
  // 요청이 비어있으므로 별도의 필드는 채우지 않음

  // 3. 서버가 준비될 떄까지 대기
  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(node->get_logger(),
                   "Client interrupted while waiting for service to appear.");
      return 0;
    }
    RCLCPP_INFO(node->get_logger(), "Service not available, waiting again...");
  }

  // 4. 서비스 요청 비동기 전송
  auto result = client->async_send_request(request);

  // 5. 응답이 올 때까지 대기 (동기적 대기)
  if (rclcpp::spin_until_future_complete(node, result) ==
      rclcpp::FutureReturnCode::SUCCESS) {
    // 6. 응답 확인
    RCLCPP_INFO(node->get_logger(), "Successfully received service response:");

    // 결과값을 한 번만 꺼내서 response 변수에 저장합니다.
    auto response = result.get();

    RCLCPP_INFO(node->get_logger(), "Success: %s",
                response->success ? "true" : "false");
    RCLCPP_INFO(node->get_logger(), "Message: %s", response->message.c_str());
  } else {
    RCLCPP_ERROR(node->get_logger(), "Failed to call service reset_path.");
  }
  rclcpp::shutdown();
  return 0;
}
