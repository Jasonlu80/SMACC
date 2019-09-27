
namespace SS2
{
//forward declaration for initial ssr
class SsrRadialRotate;
class SsrRadialReturn;

struct SsRadialPattern2 : smacc::SmaccState<SsRadialPattern2, SmDanceBot, SsrRadialRotate>
{
public:
    using SmaccState::SmaccState;

    typedef mpl::list<
    
                      // Expected event
                      sc::transition<EvStateFinish<SsRadialPattern2>, StNavigateReverse1>,

                      // Keyboard events
                      sc::transition<smacc::EvKeyPressN<SbKeyboard>, StNavigateReverse1>,
                      sc::transition<EvKeyPressP<SbKeyboard>,StNavigateToWaypointsX>,

                      // Error events
                      sc::transition<smacc::EvTopicMessageTimeout<LidarSensor>, StAcquireSensors>,
                      sc::transition<EvActionAborted<smacc::SmaccMoveBaseActionClient::Result>, StNavigateToWaypointsX>,

                      // Internal events
                      sc::custom_reaction<smacc::EvStateFinish<SsrRadialReturn>>
                > reactions;
        

    int iteration_count;
    int total_iterations;
    float ray_angle_increment_degree;
    float ray_length_meters;

    void onInitialize()
    {
        iteration_count = 0;
        total_iterations = 4;
        ray_angle_increment_degree = 90;
        ray_length_meters = 1;

        this->configure<KeyboardOrthogonal>(std::make_shared<SbKeyboard>());
    }

    sc::result react(const smacc::EvStateFinish<SsrRadialReturn> &ev)
    {
        ROS_INFO("Superstate count: %d", iteration_count);
        if (++iteration_count == total_iterations) // 1 == two times
        {
            this->throwFinishEvent();
        }

        return forward_event();
    }
};

//forward declaration for the superstate
using SS = SsRadialPattern2;
#include <sm_dance_bot/superstate_routines/radial_motion/ssr_radial_end_point.h>
#include <sm_dance_bot/superstate_routines/radial_motion/ssr_radial_return.h>
#include <sm_dance_bot/superstate_routines/radial_motion/ssr_radial_rotate.h>
} // namespace SS2