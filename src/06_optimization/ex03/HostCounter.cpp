#include "HostCounter.h"

HostCounter::HostCounter()
{
}

void HostCounter::notifyHost(std::string hostname)
{
    myHosts.insert(hostname);
}

int HostCounter::getNbOfHosts()
{
    return myHosts.size();
}
