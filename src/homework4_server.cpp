#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <homework4/Homework4Action.h>
#include <math.h>


class Homework4Action
{
protected:
  ros::Publisher pub_;
  ros::Subscriber sub_;
  ros::NodeHandle nh_;
  float xi,yi,zi,xf,yf,zf,desired_distance;
  bool executingCB;
  nav_msgs::Odometry* last_odometry_message;
  geometry_msgs::Twist message_velocity_target;
  actionlib::SimpleActionServer<homework4::Homework4Action> as_;
  std::string action_name_;
  homework4::Homework4Result result_;

public:

  Homework4Action(std::string name) :
    as_(nh_, name, boost::bind(&Homework4Action::executeCB, this, _1), false), action_name_(name)
  {
    as_.start();
    executingCB=false;
    pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
    sub_ = nh_.subscribe("/odom", 10000, &Homework4Action::odomCallback,this);
    ROS_INFO("Constr");
  }

  ~Homework4Action(void)  //questa funzione a che serve? Nel tutorial non è specificato
  {
  }
 
  void odomCallback(nav_msgs::Odometry msg){
    last_odometry_message=&msg;
    if(!executingCB){
     xi=last_odometry_message->pose.pose.position.x;
     yi=last_odometry_message->pose.pose.position.y;
     zi=last_odometry_message->pose.pose.position.z;
    }else{
     xf=last_odometry_message->pose.pose.position.x;
     yf=last_odometry_message->pose.pose.position.y;
     zf=last_odometry_message->pose.pose.position.z;
     bool success = true;     
     ROS_INFO("Goal received. Desired speed %f",message_velocity_target.linear.x);
     bool reached=false;   
     ROS_INFO(" xi %f\n xf %f\n yi %f\n yf %f\n zi %f\n zf %f",xi,xf,yi,yf,zi,zf);
     if(sqrt(pow(xf-xi,2) + pow(yf-yi,2) + pow(zf-zi,2))>=desired_distance){
       reached=true;
     }
     else{
       pub_.publish(message_velocity_target);
       if (as_.isPreemptRequested() || !ros::ok())
       { 
        ROS_INFO("%s: Preempted", action_name_.c_str());
        // set the action state to preempted
        as_.setPreempted();
        success = false;
        executingCB=false;
       } 
     }
     if(success and reached)
     {
       result_.odom_pose=*last_odometry_message;
       ROS_INFO("%s: Succeeded", action_name_.c_str());
       // set the action state to succeeded
       as_.setSucceeded(result_);
       executingCB=false;
     }
    }
  }
  void executeCB(const homework4::Homework4GoalConstPtr &goal)
  {
    desired_distance=goal->distance;
    message_velocity_target.linear.x=goal->desired_speed; 
    message_velocity_target.linear.y=0; 
    message_velocity_target.linear.z=0; 
    message_velocity_target.angular.x=0; 
    message_velocity_target.angular.y=0; 
    message_velocity_target.angular.z=0; 
    ROS_INFO("Goal received. Desired speed %f",message_velocity_target.linear.x);
    executingCB=true;
    while(executingCB){}
  }
};


int main(int argc, char** argv)
{
  ros::init(argc, argv, "homework4");

  Homework4Action homework4("homework4");
  ros::spin();

  return 0;
}
