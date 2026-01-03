#include "first_practice_pkg/srv/reset_path.hpp"
#include "rclcpp/rclcpp.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

class ResetPathServiceServer : public rclcpp::Node {
public:
  ResetPathServiceServer() : Node("reset_path_service_server") {
    RCLCPP_INFO(this->get_logger(), "Starting Reset Path Service Server...");

    // 1. 서비스 서버 생성: "reset_path"라는 이름의 서비스를 생성하고 콜백 함수
    // 등록
    service_ = this->create_service<first_practice_pkg::srv::ResetPath>(
        "reset_path", std::bind(&ResetPathServiceServer::handle_service_request,
                                this, _1, _2));
  }

private:
  // 2. 서비스 요청이 들어왔을 떄 실행될 콜백 함수
  void handle_service_request(
      const std::shared_ptr<first_practice_pkg::srv::ResetPath::Request>
          request,
      const std::shared_ptr<first_practice_pkg::srv::ResetPath::Response>
          response) {
    (void)request; // 요청은 비워뒀으므로 사용하지 않음

    // 3. 서버 로직 실행ㅣ 경로 리셋 작업을 수행했다고 가정
    RCLCPP_INFO(this->get_logger(),
                "Received path reset request. Processing...");

    // 4. 응답(response) 설정
    response->success = true;
    response->message = "Global path planning moudle has bees reset.";

    RCLCPP_INFO(this->get_logger(), "Sending Response: Success=%s",
                response->success ? "true" : "false");
  }

  rclcpp::Service<first_practice_pkg::srv::ResetPath>::SharedPtr service_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  // 서버 노드를 실행하고 요청을 기다립니다.
  rclcpp::spin(std::make_shared<ResetPathServiceServer>());
  rclcpp::shutdown();
  return 0;
}