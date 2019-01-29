// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_STACKS_STDVECTOR_STACK_H_
#define _PEANO4_STACKS_STDVECTOR_STACK_H_


#include <vector>

#include "tarch/logging/Log.h"


namespace peano4 {
  namespace stacks {
    template <class T>
    class STDVectorStack;
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

  public:
    /**
     * Constructor.
     *
     * @see EventStack::EventStack()
     */
    STDVectorStack():
      _data(),
	  _currentElement(0) {
    }

    ~STDVectorStack() {
    }

    /**
     * One is allowed to clone/copy a stack, but it has to be empty.
     * Usually only when we cut the domain into pieces.
     */
    STDVectorStack<T>( const STDVectorStack<T>& stack ):
	  _data(),
      _currentElement(0) {
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
      private:
        /**
         * Parent is friend
         */
        friend class peano4::stacks::STDVectorStack<T>;

        const int                           _size;
        const int                           _baseElement;
        peano4::stacks::STDVectorStack<T>*  _stack;

        /**
         * The default constructor creates an empty stack view
         */
/*
        PopBlockVertexStackView():
          _currentElement(0),
          _size(0),
          _remainingSize(0),
          _stack(0) {
        }
*/

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
      private:
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
        PushBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStack<T>* stack):
          _size(size),
		  _baseElement(base),
          _stack(stack) {
        }
      public:
        int size() const {
          return _size;
        }

        void set(int index, const T& value) {
          assertion2( index>=0, index, _size );
          assertion2( index<_size, index, _size );
          _stack->_data[_baseElement+index] = value;
        }

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
      PopBlockVertexStackView result(numberOfVertices, _currentElement, this);

      _currentElement-=numberOfVertices;
      assertion( _currentElement>=0 );

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
    	//assertionMsg( false, "look up whether resize is the correct function" );
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
};


#endif
