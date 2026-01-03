#include "first_practice_pkg/action/dist_turtle.hpp"
#include "first_practice_pkg/srv/reset_path.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "turtlesim/action/rotate_absolute.hpp"
#include <cmath>

using DistTurtle = first_practice_pkg::action::DistTurtle;
using RotateAbsolute = turtlesim::action::RotateAbsolute;
using ResetPath = first_practice_pkg::srv::ResetPath;
using DistTurtleGoalHandle = rclcpp_action::ClientGoalHandle<DistTurtle>;
using RotateAbsoluteGoalHandle =
    rclcpp_action::ClientGoalHandle<RotateAbsolute>;

class TurtlePatrolNode : public rclcpp::Node {
public:
  TurtlePatrolNode()
      : Node("turtle_patrol_node") // Node명 작성
  {
    // 1. action clients 생성
    dist_turtle_client_ =
        rclcpp_action::create_client<DistTurtle>(this, "dist_turtle");
    rotate_absolute_client_ = rclcpp_action::create_client<RotateAbsolute>(
        this, "turtle1/rotate_absolute");

    // 2. Service Client 생성
    reset_path_client_ = this->create_client<ResetPath>("reset_path");

    // 3. 순찰 시작 (타이머를 이용해 노드 시작 후 조금 두이ㅔ 실행)
    timer_ = this->create_wall_timer(
        std::chrono::seconds(2),
        std::bind(&TurtlePatrolNode::start_patrol, this));
  }

private:
  void start_patrol() {
    timer_->cancel(); // 한번만 실행
    RCLCPP_INFO(this->get_logger(), "!!! 순찰 시작 !!!");
    move_forward();
  }

  // Action 1: 직진 (DistTurtle)
  void move_forward() {
    if (!dist_turtle_client_->wait_for_action_server(
            std::chrono::seconds(10))) {
      RCLCPP_ERROR(this->get_logger(), "DistTurtle 서버를 찾을 수 없습니다");
      return;
    }
    auto goal_msg = DistTurtle::Goal();
    goal_msg.dist = 2.0;

    RCLCPP_INFO(this->get_logger(), "직진 중... (%.2fm)", goal_msg.dist);

    auto send_goal_options =
        rclcpp_action::Client<DistTurtle>::SendGoalOptions();
    send_goal_options.result_callback = std::bind(
        &TurtlePatrolNode::handle_move_result, this, std::placeholders::_1);

    dist_turtle_client_->async_send_goal(goal_msg, send_goal_options);
  }

  void handle_move_result(const DistTurtleGoalHandle::WrappedResult &result) {
    if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
      RCLCPP_INFO(this->get_logger(), "직진 완료");
      rotate();
    } else {
      RCLCPP_ERROR(this->get_logger(), "직진 실패 또는 중단됨");
    }
  }

  // Action 2: 회전 (RotateAbsolute)
  void rotate() {
    if (patrol_count_ >= 4) {
      finish_patrol();
      return;
    }

    if (!rotate_absolute_client_->wait_for_action_server(
            std::chrono::seconds(10))) {
      RCLCPP_ERROR(this->get_logger(),
                   "RotateAbsolute 서버를 찾을 수 없습니다");
      return;
    }

    auto goal_msg = RotateAbsolute::Goal();
    // 현재 각도에서 90도씩 더해가며 회전
    // 0 -> 90 -> 180 -> 270 -> 360
    float target_theta = (patrol_count_ + 1) * (M_PI / 2.0);

    goal_msg.theta = target_theta;

    RCLCPP_INFO(this->get_logger(), "회전 중... (목표: %.2f rad)",
                goal_msg.theta);

    auto send_goal_options =
        rclcpp_action::Client<RotateAbsolute>::SendGoalOptions();
    send_goal_options.result_callback = std::bind(
        &TurtlePatrolNode::handle_rotate_result, this, std::placeholders::_1);
    rotate_absolute_client_->async_send_goal(goal_msg, send_goal_options);
  }

  void
  handle_rotate_result(const RotateAbsoluteGoalHandle::WrappedResult &result) {
    if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
      RCLCPP_INFO(this->get_logger(), "회전 완료");
      patrol_count_++;
      move_forward();
    } else {
      RCLCPP_ERROR(this->get_logger(), "회전 실패 또는 중단됨");
    }
  }

  // --- service: 완료 보고 ---
  void finish_patrol() {
    RCLCPP_INFO(this->get_logger(), "순찰 코스 완주! 복귀 보고 중...");
    auto request = std::make_shared<ResetPath::Request>();

    reset_path_client_->async_send_request(
        request, [this](rclcpp::Client<ResetPath>::SharedFuture future) {
          auto response = future.get();
          if (response->success) {
            RCLCPP_INFO(this->get_logger(), "본부 응답: %s",
                        response->message.c_str());
          } else {
            RCLCPP_ERROR(this->get_logger(), "보고 실패.");
          }
          rclcpp::shutdown();
        });
  }

  rclcpp_action::Client<DistTurtle>::SharedPtr dist_turtle_client_;
  rclcpp_action::Client<RotateAbsolute>::SharedPtr rotate_absolute_client_;
  rclcpp::Client<ResetPath>::SharedPtr reset_path_client_;
  rclcpp::TimerBase::SharedPtr timer_;
  int patrol_count_ = 0;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TurtlePatrolNode>());
  rclcpp::shutdown();
  return 0;
}
