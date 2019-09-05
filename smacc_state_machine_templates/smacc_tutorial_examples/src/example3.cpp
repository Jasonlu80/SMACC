#include <smacc_core/smacc.h>
#include <smacc_navigation_plugin/move_base_to_goal.h>
#include <tf/tf.h>

//--------------------------------------------------------------------------------------
using namespace smacc;

// states structs forward declarations
struct Navigate;
struct ExecuteToolState;

//--------------------------------------------------------------------------------------
struct SimpleStateMachine
    : public SmaccStateMachineBase<SimpleStateMachine,Navigate>
{
  SimpleStateMachine(my_context ctx, SignalDetector *signalDetector)
      : SmaccStateMachineBase<SimpleStateMachine,Navigate>(ctx, signalDetector)
      {
      }
};
//----------------------------------------------------------
struct Navigate : SmaccState<Navigate, SimpleStateMachine> 
{

public:
  // With this line we specify that we are going to react to any EvActionResult event
  // generated by SMACC when the action server provides a response to our request
  typedef mpl::list<sc::transition<EvActionResult<SmaccMoveBaseActionClient::Result>, ExecuteToolState>> reactions;


  // This is the action client resource (it basically is a wrapper of the ROS Action Client for move base), please check the SMACC
  // code to see how to implement your own action client
  smacc::SmaccMoveBaseActionClient *moveBaseClient_;

  // This is the substate constructor. This code will be executed when the
  // workflow enters in this substate (that is according to statechart the moment when this object is created)
  Navigate(my_context ctx):
    SmaccState<Navigate, SimpleStateMachine> (ctx)
  {
    ROS_INFO("Entering Navigate");

    // this substate will need access to the "MoveBase" resource or plugin. In this line
    // you get the reference to this resource.
    context<SimpleStateMachine>().requiresComponent<smacc::SmaccMoveBaseActionClient>(moveBaseClient_ , ros::NodeHandle("move_base"));
    goToEndPoint();
  }

  // auxiliar function that defines the motion that is requested to the move_base action server
  void goToEndPoint() {
    geometry_msgs::PoseStamped radialStartPose;
    this->getGlobalSMData("radial_start_pose", radialStartPose);

    smacc::SmaccMoveBaseActionClient::Goal goal;
    goal.target_pose.header.stamp = ros::Time::now();

    goal.target_pose = radialStartPose;
    goal.target_pose.pose.position.x = 10;
    goal.target_pose.pose.position.y = 10;
    goal.target_pose.pose.orientation =
        tf::createQuaternionMsgFromRollPitchYaw(0, 0, M_PI);

    moveBaseClient_->sendGoal(goal);
  }
};

//----------------------------------------------------------------------------
struct ExecuteToolState : SmaccState<ExecuteToolState, SimpleStateMachine>
{
ExecuteToolState(my_context ctx):
    SmaccState<ExecuteToolState, SimpleStateMachine> (ctx)
    {
    }
};

//--------------------------------------------------------------------------------------
int main(int argc, char **argv) {
  // initialize the ros node
  ros::init(argc, argv, "example3");
  ros::NodeHandle nh;

  smacc::run<SimpleStateMachine>();
}