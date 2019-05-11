// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_STACKS_STDVECTOR_STACK_H_
#define _PEANO4_STACKS_STDVECTOR_STACK_H_


#include <vector>
#include <algorithm>


#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"


namespace peano4 {
  namespace stacks {
    template <class T>
    class STDVectorStack;

    template <>
    class STDVectorStack<double>;

    enum class IOMode {
      None,
      MPISend,
	  MPIReceive
    };
  }
}


/**
 * Vertex Stack Based upon C++'s STD Stack
 *
 * @author Tobias Weinzierl
 * @version $Revision: 1.2 $
 */
template <>
class peano4::stacks::STDVectorStack<double> {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    /**
     * This is the attribute holding all the temporary stacks.
     */
    std::vector< double >   _data;

    int                     _currentElement;

  protected:
    IOMode              _ioMode;
    int                 _ioRank;
    int                 _ioTag;
    MPI_Request*        _ioMPIRequest;

  public:
    /**
     * Constructor.
     *
     * @see EventStack::EventStack()
     */
    STDVectorStack();
    ~STDVectorStack();
    STDVectorStack<double>( const STDVectorStack<double>& stack );
    STDVectorStack<double>&  operator=( const STDVectorStack<double>& stack );
    void  clone( const STDVectorStack<double>&  data );


    /**
     * This class represents a whole block of the tree. You can access all
     * element within in random order, or you can pop/push elements. If we
     * grab a block from the tree, it is logically removed from the main stack.
     */
    class PopBlockVertexStackView {
      protected:
        /**
         * Parent is friend
         */
        friend class peano4::stacks::STDVectorStack<double>;

        const int                                _size;
        const int                                _baseElement;
        peano4::stacks::STDVectorStack<double>*  _stack;

        /**
         * Constructor
         */
        PopBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStack<double>* stack);

      public:
        int size() const;
        double get(int index);
        std::string toString() const;
    };

    class PushBlockVertexStackView {
      protected:
        /**
         * Parent is friend
         */
        friend class peano4::stacks::STDVectorStack<double>;

        /**
         * Constructor
         */
        PushBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStack<double>* stack);

        const int                                _size;
        const int                                _baseElement;
        peano4::stacks::STDVectorStack<double>*  _stack;
      public:
        int size() const;

        double* set(int index, const double& value);

        #if PeanoDebug>=1
        double get(int index) const;
        #endif

        std::string toString() const;
    };

    bool isSendingOrReceiving() const;
    double pop();
    double& top(int shift=0);
    void push( const double& element );
    PopBlockVertexStackView  popBlock(int numberOfVertices);
    PushBlockVertexStackView  pushBlock(int numberOfVertices);
    int size() const;
    bool empty() const;
    void clear();
    void startSend(int rank, int tag);
    void startReceive(int rank, int tag, int numberOfElements);
    void finishSendOrReceive();
    void reverse();
};



/**
 * If you use this class, you have to ensure that the type T is generated via
 * DaStGen and that you've called initDatatype on T prior to the first
 * communication call on this stack.
 */
template <class T>
class peano4::stacks::STDVectorStack {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    /**
     * This is the attribute holding all the temporary stacks.
     */
    std::vector< T >   _data;

    int                _currentElement;

  protected:
    IOMode              _ioMode;
    int                 _ioRank;
    int                 _ioTag;
    MPI_Request*        _ioMPIRequest;

  public:
    /**
     * Constructor.
     *
     * @see EventStack::EventStack()
     */
    STDVectorStack():
      _data(),
	  _currentElement(0),
	  _ioMode(IOMode::None),
	  _ioMPIRequest(nullptr) {
    }

    ~STDVectorStack() {
    }

    /**
     * One is allowed to clone/copy a stack, but it has to be empty.
     * Usually only when we cut the domain into pieces.
     */
    STDVectorStack<T>( const STDVectorStack<T>& stack ):
	  _data(),
      _currentElement(0),
	  _ioMode(IOMode::None),
	  _ioMPIRequest(nullptr) {
      assertionMsg( stack._currentElement==0, "may not copy non-empty stack" );
      assertionMsg( stack._ioMPIRequest==nullptr, "may not copy sending/receiving stack" );
    }


    /**
     * One is not allowed to copy a stack but it has to be empty.
     */
    STDVectorStack<T>&  operator=( const STDVectorStack<T>& stack ) {
      assertionEquals( stack._currentElement, 0 );
      assertionEquals( _currentElement, 0 );
      assertion( _data.empty() );
    }


    /**
     * Clone data into the current object on which clone() is called.
     */
    void  clone( const STDVectorStack<T>&  data ) {
      assertionEquals( _currentElement, 0 );
      _currentElement = data._currentElement;
      _data           = data._data;
    }


    /**
     * This class represents a whole block of the tree. You can access all
     * element within in random order, or you can pop/push elements. If we
     * grab a block from the tree, it is logically removed from the main stack.
     */
    class PopBlockVertexStackView {
      protected:
        /**
         * Parent is friend
         */
        friend class peano4::stacks::STDVectorStack<T>;

        const int                           _size;
        const int                           _baseElement;
        peano4::stacks::STDVectorStack<T>*  _stack;

        /**
         * Constructor
         */
        PopBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStack<T>* stack):
          _size(size),
          _baseElement(base),
          _stack(stack) {
        }

      public:
        int size() const {
          return _size;
        }

        T get(int index) {
          assertion2( index>=0, index, _size );
          assertion2( index<_size, index, _size );
          return _stack->_data[_baseElement+index];
        }

        std::string toString() const {
          std::ostringstream msg;
          msg << "(size=" << _size
              << ",baseElement=" << _baseElement
              << ")";
          return msg.str();
        }
    };

    class PushBlockVertexStackView {
      protected:
        /**
         * Parent is friend
         */
        friend class peano4::stacks::STDVectorStack<T>;

        /**
         * Constructor
         */
        PushBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStack<T>* stack):
          _size(size),
		  _baseElement(base),
          _stack(stack) {
        }

        const int                           _size;
        const int                           _baseElement;
        peano4::stacks::STDVectorStack<T>*  _stack;
      public:
        int size() const {
          return _size;
        }

        /**
         * @return Pointer to element set
         */
        T* set(int index, const T& value) {
          assertion2( index>=0, index, _size );
          assertion2( index<_size, index, _size );
          _stack->_data[_baseElement+index] = value;
          return &(_stack->_data[_baseElement+index]);
        }

        #if PeanoDebug>=1
        T get(int index) const {
          assertion2( index>=0, index, _size );
          assertion2( index<_size, index, _size );
          return _stack->_data[_baseElement+index];
        }
        #endif

        std::string toString() const {
          std::ostringstream msg;
          msg << "(size=" << _size
              << ",baseElement=" << _baseElement
              << ")";
          return msg.str();
        }
    };

    /**
     * Pops element from a stack.
     */
    T pop() {
      assertion(_currentElement>0);
      _currentElement--;
      return _data[_currentElement];
    }


    T& top(int shift=0) {
      //assertion2(_currentElement>shift, _currentElement, shift);
      return _data[_currentElement-1-shift];
    }

    /**
     * Pushes element to a stack.
     */
    void push( const T& element ) {
      if (_currentElement >= static_cast<int>(_data.size()) ) {
        assertion(_currentElement - static_cast<int>(_data.size()) <= 1 );
        _data.push_back(element);
      }
      else {
        _data[_currentElement] = element;
      }
      _currentElement++;
    }

    /**
     * This operation grabs numberOfVertices from the input stack en block and
     * returns a view to it. Subsequent pops do not affect this block anymore,
     * i.e. the stack is reduced immediately.
     *
     * @return Pointer to block. Your are responsible to delete this view afterwards.
     */
    PopBlockVertexStackView  popBlock(int numberOfVertices) {
      _currentElement-=numberOfVertices;
      assertion( _currentElement>=0 );

      PopBlockVertexStackView result(numberOfVertices, _currentElement, this);
      return result;
    }

    /**
     * Push a block on the output stack
     *
     * Pushing a block on the output stack basically means that we move the
     * stack pointer by numberOfVertices entries. A block write stems from a
     * regular subgrid, i.e. the corresponding subgrid remains constant, but
     * it might happen that other grid parts processed before have added new
     * vertices. So, we might have to increase the stack size before we open
     * the push view on the stack. Also, the swapping of the stacks might
     * imply that the current output stack is not big enough - the input stack
     * might be, but we are using two distinguished stack data structures.
     *
     * @param numberOfVertices Size of the view
     */
    PushBlockVertexStackView  pushBlock(int numberOfVertices) {
      PushBlockVertexStackView result(numberOfVertices, _currentElement, this);

      _currentElement+=numberOfVertices;

      if (static_cast<int>(_data.size())<_currentElement) {
    	_data.resize(_currentElement);
      }

      return result;
    }

    int size() const {
      return _currentElement;
    }

    bool empty() const {
      return _currentElement==0;
    }

    void clear() {
      _currentElement = 0;
    }

    /**
     * Always pairs up with a finish... call. When we trigger the send, the
     * code still might insert additional elements, i.e. starting does not
     * mean all the data that is to be sent out is already in the container.
     */
    void startSend(int rank, int tag) {
      #ifdef Parallel
      assertion( _ioMode==IOMode::None );
      _ioMode = IOMode::MPISend;
      _ioTag  = tag;
      _ioRank = rank;

      assertion( _ioMPIRequest == nullptr );
      _ioMPIRequest = new MPI_Request;
      int result = MPI_Isend( _data.data(), _data.size(), T::Datatype, _ioRank, _ioTag, tarch::mpi::Rank::getInstance().getCommunicator(), _ioMPIRequest);
      if  (result!=MPI_SUCCESS) {
        logError( "startSend(int,int)", "was not able to send to node " << rank << " on tag " << tag
          << ": " << tarch::mpi::MPIReturnValueToString(result)
        );
      }
      #endif
    }

    /**
     * @see startSend()
     */
    void startReceive(int rank, int tag, int numberOfElements) {
      #ifdef Parallel
      assertion( _ioMode==IOMode::None );
      _ioMode = IOMode::MPIReceive;
      _ioTag  = tag;
      _ioRank = rank;

      _data.resize(numberOfElements);
      _currentElement = numberOfElements;
      assertionEquals( _data.size(), numberOfElements );

      assertion( _ioMPIRequest == nullptr );
      _ioMPIRequest = new MPI_Request;
      int result = MPI_Irecv( _data.data(), _data.size(), T::Datatype, _ioRank, _ioTag, tarch::mpi::Rank::getInstance().getCommunicator(), _ioMPIRequest);
      if  (result!=MPI_SUCCESS) {
        logError( "startReceive(int,int,int)", "was not able to receive " << numberOfElements << " values from node " << rank << " on tag " << tag
           << ": " << tarch::mpi::MPIReturnValueToString(result)
        );
      }
      #endif
    }


    bool isSendingOrReceiving() const {
   	  return _ioMode==IOMode::MPISend or _ioMode==IOMode::MPIReceive;
    }


    void finishSendOrReceive() {
      #ifdef Parallel
      assertion( _ioMode==IOMode::MPISend or _ioMode==IOMode::MPIReceive );
      assertion( _ioMPIRequest!=nullptr );

      _ioMode = IOMode::None;

      int          flag = 0;
      int          result;
      clock_t      timeOutWarning   = -1;
      clock_t      timeOutShutdown  = -1;
      bool         triggeredTimeoutWarning = false;
      result = MPI_Test( _ioMPIRequest, &flag, MPI_STATUS_IGNORE );
      while (!flag) {
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp();
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp();
        result = MPI_Test( _ioMPIRequest, &flag, MPI_STATUS_IGNORE );
        if (result!=MPI_SUCCESS) {
          logError("finishSendOrReceive()", "failed" );
        }
        if (
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() &&
          (clock()>timeOutWarning) &&
          (!triggeredTimeoutWarning)
        ) {
          tarch::mpi::Rank::getInstance().writeTimeOutWarning(
            "peano4::stacks::STDVectorStack<double>",
            "finishSendOrReceive()", _ioRank,_ioTag, _data.size()
           );
           triggeredTimeoutWarning = true;
         }
         if (
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
           (clock()>timeOutShutdown)
         ) {
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut(
             "peano4::stacks::STDVectorStack<double>",
             "finishSendOrReceive()", _ioRank,_ioTag, _data.size()
           );
         }
        tarch::mpi::Rank::getInstance().receiveDanglingMessages();
      }
      delete _ioMPIRequest;
      _ioMPIRequest = nullptr;
      #endif
    }

    void reverse() {
      std::reverse(std::begin(_data), std::end(_data));
    }
};


template <class T>
tarch::logging::Log peano4::stacks::STDVectorStack<T>::_log( "peano4::stacks::STDVectorStack<T>" );


#endif
