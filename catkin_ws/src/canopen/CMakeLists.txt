
cmake_minimum_required(VERSION 3.0.2)
project(canopen)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
  i2cpwm_board
)

## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system)

catkin_package(
  CATKIN_DEPENDS message_runtime
)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

file (GLOB_RECURSE can_man_src CONFIGURE_DEPENDS "src/*.cpp")
  add_executable(${PROJECT_NAME} ${can_man_src})

 target_link_libraries(${PROJECT_NAME}
   ${catkin_LIBRARIES}
   ${LIBS}
 )