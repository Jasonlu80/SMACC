#include <smacc/orthogonal.h>
#include <smacc/impl/smacc_state_machine_impl.h>
#include <smacc/smacc_substate_behavior.h>

namespace smacc
{
void Orthogonal::setStateMachine(ISmaccStateMachine *value)
{
    this->stateMachine_ = value;
}

void Orthogonal::setStateBehavior(std::shared_ptr<smacc::SmaccSubStateBehavior> statebehavior)
{
    if (statebehavior != nullptr)
    {
        ROS_INFO("Setting Ortho %s State behavior: %s", this->getName().c_str(), statebehavior->getName().c_str());
        statebehavior->stateMachine = this->stateMachine_;
        currentBehavior = statebehavior;
    }
    else
    {
        ROS_INFO("Not behavioral State by orthogonal");
    }
}

std::string Orthogonal::getName() const
{
    return demangleSymbol(typeid(*this).name());
}

void Orthogonal::onEntry()
{
    if (currentBehavior != nullptr)
    {
        ROS_INFO("Orthogonal %s OnEntry, current Behavior: %s",
                 this->getName().c_str(),
                 currentBehavior->getName().c_str());

        currentBehavior->onEntry();
    }
    else
    {
        ROS_INFO("Orthogonal %s OnEntry",
                 this->getName().c_str());
    }
}

void Orthogonal::onExit()
{
    if (currentBehavior != nullptr)
    {
        ROS_INFO("Orthogonal %s OnExit, current Behavior: %s", this->getName().c_str(), currentBehavior->getName().c_str());
        currentBehavior->onExit();
        currentBehavior = nullptr;
    }
    else
    {
        ROS_INFO("Orthogonal %s OnExit", this->getName().c_str());
    }
}
} // namespace smacc