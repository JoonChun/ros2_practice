#include <functional>
#include <memory>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/twist.hpp"

#include "first_practice_pkg/action/dist_turtle.hpp"

class DistTurtleActionServer : public rclcpp::Node
{
public:
    using DistTurtle = first_practice_pkg::action::DistTurtle;
    using GoalHandleDistTurtle = rclcpp_action::ServerGoalHandle<DistTurtle>;

    explicit DistTurtleActionServer(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
    : Node("dist_turtle_action_server", options)
    {
        using namespace std::placeholders;

        this->action_server_= rclcpp_action::create_server<DistTurtle>(
            this,
            "dist_turtle",
            std::bind(&DistTurtleActionServer::handle_goal, this, _1, _2),
            std::bind(&DistTurtleActionServer::handle_cancel, this, _1),
            std::bind(&DistTurtleActionServer::handle_accepted, this, _1)
        );

        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        RCLCPP_INFO(this->get_logger(), "Dist Turtle Action Server Started");
    }

private:
    rclcpp_action::Server<DistTurtle>::SharedPtr action_server_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;

    // [요청 수신] 클라이언트가 목표를 보내면 호출됨
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const DistTurtle::Goal> goal)
    {
        RCLCPP_INFO(this->get_logger(), "Received goal request: move %f meteres", goal->dist);
        (void)uuid;
        // 무조건 수락 (REJECT할 조건이 있다면 여기서 처리)
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    // [취소 요청] 클라이언트가 중간에 취소하면 호출됨
    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandleDistTurtle> goal_handle)
    {
        RCLCPP_INFO(this->get_logger(), "Receied request to cancel goal");
        (void)goal_handle;
        return rclcpp_action::CancelResponse::ACCEPT;
    }
    // [실행 준비] 목표가 수락되면 실제 실행 스레드를 시작함
    void handle_accepted(const std::shared_ptr<GoalHandleDistTurtle> goal_handle)
    {
        using namespace std::placeholders;
        // execute 함수를 별도 스레드에서 실행 (오래걸리는 작업이므로 블로킹 방지)
        std::thread{std::bind(&DistTurtleActionServer::execute, this, _1), goal_handle}.detach();
    }
    // [실제 로직] 거북이를 움직이고 피드백을 주는 메인 함수
    void execute(const std::shared_ptr<GoalHandleDistTurtle> goal_handle)
    {
        RCLCPP_INFO(this->get_logger(), "Executing goal");

        const auto goal = goal_handle->get_goal();
        auto feedback = std::make_shared<DistTurtle::Feedback>();
        auto result = std::make_shared<DistTurtle::Result>();

        // 이동 설정
        auto twist = geometry_msgs::msg::Twist();
        float total_dist = 0.0;
        float linear_vel = 0.5; // m/s

        // 루프 주기 (10Hz)
        rclcpp::Rate loop_rate(10);

        // 목표 거리까지 루프 실행
        while (total_dist < goal->dist && rclcpp::ok())
        {
            // 1. 취소 요청이 들어왔는지 확인
            if (goal_handle->is_canceling()) {
                goal_handle->canceled(result);
                RCLCPP_INFO(this->get_logger(), "Goal canceled");

                // 멈춤 명령 전송
                twist.linear.x = 0.0;
                publisher_->publish(twist);
                return;
            }

            // 2. 거북이 이동 명령
            twist.linear.x = linear_vel;
            publisher_->publish(twist);

            // 3. 이동 거리 계산 (거리=속도*시간)
            // 정확한 계싼은 Poise를 구독해야하지만, 여기서는 단순화하여 계산
            total_dist += linear_vel * 0.1; // 0.1초마다 루프

            // 4. 피드백 전송
            feedback->remained_dist = goal->dist - total_dist;
            goal_handle->publish_feedback(feedback);

            RCLCPP_INFO(this->get_logger(), "Feedback: Remained %f m", feedback->remained_dist);

            loop_rate.sleep();
        }

        // 5. 실행 완료: 거북이 정지 및 결과 전송
        twist.linear.x = 0.0;
        publisher_->publish(twist);

        if (rclcpp::ok()) {
            result->total_dist = total_dist;
            result->total_time = total_dist / linear_vel;
            goal_handle->succeed(result);
            RCLCPP_INFO(this->get_logger(), "Goal succeeded: Moved %f m in %f seconds", result->total_dist, result->total_time);
        }
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistTurtleActionServer>());
    rclcpp::shutdown();

    return 0;
}