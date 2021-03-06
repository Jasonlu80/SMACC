namespace sm_pr2_plugs{
// STATE DECLARATION
struct StCloseGripper : smacc::SmaccState<StCloseGripper, MsUnplug>
{
    using SmaccState::SmaccState;

// TRANSITION TABLE
    typedef mpl::list<
        
    // Transition<EvKeyPressN<CbDefaultKeyboardBehavior, OrKeyboard>, StFetchPlug, PREEMPT>,
    Transition<EvTimer<CbTimerCountdownOnce, OrTimer>, StClearLeftArm, SUCCESS>
    
    >reactions;

// STATE FUNCTIONS
    static void staticConfigure()
    {
        configure_orthogonal<OrTimer,  CbTimerCountdownOnce>(5);    
        configure_orthogonal<OrKeyboard, CbDefaultKeyboardBehavior>();
    }

    void runtimeConfigure()
    {
    }

    void onEntry()
    {
        ROS_INFO("On Entry!");
    }

    void onExit()
    {
        ROS_INFO("On Exit!");
    }
   
};
} // namespace sm_pr2_plugs