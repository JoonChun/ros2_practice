# ROS 2 Communication Practice Package (C++)

This package (`first_practice_pkg`) demonstrates the implementation of core ROS 2 communication patterns: **Topics, Services, and Actions** using C++ in ROS 2 Humble.

The project focuses on controlling a `turtlesim` node, simulating scenarios relevant to autonomous driving path planning (e.g., sending velocity commands, resetting paths, and navigating specific distances).

## 🛠 Environment
- **OS:** Ubuntu 22.04 LTS (Jammy Jellyfish)
- **ROS Distro:** ROS 2 Humble Hawksbill
- **Language:** C++ 14/17
- **Dependencies:** `turtlesim`, `rclcpp`, `rclcpp_action`, `geometry_msgs`, `std_msgs`

## 🚀 Installation

### 1. Prerequisites
Make sure you have ROS 2 Humble and Turtlesim installed.
```bash
sudo apt update
sudo apt install ros-humble-desktop ros-humble-turtlesim
```
### 2. Build the Package
```bash
mkdir -p ~/colcon_ws/src
cd ~/colcon_ws/src
git clone [YOUR_GITHUB_REPO_URL]
cd ~/colcon_ws
colcon build --symlink-install --packages-select first_practice_pkg
source install/setup.bash
```
### 📚 Features & Usage
Before running any node, ensure turtlesim is running in a separate terminal:
```bash
ros2 run turtlesim turtlesim_node

# Terminal 1: Publisher
ros2 run first_practice_pkg talker

# Terminal 2: Subscriber
ros2 run first_practice_pkg listener

# Terminal 3: Move Turtle (Circle motion)
ros2 run first_practice_pkg turtle_cmd
```

## 📂 Project Structure
```text
first_practice_pkg/
├── launch/
│   └── turtle_launch.py       # Launch file for patrol + server
├── action/
│   └── DistTurtle.action      # Action Interface Definition
├── srv/
│   └── ResetPath.srv          # Service Interface Definition
├── src/
│   ├── talker.cpp             # Topic Publisher
│   ├── listener.cpp           # Topic Subscriber
│   ├── turtle_cmd.cpp         # Twist Publisher for Turtlesim
│   ├── reset_path_server.cpp  # Service Server
│   ├── reset_path_client.cpp  # Service Client
│   ├── dist_turtle_action_server.cpp # Action Server
│   ├── dist_turtle_action_client.cpp # Action Client
│   └── turtle_patrol.cpp      # Advanced: Patrol Node (Action/Service Client)
├── CMakeLists.txt             # Build configuration
└── package.xml                # Package dependencies
```

## 🐢 Patrol Turtle (A to Z)

The Patrol Turtle implementation demonstrates a more advanced control flow using **ROS 2 Actions and Services**, rather than simple Topics.

### A. Concept
The "Patrol" behavior involves robust navigation between points. We use **Actions** for long-running movement tasks (providing feedback) and **Services** for instantaneous requests (reporting completion).

### B. Implementation (The "How-To")
1. **Action Clients**: 
   - `DistTurtle`: Moves the turtle forward by a specific distance.
   - `RotateAbsolute`: Rotates the turtle to a specific absolute angle.
2. **Service Client**: 
   - `ResetPath`: Reports to the headquarters (Server) when the patrol is complete.
3. **Logic (State Machine)**:
   - **Step 1**: Send `DistTurtle` goal (Move 2m).
   - **Step 2**: On success, send `RotateAbsolute` goal (Rotate 90 deg).
   - **Step 3**: Repeat 4 times to draw a square.
   - **Step 4**: Call `ResetPath` service to finish.

### C. Execution
Use the launch file to start the simulator, action server, service server, and the patrol node all at once:
```bash
ros2 launch first_practice_pkg turtle_launch.py
```

### Z. Key Takeaway
By the end of this exercise, you understand how to coordinate multiple ROS 2 communication patterns (Actions for movement, Services for transactions) to create a complex robot behavior.

