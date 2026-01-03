from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # 1. Turtlesim 노드 실행
        Node(
            package='turtlesim',
            executable='turtlesim_node',
            name='turtlesim'
        ),

        # 2. 우리가 만든 Action Server 실행
        Node(
            package='first_practice_pkg',
            executable='dist_turtle_action_server',
            name='my_action_server',
            output='screen'
        ),

        # 3. 파라미터 적용된 Turtle Command 실행 (주석 처리됨)
        # Node(
        #     package='first_practice_pkg',
        #     executable='turtle_cmd',
        #     name='turtle_cmd_publisher',
        #     output='screen',
        #     parameters=[{
        #         'move_speed': 3.0,
        #         'turn_speed': 1.5
        #     }]
        # ),

        # 4. Turtle Patrol (심화: 순찰대) 실행
        Node(
            package='first_practice_pkg',
            executable='turtle_patrol',
            name='turtle_patrol_node',
            output='screen'
        ),

        # 5. Service Server (복귀 보고용) 실행
        Node(
            package='first_practice_pkg',
            executable='reset_path_server',
            name='reset_path_service_server',
            output='screen'
        )
    ])
