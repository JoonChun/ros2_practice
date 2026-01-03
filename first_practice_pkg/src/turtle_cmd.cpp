#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include <chrono>

using namespace std::chrono_literals;

class TurtleCmdPublisher : public rclcpp::Node {
public:
  TurtleCmdPublisher() : Node("turtle_cmd_publisher") {
    // 1. Publisher 생성 : '/turtle1/cmd_vel' 토픽에 geometry_msgs::msg::Twist
    // 메시지 타입으로 퍼블리셔 생성
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "/turtle1/cmd_vel", 10);

    // 파라미터 선언 및 기본값 설정
    this->declare_parameter("move_speed", 2.0);
    this->declare_parameter("turn_speed", 1.0);

    // 2. 100ms(0.1초)마다 time_callback 함수 실행 (10Hz)
    timer_ = this->create_wall_timer(
        100ms, std::bind(&TurtleCmdPublisher::timer_callback, this));
  }

private:
  void timer_callback() {
    // 3. twist 메세지 객체 생성
    auto msg = geometry_msgs::msg::Twist();

    // 파라미터 값 읽어오기 (실시간으로 변경된 값 반영을 위해 콜백 내에서 호출)
    double move_speed_val = this->get_parameter("move_speed").as_double();
    double turn_speed_val = this->get_parameter("turn_speed").as_double();

    // 4. 속도 설정 (경로계획 알고리즘의 출력값이 들어갈 자리)
    msg.linear.x = move_speed_val;
    msg.angular.z = turn_speed_val; // 반시계 방향 회전

    publisher_->publish(msg);
    RCLCPP_INFO(this->get_logger(),
                "Moving with speed: linear=%.2f, angular=%.2f", move_speed_val,
                turn_speed_val);
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TurtleCmdPublisher>());
  rclcpp::shutdown();
  return 0;
}
