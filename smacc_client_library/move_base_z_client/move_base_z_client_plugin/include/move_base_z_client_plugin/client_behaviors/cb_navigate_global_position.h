#pragma once

#include <smacc/smacc_client_behavior.h>
#include <boost/optional.hpp>
#include <geometry_msgs/Point.h>
#include <move_base_z_client_plugin/move_base_z_client_plugin.h>
#include <odom_tracker/odom_tracker.h>
#include <planner_switcher/planner_switcher.h>
#include <tf/tf.h>

namespace sm_dance_bot
{
class CbNavigateGlobalPosition : public smacc::SmaccClientBehavior
{
public:
  boost::optional<geometry_msgs::Point> initialPoint;
  boost::optional<float> initialYaw;

  CbNavigateGlobalPosition()
  {
  }

  CbNavigateGlobalPosition(float x, float y, float yaw)
  {
    auto p = geometry_msgs::Point();
    p.x = x;
    p.y = y;
    initialPoint = p;
    initialYaw = yaw;
  }

  virtual void onEntry()
  {
    ROS_INFO("Entering Navigate Global position");

    // this substate will need access to the "MoveBase" resource or plugin. In this line
    // you get the reference to this resource.
    this->requiresClient(moveBaseClient_);
    auto* odomTracker_ = this->getComponent<odom_tracker::OdomTracker>();

    ROS_INFO("Component requirements completed");

    moveBaseClient_->plannerSwitcher_->setDefaultPlanners();
    this->odomTracker_->setWorkingMode(odom_tracker::WorkingMode::RECORD_PATH_FORWARD);

    goToRadialStart();
  }

  // auxiliar function that defines the motion that is requested to the move_base action server
  void goToRadialStart()
  {
    ROS_INFO("Sending Goal to MoveBase");
    smacc::ClMoveBaseZ::Goal goal;
    goal.target_pose.header.frame_id = "/odom";
    goal.target_pose.header.stamp = ros::Time::now();
    readStartPoseFromParameterServer(goal);

    // store the start pose on the state machine storage so that it can
    // be referenced from other states (for example return to radial start)
    this->stateMachine->setGlobalSMData("radial_start_pose", goal.target_pose);

    moveBaseClient_->sendGoal(goal);
  }

  void readStartPoseFromParameterServer(smacc::ClMoveBaseZ::Goal &goal)
  {
    if (!initialPoint)
    {
      this->currentState->getParam("start_position_x", goal.target_pose.pose.position.x);
      this->currentState->getParam("start_position_y", goal.target_pose.pose.position.y);
      double yaw;
      this->currentState->getParam("start_position_yaw", yaw);

      goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(yaw);
    }
    else
    {
      goal.target_pose.pose.position = *initialPoint;
      goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(*initialYaw);
    }

    ROS_INFO_STREAM("start position read from parameter server: " << goal.target_pose.pose.position);
  }

  // This is the substate destructor. This code will be executed when the
  // workflow exits from this substate (that is according to statechart the moment when this object is destroyed)
  virtual void onExit() override
  {
    ROS_INFO("Exiting move goal Action Client");
  }

private:
  // keeps the reference to the move_base resorce or plugin (to connect to the move_base action server).
  // this resource can be used from any method in this state
  smacc::ClMoveBaseZ *moveBaseClient_;
};
} // namespace sm_dance_bot