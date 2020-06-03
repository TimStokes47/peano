// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_SERVICES_SERVICE_REPOSITORY_H_
#define _TARCH_SERVICES_SERVICE_REPOSITORY_H_


#include <vector>
#include <string>

#include "tarch/services/Service.h"


namespace tarch {
  namespace services {
    class ServiceRepository;
  }
}


/**
 * Service Repository
 *
 * See the interface Service for a detailed description. The registry is a
 * service itself, but it does not register as a service.
 *
 * @author Tobias Weinzierl
 */
class tarch::services::ServiceRepository: public tarch::services::Service {
  private:
    struct ServiceEntry {
      std::string  _name;
      Service*     _service;
    };

    typedef  std::vector<ServiceEntry>      ServiceContainer;

    ServiceContainer         _services;

    ServiceRepository();
  public:
    virtual ~ServiceRepository();

    static ServiceRepository& getInstance();

    /**
     * @param service Pointer to service
     * @param name    Name of service (mandatory, not empty)
     */
    void addService( Service* const service, const std::string& name );
    bool hasService( Service* service ) const;
    void removeService( Service* const service );

    /**
     * Answer to MPI Messages
     *
     * Tell all registered services to answer to MPI messages that are still
     * pending in the MPI queues.
     *
     * <h2> MPI tuning </h2>
     *
     * I once had an implementation that issues in Iprobe before it enters the
     * receiveDanglingMessages() of all registered services. This is not a good
     * idea. It does not allow services to buffer requests and then to answer
     * them all in one rush. However, I want to do exactly this (wait until all
     * trees have finished a traversal before I insert new ones, e.g.). So I
     * removed this premature Iprobe optimisation; and debugged for ages before
     * I found out that this is the reason for deadlocks.
     *
     */
    virtual void receiveDanglingMessages();

    /**
     * @return List of registered services separated by whitespaces
     */
    std::string getListOfRegisteredServices() const;
};


#endif
