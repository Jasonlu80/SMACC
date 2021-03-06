/*****************************************************************************************************************
 * ReelRobotix Inc. - Software License Agreement      Copyright (c) 2018-2020
 * 	 Authors: Pablo Inigo Blasco, Brett Aldrich
 *
 ******************************************************************************************************************/

#include <moveit_z_client/client_behaviors/cb_move_end_effector.h>
// #include <moveit/kinematic_constraints/kinematic_constraint.h>
#include <moveit/kinematic_constraints/utils.h>

namespace moveit_z_client
{
CbMoveEndEffector::CbMoveEndEffector()
{
}

CbMoveEndEffector::CbMoveEndEffector(geometry_msgs::PoseStamped target_pose, std::string tip_link)
    : targetPose(target_pose)
{
    tip_link_ = tip_link;
}

void CbMoveEndEffector::onEntry()
{
    ROS_DEBUG("Synchronous sleep of 4 seconds");
    ros::WallDuration(4).sleep();

    if (this->group_)
    {
        moveit::planning_interface::MoveGroupInterface move_group(*(this->group_));
        this->moveToAbsolutePose(move_group, movegroupClient_->planningSceneInterface, targetPose);
    }
    else
    {
        this->requiresClient(movegroupClient_);
        this->moveToAbsolutePose(movegroupClient_->moveGroupClientInterface, movegroupClient_->planningSceneInterface, targetPose);
    }
    
    ROS_DEBUG("Synchronous sleep of 3 seconds");
    ros::WallDuration(3).sleep();
}

void CbMoveEndEffector::onExit()
{
}

bool CbMoveEndEffector::moveToAbsolutePose(moveit::planning_interface::MoveGroupInterface &moveGroupInterface,
                                           moveit::planning_interface::PlanningSceneInterface &planningSceneInterface,
                                           geometry_msgs::PoseStamped &targetObjectPose)
{
    moveGroupInterface.setPlanningTime(1.0);

    ROS_INFO_STREAM("[CbMoveEndEffector] Target End efector Pose: " << targetObjectPose);

    moveGroupInterface.setPoseTarget(targetObjectPose, tip_link_);
    moveGroupInterface.setPoseReferenceFrame(targetObjectPose.header.frame_id);

    moveit::planning_interface::MoveGroupInterface::Plan computedMotionPlan;
    bool success = (moveGroupInterface.plan(computedMotionPlan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
    ROS_INFO_NAMED("CbMoveEndEffector", "Success Visualizing plan 1 (pose goal) %s", success ? "" : "FAILED");

    
    if (success)
    {
        auto executionResult = moveGroupInterface.execute(computedMotionPlan);

        if (executionResult == moveit_msgs::MoveItErrorCodes::SUCCESS)
        {
            ROS_INFO("[CbMoveEndEffector] motion execution succedded");
            movegroupClient_->postEventMotionExecutionSucceded();
        }
        else
        {
            ROS_INFO("[CbMoveEndEffector] motion execution failed");
            movegroupClient_->postEventMotionExecutionFailed();
        }
    }
    else
    {
        ROS_INFO("[CbMoveEndEffector] motion execution failed");
        movegroupClient_->postEventMotionExecutionFailed();
    }

    return success;
}

} // namespace moveit_z_client