// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_STACKS_STDVECTOR_STACK_H_
#define _PEANO4_STACKS_STDVECTOR_STACK_H_


#include <vector>

#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"


namespace peano4 {
  namespace stacks {
    template <class T>
    class STDVectorStack;

    template <class T>
    class STDVectorStackForUserDefinedData;
  }
}


/**
 * Vertex Stack Based upon C++'s STD Stack
 *
 * @author Tobias Weinzierl
 * @version $Revision: 1.2 $
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
    enum class IOMode {
      None,
      MPISend,
	  MPIReceive
    };

    IOMode              _ioMode;
    int                 _ioRank;
    int                 _ioTag;

  public:
    /**
     * Constructor.
     *
     * @see EventStack::EventStack()
     */
    STDVectorStack():
      _data(),
	  _currentElement(0),
	  _ioMode(IOMode::None) {
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
	  _ioMode(IOMode::None) {
      assertionMsg( stack._currentElement==0, "may not copy non-empty stack" );
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

        void set(int index, const T& value) {
          assertion2( index>=0, index, _size );
          assertion2( index<_size, index, _size );
          _stack->_data[_baseElement+index] = value;
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
    	// @todo A reserve might be correct here. Not sure
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
      assertion( _ioMode==IOMode::None );
      _ioMode = IOMode::MPISend;
      _ioTag  = tag;
      _ioRank = rank;
      assertion(false);
    }


    void finishSend() {
      assertion( _ioMode==IOMode::MPISend );
      _ioMode = IOMode::None;
      assertion(false);
    }

    /**
     * @see startSend()
     */
    void startReceive(int rank, int tag) {
      assertion( _ioMode==IOMode::None );
      _ioMode = IOMode::MPIReceive;
      _ioTag  = tag;
      _ioRank = rank;
      assertion(false);
    }

    void finishReceive() {
      assertion( _ioMode==IOMode::MPIReceive );
      _ioMode = IOMode::None;
      assertion(false);
    }
};



template <class T>
class peano4::stacks::STDVectorStackForUserDefinedData {
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
    enum class IOMode {
      None,
      MPISend,
	  MPIReceive
    };

    IOMode              _ioMode;
    int                 _ioRank;
    int                 _ioTag;

  public:
    /**
     * Constructor.
     *
     * @see EventStack::EventStack()
     */
    STDVectorStackForUserDefinedData():
      _data(),
	  _currentElement(0),
	  _ioMode(IOMode::None) {
    }

    ~STDVectorStackForUserDefinedData() {
    }

    /**
     * One is allowed to clone/copy a stack, but it has to be empty.
     * Usually only when we cut the domain into pieces.
     */
    STDVectorStackForUserDefinedData<T>( const STDVectorStackForUserDefinedData<T>& stack ):
	  _data(),
      _currentElement(0),
	  _ioMode(IOMode::None) {
      assertionMsg( stack._currentElement==0, "may not copy non-empty stack" );
    }


    /**
     * One is not allowed to copy a stack but it has to be empty.
     */
    STDVectorStackForUserDefinedData<T>&  operator=( const STDVectorStackForUserDefinedData<T>& stack ) {
      assertionEquals( stack._currentElement, 0 );
      assertionEquals( _currentElement, 0 );
      assertion( _data.empty() );
    }


    /**
     * Clone data into the current object on which clone() is called.
     */
    void  clone( const STDVectorStackForUserDefinedData<T>&  data ) {
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
        friend class peano4::stacks::STDVectorStackForUserDefinedData<T>;

        const int                           _size;
        const int                           _baseElement;
        peano4::stacks::STDVectorStackForUserDefinedData<T>*  _stack;

        /**
         * Constructor
         */
        PopBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStackForUserDefinedData<T>* stack):
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
        friend class peano4::stacks::STDVectorStackForUserDefinedData<T>;

        /**
         * Constructor
         */
        PushBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStackForUserDefinedData<T>* stack):
          _size(size),
		  _baseElement(base),
          _stack(stack) {
        }

        const int                           _size;
        const int                           _baseElement;
        peano4::stacks::STDVectorStackForUserDefinedData<T>*  _stack;
      public:
        int size() const {
          return _size;
        }

        void set(int index, const T& value) {
          assertion2( index>=0, index, _size );
          assertion2( index<_size, index, _size );
          _stack->_data[_baseElement+index] = value;
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
    	// @todo A reserve might be correct here. Not sure
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
      assertion( _ioMode==IOMode::None );
      _ioMode = IOMode::MPISend;
      _ioTag  = tag;
      _ioRank = rank;
      assertion(false);
    }


    void finishSend() {
      assertion( _ioMode==IOMode::MPISend );
      _ioMode = IOMode::None;
      assertion(false);
    }

    /**
     * @see startSend()
     */
    void startReceive(int rank, int tag) {
      assertion( _ioMode==IOMode::None );
      _ioMode = IOMode::MPIReceive;
      _ioTag  = tag;
      _ioRank = rank;
      assertion(false);
    }

    void finishReceive() {
      assertion( _ioMode==IOMode::MPIReceive );
      _ioMode = IOMode::None;
      assertion(false);
    }
};


#endif
