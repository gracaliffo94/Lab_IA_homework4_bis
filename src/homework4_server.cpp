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
  float xi,yi,zi,xf,yf,zf;
  bool executingCB;
  nav_msgs::Odometry* last_odometry_message;
  geometry_msgs::Twist message_velocity_target;
  actionlib::SimpleActionServer<homework4::Homework4Action> as_; // NodeHandle instance must be created before this line. Otherwise strange error occurs.
  std::string action_name_;
  // create messages that are used to published result
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

 }
    //ROS_INFO("ODOM: X %f \n Y %f\n Z %f", last_odometry_message->pose.pose.position.x,last_odometry_message->pose.pose.position.y,last_odometry_message->pose.pose.position.z);
  }
  void executeCB(const homework4::Homework4GoalConstPtr &goal)
  {

    while(last_odometry_message==NULL){
      ROS_INFO("WAITING FOR ODOMETRY MESSAGE");
    }
    bool success = true;
    executingCB=true;
    message_velocity_target.linear.x=goal->desired_speed; 
    message_velocity_target.linear.y=0; 
    message_velocity_target.linear.z=0; 
    message_velocity_target.angular.x=0; 
    message_velocity_target.angular.y=0; 
    message_velocity_target.angular.z=0; 
    ROS_INFO("Goal received. Desired speed %f",message_velocity_target.linear.x);
    
    // publish info to the console for the user
  //  ROS_INFO("%s: Executing,  %i with seeds %i, %i", action_name_.c_str(), goal->order, feedback_.sequence[0], feedback_.sequence[1]);

 //   ROS_INFO("Goal received4\n position x %f\n position y %f, position z %f",(last_odometry_message)->pose.pose.position.x,(last_odometry_message)->pose.pose.position.y,(last_odometry_message)->pose.pose.position.z);
    bool reached=false;   
    while(!reached){
        ROS_INFO(" xi %f\n xf %f\n yi %f\n yf %f\n zi %f\n zf %f",xi,xf,yi,yf,zi,zf);
  //      ROS_INFO("DISTANCE %f, TARGET %f ",sqrt(pow(xf-xi,2) + pow(yf-yi,2) + pow(zf-zi,2)),goal->distance);
        if(sqrt(pow(xf-xi,2) + pow(yf-yi,2) + pow(zf-zi,2))>=goal->distance){
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
    	    break;
    	   } //(last_odometry_message->pose.pose.position.x)<((&initial_position)->x)
        }
    }
    if(success)
    {
      result_.odom_pose=*last_odometry_message;
      ROS_INFO("%s: Succeeded", action_name_.c_str());
      // set the action state to succeeded
      as_.setSucceeded(result_);
    }
    executingCB=false;
  }


};


int main(int argc, char** argv)
{
  ros::init(argc, argv, "homework4");

  Homework4Action homework4("homework4");
  ros::spin();

  return 0;
}
