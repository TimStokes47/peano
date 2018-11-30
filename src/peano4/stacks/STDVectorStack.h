// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_STACKS_STDVECTOR_STACK_H_
#define _PEANO4_STACKS_STDVECTOR_STACK_H_


#include <vector>


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

    /**
     * One is not allowed to clone a stack.
     */
    STDVectorStack<T>( const STDVectorStack<T>& stack ) = delete;

    /**
     * One is not allowed to clone a stack.
     */
    STDVectorStack<T>&  operator=( const STDVectorStack<T>& stack ) = delete;

  public:
    class PopBlockVertexStackView {
      private:
        /**
         * Parent is friend
         */
        friend class peano4::stacks::STDVectorStack<T>;

        int         _currentElement;

        /**
         * Should be const, but gcc had some issues with this.
         */
        int         _size;

        int         _remainingSize;

        peano4::stacks::STDVectorStack<T>* _stack;

        /**
         * The default constructor creates an empty stack view
         */
        PopBlockVertexStackView():
          _currentElement(0),
          _size(0),
          _remainingSize(0),
          _stack(0) {
        }

        /**
         * Constructor
         */
        PopBlockVertexStackView(int size, int currentElementBeforeViewIsOpened, peano4::stacks::STDVectorStack<T>* stack):
          _currentElement(currentElementBeforeViewIsOpened),
          _size(size),
          _remainingSize(size),
          _stack(stack) {
        }

      public:
        int getTotalViewSize() const {
          return _size;
        }

        int size() const {
          return _remainingSize;
        }

        bool isEmpty() const {
          return size()==0;
        }

        T pop() {
          assertion( _remainingSize>0 );
          _remainingSize--;
          _currentElement--;
          assertion( _currentElement>=0 );
          assertion( _stack!=0 );
          return _stack->_container[_currentElement];
        }

        PopBlockVertexStackView popBlockFromInputStack(int numberOfVertices) {
          PopBlockVertexStackView result(numberOfVertices, _currentElement, _stack);

          _remainingSize  -= numberOfVertices;
          _currentElement -= numberOfVertices;

          assertion( _remainingSize>=0 );
          assertion( _currentElement>=0 );

          return result;
        }

        std::string toString() const {
          std::ostringstream msg;
          msg << "(size=" << _size
              << ",currentElement=" << _currentElement
              << ",remaining-size=" << _remainingSize
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

        int         _currentElement;

        /**
         * Should be const, but gcc had some issues with this.
         */
        int         _size;

        int         _remainingSize;

        peano4::stacks::STDVectorStack<T>*  _stack;

        /**
         * The default constructor creates an empty stack view
         */
        PushBlockVertexStackView():
          _currentElement(0),
          _size(0),
          _remainingSize(0),
          _stack(0) {
        }

        /**
         * Constructor
         */
        PushBlockVertexStackView(int size, int currentElementBeforeViewIsOpened, peano4::stacks::STDVectorStack<T>* stack):
          _currentElement(currentElementBeforeViewIsOpened),
          _size(size),
          _remainingSize(size),
          _stack(stack) {
        }
      public:
        int getTotalViewSize() const {
          return _size;
        }

        int size() const {
          return _remainingSize;
        }

        bool isOpen() const {
          return size()!=0;
        }

        void push(const T& value) {
          assertion( _remainingSize>0 );
          _remainingSize--;
          assertion( _stack!=0 );
          _stack->_container[_currentElement] = value;
          _currentElement++;
        }

        PushBlockVertexStackView pushBlockOnOutputStack(int numberOfVertices) {
          PushBlockVertexStackView result(numberOfVertices, _currentElement, _stack);

          _remainingSize  -= numberOfVertices;
          _currentElement += numberOfVertices;

          assertion3( _remainingSize>=0, numberOfVertices, _remainingSize, _currentElement );

          return result;
        }


        std::string toString() const {
          std::ostringstream msg;
          msg << "(size=" << _size
              << ",currentElement=" << _currentElement
              << ",remaining-size=" << _remainingSize
              << ")";
          return msg.str();
        }
    };


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
     * One is not allowed to clone a stack.
     */
//    STDVectorStack( const STDVectorStack<T>&& stack ) = default;

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
    PopBlockVertexStackView&&  popBlock(int numberOfVertices) {
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
    PushBlockVertexStackView&&  pushBlock(int numberOfVertices) {
      PushBlockVertexStackView result(numberOfVertices, _currentElement, this);

      _currentElement+=numberOfVertices;

      if (static_cast<int>(_data.size())<_currentElement) {
    	assertionMsg( false, "look up whether resize is the correct function" );
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
