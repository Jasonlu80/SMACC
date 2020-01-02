#pragma once

#include <sm_three_some/clients/cl_subscriber/cl_subscriber.h>
#include <smacc/smacc_orthogonal.h>

namespace sm_three_some
{
using namespace sm_three_some::cl_subscriber;

class OrSubscriber : public smacc::Orthogonal
{
public:
    virtual void onInitialize() override
    {
        auto subscriber_client = this->createClient<OrSubscriber, ClSubscriber>();
        subscriber_client->initialize();
    }
};
} // namespace sm_three_some