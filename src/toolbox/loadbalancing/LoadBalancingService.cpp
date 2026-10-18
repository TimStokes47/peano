// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "LoadBalancingService.h"

#include "AbstractLoadBalancing.h"


tarch::logging::Log toolbox::loadbalancing::LoadBalancingService::_log("toolbox::loadbalancing::LoadBalancingService");

toolbox::loadbalancing::LoadBalancingService toolbox::loadbalancing::LoadBalancingService::_singleton;


toolbox::loadbalancing::LoadBalancingService::LoadBalancingService():
  _loadBalancing(nullptr) {}


toolbox::loadbalancing::LoadBalancingService::~LoadBalancingService() { shutdown(); }


toolbox::loadbalancing::LoadBalancingService& toolbox::loadbalancing::LoadBalancingService::getInstance() {
  return _singleton;
}


void toolbox::loadbalancing::LoadBalancingService::setLoadBalancing(AbstractLoadBalancing* loadBalancing) {
  if (_loadBalancing != nullptr and _loadBalancing != loadBalancing) {
    delete _loadBalancing;
  }
  _loadBalancing = loadBalancing;
}


toolbox::loadbalancing::AbstractLoadBalancing* toolbox::loadbalancing::LoadBalancingService::getLoadBalancing() const { return _loadBalancing; }


bool toolbox::loadbalancing::LoadBalancingService::hasLoadBalancing() const { return _loadBalancing != nullptr; }


void toolbox::loadbalancing::LoadBalancingService::shutdown() {
  delete _loadBalancing;
  _loadBalancing = nullptr;
}
