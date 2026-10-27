// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/logging/Log.h"


namespace toolbox {
  namespace loadbalancing {
    class AbstractLoadBalancing;
    class LoadBalancingService;
  } // namespace loadbalancing
} // namespace toolbox


/**
 * Load Balancing Service
 *
 * Singleton which administers exactly one AbstractLoadBalancing instance -
 * the load balancing strategy that is currently active. User/extension code
 * (the main routine, typically) hands over an AbstractLoadBalancing subclass
 * via setLoadBalancing(), and everybody else who needs to trigger or query
 * the load balancing goes through this service instead of holding (or
 * passing around) a pointer to the strategy object itself.
 *
 * The service owns the AbstractLoadBalancing object that it administers: it
 * deletes the previous strategy whenever you install a new one via
 * setLoadBalancing(), and it deletes the last installed strategy in
 * shutdown()/the destructor.
 *
 * @see tarch::services::Service for a discussion of how singletons are
 *   realised throughout Peano.
 *
 * @author Tobias Weinzierl
 */
class toolbox::loadbalancing::LoadBalancingService {
  private:
    static tarch::logging::Log _log;

    static LoadBalancingService _singleton;

    /**
     * Owned by this service. May be nullptr if no load balancing has been
     * installed (yet).
     */
    AbstractLoadBalancing* _loadBalancing;

    /**
     * Singletons don't do anything in their constructor - see
     * tarch::services::Service for a discussion.
     */
    LoadBalancingService();

    /**
     * You may not copy a singleton.
     */
    LoadBalancingService(const LoadBalancingService& other) = delete;

  public:
    virtual ~LoadBalancingService();

    /**
     * @return The singleton instance.
     */
    static LoadBalancingService& getInstance();

    /**
     * Install a new load balancing strategy.
     *
     * Ownership of loadBalancing is handed over to the service, i.e. the
     * service will delete the object eventually (either because you replace
     * it with another call to setLoadBalancing(), or because shutdown()/the
     * destructor is invoked). If a load balancing strategy had already been
     * installed, it is deleted before the new one is stored, unless you pass
     * in the very same pointer again.
     *
     * @param loadBalancing New strategy. Might be nullptr to simply discard
     *   the previously installed strategy without installing a new one.
     */
    void setLoadBalancing(AbstractLoadBalancing* loadBalancing);

    /**
     * @return The currently installed load balancing strategy, or nullptr if
     *   none has been installed (yet).
     */
    AbstractLoadBalancing* getLoadBalancing() const;

    /**
     * @return Is a load balancing strategy installed, i.e. does
     *   getLoadBalancing() not return nullptr.
     */
    bool hasLoadBalancing() const;

    /**
     * Delete the installed load balancing strategy (if any) and reset the
     * service, i.e. getLoadBalancing() returns nullptr afterwards.
     */
    void shutdown();
};
