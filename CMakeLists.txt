cmake_minimum_required(VERSION 2.8.3)
project(homework4)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  actionlib
  actionlib_msgs
  nav_msgs
  message_generation
  rospy
  std_msgs
)

find_package(Boost REQUIRED COMPONENTS system)

add_action_files(
  DIRECTORY action
  FILES Homework4.action
)

generate_messages(
  DEPENDENCIES actionlib_msgs std_msgs nav_msgs # Or other packages containing msgs
)

catkin_package(
  CATKIN_DEPENDS actionlib_msgs nav_msgs
)

include_directories(
 include
  ${catkin_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} #ho dovuto aggiungere questo "Boost" per risolvere l'errore "undefined reference to init.. "
)


#You should reorder you cmakelists so that the calls to find_package(catkin ...), catkin_package(...) and include_directories() come before any calls to add_executable(), add_library() or target_link_libraries()

add_executable(homework4_server src/homework4_server.cpp)

target_link_libraries(
  homework4_server
  ${catkin_LIBRARIES}
)

add_dependencies(
  homework4_server
  ${homework4_EXPORTED_TARGETS}
)

add_executable(homework4_client src/homework4_client.cpp)

target_link_libraries( 
  homework4_client
  ${catkin_LIBRARIES}
)

add_dependencies(
  homework4_client
  ${homework4_EXPORTED_TARGETS}
)
