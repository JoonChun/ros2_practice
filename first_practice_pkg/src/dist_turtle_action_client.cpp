#include <functional>
#include <future>
#include <memory>
#include <string>
#include <sstream>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "first_practice_pkg/action/dist_turtle.hpp"

class DistTurtleActionClient : public rclcpp::Node
{
public:
    using DistTurtle = first_practice_pkg::action::DistTurtle;
    using GoalHandleDistTurtle = rclcpp_action::ClientGoalHandle<DistTurtle>;

    explicit DistTurtleActionClient(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
    : Node("dist_turtle_action_client", options)
    {
        this->client_ptr_ = rclcpp_action::create_client<DistTurtle>(
            this,
            "dist_turtle"
        );
        
        this->timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&DistTurtleActionClient::send_goal,this));
    }

    void send_goal()
    {
        using namespace std::placeholders;
        this->timer_->cancel(); // 목표는 한번만 실행

        if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10))) {
            RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting");
            return;
        }

        auto goal_msg = DistTurtle::Goal();
        goal_msg.dist = 2.0; // [설정] 2미터 이동 목표

        RCLCPP_INFO(this->get_logger(), "Sending goal request: Dist %f", goal_msg.dist);

        auto send_goal_options = rclcpp_action::Client<DistTurtle>::SendGoalOptions();

        // 3가지 핵심 콜백 등록
        send_goal_options.goal_response_callback = 
            std::bind(&DistTurtleActionClient::goal_response_callback, this, _1);

        send_goal_options.feedback_callback = 
            std::bind(&DistTurtleActionClient::feedback_callback, this, _1, _2);

        send_goal_options.result_callback = 
            std::bind(&DistTurtleActionClient::result_callback, this, _1);

        this->client_ptr_->async_send_goal(goal_msg, send_goal_options);
    }

private:
    rclcpp_action::Client<DistTurtle>::SharedPtr client_ptr_;
    rclcpp::TimerBase::SharedPtr timer_;

    // 1. 서버가 목표를 수락했는지 확인
    void goal_response_callback(const GoalHandleDistTurtle::SharedPtr & goal_handle)
    {
        if (!goal_handle) {
            RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
        } else{
            RCLCPP_INFO(this->get_logger(), "Goal accepted by server ,waiting for result");
        }
        
    }

    // 2. 중간 피드백 처리 (남은 거리 출력)
    void feedback_callback(
        GoalHandleDistTurtle::SharedPtr,
        const std::shared_ptr<const DistTurtle::Feedback> feedback)
    {
        RCLCPP_INFO(this->get_logger(), "Feedback: Remained Dist = %f m", feedback->remained_dist);
    }
    
    // 3. 최종 결과 처리
    void result_callback(const GoalHandleDistTurtle::WrappedResult & result)
    {
        switch(result.code) {
            case rclcpp_action::ResultCode::SUCCEEDED:
                RCLCPP_INFO(this->get_logger(), "Result: Goal Succeeded!");
                RCLCPP_INFO(this->get_logger(), "Total Dist: %f, Total Time: %f",
                    result.result->total_dist, result.result->total_time);
                break;
            case rclcpp_action::ResultCode::ABORTED:
                RCLCPP_INFO(this->get_logger(), "Goal was aborted");
                break;
            case rclcpp_action::ResultCode::CANCELED:
                RCLCPP_ERROR(this->get_logger(), "Goal was canceled");
                break;
            default:
                RCLCPP_ERROR(this->get_logger(), "Unknown result code");
                break;
        }
        rclcpp::shutdown();
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistTurtleActionClient>());
    rclcpp::shutdown();
    return 0;
}