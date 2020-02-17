#include "GridTraversalEvent.h"

peano4::grid::GridTraversalEvent::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::GridTraversalEvent::PersistentRecords::PersistentRecords(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const std::bitset<TwoPowerD>& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom, const tarch::la::Vector<TwoPowerD,int>& vertexDataTo, const tarch::la::Vector<TwoTimesD,int>& faceDataFrom, const tarch::la::Vector<TwoTimesD,int>& faceDataTo, const int& cellData, const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData, const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData, const DataExchangeType& sendReceiveCellData, const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank, const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank, const int& sendReceiveCellDataRank, const tarch::la::Vector<Dimensions,int>& relativePositionToFather):
_x(x),
_h(h),
_isRefined(isRefined),
_vertexDataFrom(vertexDataFrom),
_vertexDataTo(vertexDataTo),
_faceDataFrom(faceDataFrom),
_faceDataTo(faceDataTo),
_cellData(cellData),
_sendReceiveVertexData(sendReceiveVertexData),
_sendReceiveFaceData(sendReceiveFaceData),
_sendReceiveCellData(sendReceiveCellData),
_sendReceiveVertexDataRank(sendReceiveVertexDataRank),
_sendReceiveFaceDataRank(sendReceiveFaceDataRank),
_sendReceiveCellDataRank(sendReceiveCellDataRank),
_relativePositionToFather(relativePositionToFather) {
   
}


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::PersistentRecords::getX() const  {
   return _x;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _x = (x);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::PersistentRecords::getH() const  {
   return _h;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _h = (h);
}



 std::bitset<TwoPowerD> peano4::grid::GridTraversalEvent::PersistentRecords::getIsRefined() const  {
   return _isRefined;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setIsRefined(const std::bitset<TwoPowerD>& isRefined)  {
   _isRefined = (isRefined);
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getVertexDataFrom() const  {
   return _vertexDataFrom;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setVertexDataFrom(const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom)  {
   _vertexDataFrom = (vertexDataFrom);
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getVertexDataTo() const  {
   return _vertexDataTo;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setVertexDataTo(const tarch::la::Vector<TwoPowerD,int>& vertexDataTo)  {
   _vertexDataTo = (vertexDataTo);
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getFaceDataFrom() const  {
   return _faceDataFrom;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setFaceDataFrom(const tarch::la::Vector<TwoTimesD,int>& faceDataFrom)  {
   _faceDataFrom = (faceDataFrom);
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getFaceDataTo() const  {
   return _faceDataTo;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setFaceDataTo(const tarch::la::Vector<TwoTimesD,int>& faceDataTo)  {
   _faceDataTo = (faceDataTo);
}



 int peano4::grid::GridTraversalEvent::PersistentRecords::getCellData() const  {
   return _cellData;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setCellData(const int& cellData)  {
   _cellData = cellData;
}



 tarch::la::Vector<TwoPowerD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEvent::PersistentRecords::getSendReceiveVertexData() const  {
   return _sendReceiveVertexData;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setSendReceiveVertexData(const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData)  {
   _sendReceiveVertexData = (sendReceiveVertexData);
}



 tarch::la::Vector<TwoTimesD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEvent::PersistentRecords::getSendReceiveFaceData() const  {
   return _sendReceiveFaceData;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setSendReceiveFaceData(const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData)  {
   _sendReceiveFaceData = (sendReceiveFaceData);
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEvent::PersistentRecords::getSendReceiveCellData() const  {
   return _sendReceiveCellData;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setSendReceiveCellData(const DataExchangeType& sendReceiveCellData)  {
   _sendReceiveCellData = sendReceiveCellData;
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getSendReceiveVertexDataRank() const  {
   return _sendReceiveVertexDataRank;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setSendReceiveVertexDataRank(const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank)  {
   _sendReceiveVertexDataRank = (sendReceiveVertexDataRank);
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getSendReceiveFaceDataRank() const  {
   return _sendReceiveFaceDataRank;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setSendReceiveFaceDataRank(const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank)  {
   _sendReceiveFaceDataRank = (sendReceiveFaceDataRank);
}



 int peano4::grid::GridTraversalEvent::PersistentRecords::getSendReceiveCellDataRank() const  {
   return _sendReceiveCellDataRank;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setSendReceiveCellDataRank(const int& sendReceiveCellDataRank)  {
   _sendReceiveCellDataRank = sendReceiveCellDataRank;
}



 tarch::la::Vector<Dimensions,int> peano4::grid::GridTraversalEvent::PersistentRecords::getRelativePositionToFather() const  {
   return _relativePositionToFather;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setRelativePositionToFather(const tarch::la::Vector<Dimensions,int>& relativePositionToFather)  {
   _relativePositionToFather = (relativePositionToFather);
}


peano4::grid::GridTraversalEvent::GridTraversalEvent() {
   
}


peano4::grid::GridTraversalEvent::GridTraversalEvent(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._x, persistentRecords._h, persistentRecords._isRefined, persistentRecords._vertexDataFrom, persistentRecords._vertexDataTo, persistentRecords._faceDataFrom, persistentRecords._faceDataTo, persistentRecords._cellData, persistentRecords._sendReceiveVertexData, persistentRecords._sendReceiveFaceData, persistentRecords._sendReceiveCellData, persistentRecords._sendReceiveVertexDataRank, persistentRecords._sendReceiveFaceDataRank, persistentRecords._sendReceiveCellDataRank, persistentRecords._relativePositionToFather) {
   
}


peano4::grid::GridTraversalEvent::GridTraversalEvent(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const std::bitset<TwoPowerD>& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom, const tarch::la::Vector<TwoPowerD,int>& vertexDataTo, const tarch::la::Vector<TwoTimesD,int>& faceDataFrom, const tarch::la::Vector<TwoTimesD,int>& faceDataTo, const int& cellData, const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData, const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData, const DataExchangeType& sendReceiveCellData, const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank, const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank, const int& sendReceiveCellDataRank, const tarch::la::Vector<Dimensions,int>& relativePositionToFather):
_persistentRecords(x, h, isRefined, vertexDataFrom, vertexDataTo, faceDataFrom, faceDataTo, cellData, sendReceiveVertexData, sendReceiveFaceData, sendReceiveCellData, sendReceiveVertexDataRank, sendReceiveFaceDataRank, sendReceiveCellDataRank, relativePositionToFather) {
   
}


peano4::grid::GridTraversalEvent::~GridTraversalEvent() { }


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::getX() const  {
   return _persistentRecords._x;
}



 void peano4::grid::GridTraversalEvent::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _persistentRecords._x = (x);
}



 double peano4::grid::GridTraversalEvent::getX(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._x[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setX(int elementIndex, const double& x)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._x[elementIndex]= x;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::getH() const  {
   return _persistentRecords._h;
}



 void peano4::grid::GridTraversalEvent::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _persistentRecords._h = (h);
}



 double peano4::grid::GridTraversalEvent::getH(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._h[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setH(int elementIndex, const double& h)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._h[elementIndex]= h;
   
}



 std::bitset<TwoPowerD> peano4::grid::GridTraversalEvent::getIsRefined() const  {
   return _persistentRecords._isRefined;
}



 void peano4::grid::GridTraversalEvent::setIsRefined(const std::bitset<TwoPowerD>& isRefined)  {
   _persistentRecords._isRefined = (isRefined);
}



 bool peano4::grid::GridTraversalEvent::getIsRefined(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   return _persistentRecords._isRefined[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setIsRefined(int elementIndex, const bool& isRefined)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._isRefined[elementIndex]= isRefined;
   
}



 void peano4::grid::GridTraversalEvent::flipIsRefined(int elementIndex)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._isRefined.flip(elementIndex);
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::getVertexDataFrom() const  {
   return _persistentRecords._vertexDataFrom;
}



 void peano4::grid::GridTraversalEvent::setVertexDataFrom(const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom)  {
   _persistentRecords._vertexDataFrom = (vertexDataFrom);
}



 int peano4::grid::GridTraversalEvent::getVertexDataFrom(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   return _persistentRecords._vertexDataFrom[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setVertexDataFrom(int elementIndex, const int& vertexDataFrom)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._vertexDataFrom[elementIndex]= vertexDataFrom;
   
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::getVertexDataTo() const  {
   return _persistentRecords._vertexDataTo;
}



 void peano4::grid::GridTraversalEvent::setVertexDataTo(const tarch::la::Vector<TwoPowerD,int>& vertexDataTo)  {
   _persistentRecords._vertexDataTo = (vertexDataTo);
}



 int peano4::grid::GridTraversalEvent::getVertexDataTo(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   return _persistentRecords._vertexDataTo[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setVertexDataTo(int elementIndex, const int& vertexDataTo)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._vertexDataTo[elementIndex]= vertexDataTo;
   
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::getFaceDataFrom() const  {
   return _persistentRecords._faceDataFrom;
}



 void peano4::grid::GridTraversalEvent::setFaceDataFrom(const tarch::la::Vector<TwoTimesD,int>& faceDataFrom)  {
   _persistentRecords._faceDataFrom = (faceDataFrom);
}



 int peano4::grid::GridTraversalEvent::getFaceDataFrom(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   return _persistentRecords._faceDataFrom[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setFaceDataFrom(int elementIndex, const int& faceDataFrom)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   _persistentRecords._faceDataFrom[elementIndex]= faceDataFrom;
   
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::getFaceDataTo() const  {
   return _persistentRecords._faceDataTo;
}



 void peano4::grid::GridTraversalEvent::setFaceDataTo(const tarch::la::Vector<TwoTimesD,int>& faceDataTo)  {
   _persistentRecords._faceDataTo = (faceDataTo);
}



 int peano4::grid::GridTraversalEvent::getFaceDataTo(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   return _persistentRecords._faceDataTo[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setFaceDataTo(int elementIndex, const int& faceDataTo)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   _persistentRecords._faceDataTo[elementIndex]= faceDataTo;
   
}



 int peano4::grid::GridTraversalEvent::getCellData() const  {
   return _persistentRecords._cellData;
}



 void peano4::grid::GridTraversalEvent::setCellData(const int& cellData)  {
   _persistentRecords._cellData = cellData;
}



 tarch::la::Vector<TwoPowerD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEvent::getSendReceiveVertexData() const  {
   return _persistentRecords._sendReceiveVertexData;
}



 void peano4::grid::GridTraversalEvent::setSendReceiveVertexData(const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData)  {
   _persistentRecords._sendReceiveVertexData = (sendReceiveVertexData);
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEvent::getSendReceiveVertexData(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   return _persistentRecords._sendReceiveVertexData[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setSendReceiveVertexData(int elementIndex, const DataExchangeType& sendReceiveVertexData)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._sendReceiveVertexData[elementIndex]= sendReceiveVertexData;
   
}



 tarch::la::Vector<TwoTimesD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEvent::getSendReceiveFaceData() const  {
   return _persistentRecords._sendReceiveFaceData;
}



 void peano4::grid::GridTraversalEvent::setSendReceiveFaceData(const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData)  {
   _persistentRecords._sendReceiveFaceData = (sendReceiveFaceData);
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEvent::getSendReceiveFaceData(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   return _persistentRecords._sendReceiveFaceData[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setSendReceiveFaceData(int elementIndex, const DataExchangeType& sendReceiveFaceData)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   _persistentRecords._sendReceiveFaceData[elementIndex]= sendReceiveFaceData;
   
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEvent::getSendReceiveCellData() const  {
   return _persistentRecords._sendReceiveCellData;
}



 void peano4::grid::GridTraversalEvent::setSendReceiveCellData(const DataExchangeType& sendReceiveCellData)  {
   _persistentRecords._sendReceiveCellData = sendReceiveCellData;
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::getSendReceiveVertexDataRank() const  {
   return _persistentRecords._sendReceiveVertexDataRank;
}



 void peano4::grid::GridTraversalEvent::setSendReceiveVertexDataRank(const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank)  {
   _persistentRecords._sendReceiveVertexDataRank = (sendReceiveVertexDataRank);
}



 int peano4::grid::GridTraversalEvent::getSendReceiveVertexDataRank(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   return _persistentRecords._sendReceiveVertexDataRank[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setSendReceiveVertexDataRank(int elementIndex, const int& sendReceiveVertexDataRank)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._sendReceiveVertexDataRank[elementIndex]= sendReceiveVertexDataRank;
   
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::getSendReceiveFaceDataRank() const  {
   return _persistentRecords._sendReceiveFaceDataRank;
}



 void peano4::grid::GridTraversalEvent::setSendReceiveFaceDataRank(const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank)  {
   _persistentRecords._sendReceiveFaceDataRank = (sendReceiveFaceDataRank);
}



 int peano4::grid::GridTraversalEvent::getSendReceiveFaceDataRank(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   return _persistentRecords._sendReceiveFaceDataRank[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setSendReceiveFaceDataRank(int elementIndex, const int& sendReceiveFaceDataRank)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   _persistentRecords._sendReceiveFaceDataRank[elementIndex]= sendReceiveFaceDataRank;
   
}



 int peano4::grid::GridTraversalEvent::getSendReceiveCellDataRank() const  {
   return _persistentRecords._sendReceiveCellDataRank;
}



 void peano4::grid::GridTraversalEvent::setSendReceiveCellDataRank(const int& sendReceiveCellDataRank)  {
   _persistentRecords._sendReceiveCellDataRank = sendReceiveCellDataRank;
}



 tarch::la::Vector<Dimensions,int> peano4::grid::GridTraversalEvent::getRelativePositionToFather() const  {
   return _persistentRecords._relativePositionToFather;
}



 void peano4::grid::GridTraversalEvent::setRelativePositionToFather(const tarch::la::Vector<Dimensions,int>& relativePositionToFather)  {
   _persistentRecords._relativePositionToFather = (relativePositionToFather);
}



 int peano4::grid::GridTraversalEvent::getRelativePositionToFather(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._relativePositionToFather[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setRelativePositionToFather(int elementIndex, const int& relativePositionToFather)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._relativePositionToFather[elementIndex]= relativePositionToFather;
   
}


std::string peano4::grid::GridTraversalEvent::toString(const DataExchangeType& param) {
   switch (param) {
      case ExchangeHorizontally: return "ExchangeHorizontally";
      case StreamInOut: return "StreamInOut";
      case ExchangeVerticallyWithMaster: return "ExchangeVerticallyWithMaster";
      case ExchangeVerticallyWithWorker: return "ExchangeVerticallyWithWorker";
      case None: return "None";
   }
   return "undefined";
}

std::string peano4::grid::GridTraversalEvent::getDataExchangeTypeMapping() {
   return "DataExchangeType(ExchangeHorizontally=0,StreamInOut=1,ExchangeVerticallyWithMaster=2,ExchangeVerticallyWithWorker=3,None=4)";
}


std::string peano4::grid::GridTraversalEvent::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::GridTraversalEvent::toString (std::ostream& out) const {
   out << "("; 
   out << "x:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(Dimensions-1) << "]";
   out << ",";
   out << "h:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getH(i) << ",";
   }
   out << getH(Dimensions-1) << "]";
   out << ",";
   out << "isRefined:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getIsRefined(i) << ",";
   }
   out << getIsRefined(TwoPowerD-1) << "]";
   out << ",";
   out << "vertexDataFrom:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getVertexDataFrom(i) << ",";
   }
   out << getVertexDataFrom(TwoPowerD-1) << "]";
   out << ",";
   out << "vertexDataTo:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getVertexDataTo(i) << ",";
   }
   out << getVertexDataTo(TwoPowerD-1) << "]";
   out << ",";
   out << "faceDataFrom:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getFaceDataFrom(i) << ",";
   }
   out << getFaceDataFrom(TwoTimesD-1) << "]";
   out << ",";
   out << "faceDataTo:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getFaceDataTo(i) << ",";
   }
   out << getFaceDataTo(TwoTimesD-1) << "]";
   out << ",";
   out << "cellData:" << getCellData();
   out << ",";
   out << "sendReceiveVertexData:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getSendReceiveVertexData(i) << ",";
   }
   out << getSendReceiveVertexData(TwoPowerD-1) << "]";
   out << ",";
   out << "sendReceiveFaceData:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getSendReceiveFaceData(i) << ",";
   }
   out << getSendReceiveFaceData(TwoTimesD-1) << "]";
   out << ",";
   out << "sendReceiveCellData:" << toString(getSendReceiveCellData());
   out << ",";
   out << "sendReceiveVertexDataRank:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getSendReceiveVertexDataRank(i) << ",";
   }
   out << getSendReceiveVertexDataRank(TwoPowerD-1) << "]";
   out << ",";
   out << "sendReceiveFaceDataRank:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getSendReceiveFaceDataRank(i) << ",";
   }
   out << getSendReceiveFaceDataRank(TwoTimesD-1) << "]";
   out << ",";
   out << "sendReceiveCellDataRank:" << getSendReceiveCellDataRank();
   out << ",";
   out << "relativePositionToFather:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getRelativePositionToFather(i) << ",";
   }
   out << getRelativePositionToFather(Dimensions-1) << "]";
   out <<  ")";
}


peano4::grid::GridTraversalEvent::PersistentRecords peano4::grid::GridTraversalEvent::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::GridTraversalEventPacked peano4::grid::GridTraversalEvent::convert() const{
   return GridTraversalEventPacked(
      getX(),
      getH(),
      getIsRefined(),
      getVertexDataFrom(),
      getVertexDataTo(),
      getFaceDataFrom(),
      getFaceDataTo(),
      getCellData(),
      getSendReceiveVertexData(),
      getSendReceiveFaceData(),
      getSendReceiveCellData(),
      getSendReceiveVertexDataRank(),
      getSendReceiveFaceDataRank(),
      getSendReceiveCellDataRank(),
      getRelativePositionToFather()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridTraversalEvent::_log( "peano4::grid::GridTraversalEvent" );
   
   MPI_Datatype peano4::grid::GridTraversalEvent::Datatype = 0;
   MPI_Datatype peano4::grid::GridTraversalEvent::FullDatatype = 0;
   
   
   void peano4::grid::GridTraversalEvent::initDatatype() {
      {
         GridTraversalEvent dummyGridTraversalEvent[16];
         
         #ifdef MPI2
         const int Attributes = 15;
         #else
         const int Attributes = 15+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_CXX_BOOL		 //isRefined
            , MPI_INT		 //vertexDataFrom
            , MPI_INT		 //vertexDataTo
            , MPI_INT		 //faceDataFrom
            , MPI_INT		 //faceDataTo
            , MPI_INT		 //cellData
            , MPI_INT		 //sendReceiveVertexData
            , MPI_INT		 //sendReceiveFaceData
            , MPI_INT		 //sendReceiveCellData
            , MPI_INT		 //sendReceiveVertexDataRank
            , MPI_INT		 //sendReceiveFaceDataRank
            , MPI_INT		 //sendReceiveCellDataRank
            , MPI_INT		 //relativePositionToFather
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , TwoPowerD		 //isRefined
            , TwoPowerD		 //vertexDataFrom
            , TwoPowerD		 //vertexDataTo
            , TwoTimesD		 //faceDataFrom
            , TwoTimesD		 //faceDataTo
            , 1		 //cellData
            , TwoPowerD		 //sendReceiveVertexData
            , TwoTimesD		 //sendReceiveFaceData
            , 1		 //sendReceiveCellData
            , TwoPowerD		 //sendReceiveVertexDataRank
            , TwoTimesD		 //sendReceiveFaceDataRank
            , 1		 //sendReceiveCellDataRank
            , Dimensions		 //relativePositionToFather
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataFrom[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataFrom[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataTo[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataTo[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataFrom[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataFrom[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataTo[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataTo[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellData))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellData))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEvent::Datatype );
         MPI_Type_commit( &GridTraversalEvent::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEvent::Datatype);
         MPI_Type_commit( &GridTraversalEvent::Datatype );
         #endif
         
      }
      {
         GridTraversalEvent dummyGridTraversalEvent[16];
         
         #ifdef MPI2
         const int Attributes = 15;
         #else
         const int Attributes = 15+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_CXX_BOOL		 //isRefined
            , MPI_INT		 //vertexDataFrom
            , MPI_INT		 //vertexDataTo
            , MPI_INT		 //faceDataFrom
            , MPI_INT		 //faceDataTo
            , MPI_INT		 //cellData
            , MPI_INT		 //sendReceiveVertexData
            , MPI_INT		 //sendReceiveFaceData
            , MPI_INT		 //sendReceiveCellData
            , MPI_INT		 //sendReceiveVertexDataRank
            , MPI_INT		 //sendReceiveFaceDataRank
            , MPI_INT		 //sendReceiveCellDataRank
            , MPI_INT		 //relativePositionToFather
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , TwoPowerD		 //isRefined
            , TwoPowerD		 //vertexDataFrom
            , TwoPowerD		 //vertexDataTo
            , TwoTimesD		 //faceDataFrom
            , TwoTimesD		 //faceDataTo
            , 1		 //cellData
            , TwoPowerD		 //sendReceiveVertexData
            , TwoTimesD		 //sendReceiveFaceData
            , 1		 //sendReceiveCellData
            , TwoPowerD		 //sendReceiveVertexDataRank
            , TwoTimesD		 //sendReceiveFaceDataRank
            , 1		 //sendReceiveCellDataRank
            , Dimensions		 //relativePositionToFather
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataFrom[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataFrom[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataTo[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexDataTo[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataFrom[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataFrom[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataTo[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceDataTo[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellData))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellData))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEvent::FullDatatype );
         MPI_Type_commit( &GridTraversalEvent::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEvent::FullDatatype);
         MPI_Type_commit( &GridTraversalEvent::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::grid::GridTraversalEvent::shutdownDatatype() {
      MPI_Type_free( &GridTraversalEvent::Datatype );
      MPI_Type_free( &GridTraversalEvent::FullDatatype );
      
   }
   
   void peano4::grid::GridTraversalEvent::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridTraversalEvent " 
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result); 
         _log.error( "send(int)",msg.str() ); 
       } 
    } 
    break; 
   case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    {
      MPI_Request* sendRequestHandle = new MPI_Request(); 
      int          flag = 0; 
       int          result; 
       clock_t      timeOutWarning   = -1; 
       clock_t      timeOutShutdown  = -1; 
       bool         triggeredTimeoutWarning = false;  
       result = MPI_Isend(  
         this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination,  
         tag, tarch::mpi::Rank::getInstance().getCommunicator(), 
         sendRequestHandle  
       ); 
       if  (result!=MPI_SUCCESS) {  
         std::ostringstream msg;  
         msg << "was not able to send message peano4::grid::GridTraversalEvent "  
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result);  
         _log.error( "send(int)",msg.str() );  
       }  
       result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
       while (!flag) { 
         if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
         result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
           std::ostringstream msg; 
           msg << "testing for finished send task for peano4::grid::GridTraversalEvent " 
               << toString() 
               << " sent to node " << destination 
               << " failed: " << tarch::mpi::MPIReturnValueToString(result); 
           _log.error("send(int)", msg.str() ); 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
           (clock()>timeOutWarning) && 
           (!triggeredTimeoutWarning) 
         ) { 
           tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
             "peano4::grid::GridTraversalEvent", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridTraversalEvent", 
             "send(int)", destination,tag,1 
           ); 
         } 
 	       tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
       } 
       delete sendRequestHandle; 
     }  
     break; 
   case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    assertionMsg(false,"should not be called"); 
    break; 
} 
 // ============================= 
// end injected snippet/aspect 
// ============================= 

      
   }
   
   
   
   void peano4::grid::GridTraversalEvent::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
MPI_Status status; 
switch (mode) { 
  case ExchangeMode::Blocking: 
    { 
      const int   result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEvent from rank " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    } 
    break; 
  case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    { 
      int          flag = 0; 
      int          result; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      MPI_Request* sendRequestHandle = new MPI_Request(); 
       result = MPI_Irecv( 
        this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, 
        tarch::mpi::Rank::getInstance().getCommunicator(), sendRequestHandle 
      ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEvent from rank " 
             << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
      result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEvent failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      delete sendRequestHandle; 
    }    break; 
  case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    {
      int flag; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      int result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
       if (result!=MPI_SUCCESS) { 
        std::ostringstream msg; 
        msg << "testing for finished receive task for peano4::grid::GridTraversalEvent failed: " 
            << tarch::mpi::MPIReturnValueToString(result); 
        _log.error("receive(int)", msg.str() ); 
      } 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEvent failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEvent from rank " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    }
    break; 
  } 
// =========================== 
// end injected snippet/aspect 
// =========================== 

      
     _senderDestinationRank = source==MPI_ANY_SOURCE ? status.MPI_SOURCE : source;
   }
   
   
   
   bool peano4::grid::GridTraversalEvent::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Status status;
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, &status
      );
      if (flag) {
         int  messageCounter;
         if (exchangeOnlyAttributesMarkedWithParallelise) {
            MPI_Get_count(&status, Datatype, &messageCounter);
         }
         else {
            MPI_Get_count(&status, FullDatatype, &messageCounter);
         }
         return messageCounter > 0;
      }
      else return false;
      
   }
   
   int peano4::grid::GridTraversalEvent::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::grid::GridTraversalEventPacked::PersistentRecords::PersistentRecords() {
   if ((TwoPowerD+48 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((TwoPowerD+48 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((54 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((54 < (8 * sizeof(long int))));
   if ((18 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((18 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::PersistentRecords::PersistentRecords(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const std::bitset<TwoPowerD>& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom, const tarch::la::Vector<TwoPowerD,int>& vertexDataTo, const tarch::la::Vector<TwoTimesD,int>& faceDataFrom, const tarch::la::Vector<TwoTimesD,int>& faceDataTo, const int& cellData, const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData, const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData, const DataExchangeType& sendReceiveCellData, const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank, const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank, const int& sendReceiveCellDataRank, const tarch::la::Vector<Dimensions,int>& relativePositionToFather):
_x(x),
_h(h),
_sendReceiveVertexDataRank(sendReceiveVertexDataRank),
_sendReceiveFaceDataRank(sendReceiveFaceDataRank),
_sendReceiveCellDataRank(sendReceiveCellDataRank),
_relativePositionToFather(relativePositionToFather) {
   setIsRefined(isRefined);
   setVertexDataFrom(vertexDataFrom);
   setVertexDataTo(vertexDataTo);
   setFaceDataFrom(faceDataFrom);
   setFaceDataTo(faceDataTo);
   setCellData(cellData);
   setSendReceiveVertexData(sendReceiveVertexData);
   setSendReceiveFaceData(sendReceiveFaceData);
   setSendReceiveCellData(sendReceiveCellData);
   if ((TwoPowerD+48 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((TwoPowerD+48 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((54 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((54 < (8 * sizeof(long int))));
   if ((18 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((18 < (8 * sizeof(long int))));
   
}


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::PersistentRecords::getX() const  {
   return _x;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _x = (x);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::PersistentRecords::getH() const  {
   return _h;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _h = (h);
}



 std::bitset<TwoPowerD> peano4::grid::GridTraversalEventPacked::PersistentRecords::getIsRefined() const  {
   long int mask = (long int) (1 << (TwoPowerD)) - 1 ;
   mask = static_cast<long int>(mask << (0));
   long int tmp = static_cast<long int>(_packedRecords0 & mask);
   tmp = static_cast<long int>(tmp >> (0));
   std::bitset<TwoPowerD> result = tmp;
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setIsRefined(const std::bitset<TwoPowerD>& isRefined)  {
   long int mask = (long int) (1 << (TwoPowerD)) - 1 ;
   mask = static_cast<long int>(mask << (0));
   _packedRecords0 = static_cast<long int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<long int>(_packedRecords0 | isRefined.to_ulong() << (0));
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getVertexDataFrom() const  {
   long int mask = 15;
   mask = mask << (0);
   tarch::la::Vector<TwoPowerD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _packedRecords1 & mask;
      tmp = tmp >> (0 + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= -4 && result[i] <= 9));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setVertexDataFrom(const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((vertexDataFrom[i] >= -4 && vertexDataFrom[i] <= 9));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 15;
      mask = mask << (0 + i * 4 );
      _packedRecords1 = _packedRecords1 & ~mask;
      long int tmp = static_cast<long int>(vertexDataFrom[i]);
      _packedRecords1 = _packedRecords1 | ( tmp - -4) << (0 + i * 4);
   }
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getVertexDataTo() const  {
   long int mask = 15;
   mask = mask << (0);
   tarch::la::Vector<TwoPowerD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _packedRecords2 & mask;
      tmp = tmp >> (0 + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= -4 && result[i] <= 9));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setVertexDataTo(const tarch::la::Vector<TwoPowerD,int>& vertexDataTo)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((vertexDataTo[i] >= -4 && vertexDataTo[i] <= 9));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 15;
      mask = mask << (0 + i * 4 );
      _packedRecords2 = _packedRecords2 & ~mask;
      long int tmp = static_cast<long int>(vertexDataTo[i]);
      _packedRecords2 = _packedRecords2 | ( tmp - -4) << (0 + i * 4);
   }
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getFaceDataFrom() const  {
   long int mask = 15;
   mask = mask << (TwoPowerD);
   tarch::la::Vector<TwoTimesD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _packedRecords0 & mask;
      tmp = tmp >> (TwoPowerD + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= -4 && result[i] <= 7));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setFaceDataFrom(const tarch::la::Vector<TwoTimesD,int>& faceDataFrom)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((faceDataFrom[i] >= -4 && faceDataFrom[i] <= 7));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 15;
      mask = mask << (TwoPowerD + i * 4 );
      _packedRecords0 = _packedRecords0 & ~mask;
      long int tmp = static_cast<long int>(faceDataFrom[i]);
      _packedRecords0 = _packedRecords0 | ( tmp - -4) << (TwoPowerD + i * 4);
   }
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getFaceDataTo() const  {
   long int mask = 15;
   mask = mask << (TwoPowerD + 24);
   tarch::la::Vector<TwoTimesD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _packedRecords0 & mask;
      tmp = tmp >> (TwoPowerD + 24 + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= -4 && result[i] <= 7));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setFaceDataTo(const tarch::la::Vector<TwoTimesD,int>& faceDataTo)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((faceDataTo[i] >= -4 && faceDataTo[i] <= 7));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 15;
      mask = mask << (TwoPowerD + 24 + i * 4 );
      _packedRecords0 = _packedRecords0 & ~mask;
      long int tmp = static_cast<long int>(faceDataTo[i]);
      _packedRecords0 = _packedRecords0 | ( tmp - -4) << (TwoPowerD + 24 + i * 4);
   }
}



 int peano4::grid::GridTraversalEventPacked::PersistentRecords::getCellData() const  {
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (0));
   long int tmp = static_cast<long int>(_packedRecords3 & mask);
   tmp = static_cast<long int>(tmp >> (0));
   tmp = tmp + -4;
   assertion(( tmp >= -4 &&  tmp <= 1));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setCellData(const int& cellData)  {
   assertion((cellData >= -4 && cellData <= 1));
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (0));
   _packedRecords3 = static_cast<long int>(_packedRecords3 & ~mask);
   _packedRecords3 = static_cast<long int>(_packedRecords3 | (static_cast<long int>(cellData) - -4) << (0));
}



 tarch::la::Vector<TwoPowerD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEventPacked::PersistentRecords::getSendReceiveVertexData() const  {
   long int mask = 7;
   mask = mask << (3);
   tarch::la::Vector<TwoPowerD,DataExchangeType> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _packedRecords3 & mask;
      tmp = tmp >> (3 + i * 3);
      result[i] = (DataExchangeType) tmp;
      mask = mask << 3;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= 0 && result[i] <= 4));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setSendReceiveVertexData(const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((sendReceiveVertexData[i] >= 0 && sendReceiveVertexData[i] <= 4));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 7;
      mask = mask << (3 + i * 3 );
      _packedRecords3 = _packedRecords3 & ~mask;
      long int tmp = static_cast<long int>(sendReceiveVertexData[i]);
      _packedRecords3 = _packedRecords3 | ( tmp << (3 + i * 3));
   }
}



 tarch::la::Vector<TwoTimesD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEventPacked::PersistentRecords::getSendReceiveFaceData() const  {
   long int mask = 7;
   mask = mask << (0);
   tarch::la::Vector<TwoTimesD,DataExchangeType> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _packedRecords4 & mask;
      tmp = tmp >> (0 + i * 3);
      result[i] = (DataExchangeType) tmp;
      mask = mask << 3;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= 0 && result[i] <= 4));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setSendReceiveFaceData(const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((sendReceiveFaceData[i] >= 0 && sendReceiveFaceData[i] <= 4));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 7;
      mask = mask << (0 + i * 3 );
      _packedRecords4 = _packedRecords4 & ~mask;
      long int tmp = static_cast<long int>(sendReceiveFaceData[i]);
      _packedRecords4 = _packedRecords4 | ( tmp << (0 + i * 3));
   }
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEventPacked::PersistentRecords::getSendReceiveCellData() const  {
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (51));
   long int tmp = static_cast<long int>(_packedRecords3 & mask);
   tmp = static_cast<long int>(tmp >> (51));
   assertion(( tmp >= 0 &&  tmp <= 4));
   return (DataExchangeType) tmp;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setSendReceiveCellData(const DataExchangeType& sendReceiveCellData)  {
   assertion((sendReceiveCellData >= 0 && sendReceiveCellData <= 4));
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (51));
   _packedRecords3 = static_cast<long int>(_packedRecords3 & ~mask);
   _packedRecords3 = static_cast<long int>(_packedRecords3 | static_cast<long int>(sendReceiveCellData) << (51));
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getSendReceiveVertexDataRank() const  {
   return _sendReceiveVertexDataRank;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setSendReceiveVertexDataRank(const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank)  {
   _sendReceiveVertexDataRank = (sendReceiveVertexDataRank);
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getSendReceiveFaceDataRank() const  {
   return _sendReceiveFaceDataRank;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setSendReceiveFaceDataRank(const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank)  {
   _sendReceiveFaceDataRank = (sendReceiveFaceDataRank);
}



 int peano4::grid::GridTraversalEventPacked::PersistentRecords::getSendReceiveCellDataRank() const  {
   return _sendReceiveCellDataRank;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setSendReceiveCellDataRank(const int& sendReceiveCellDataRank)  {
   _sendReceiveCellDataRank = sendReceiveCellDataRank;
}



 tarch::la::Vector<Dimensions,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getRelativePositionToFather() const  {
   return _relativePositionToFather;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setRelativePositionToFather(const tarch::la::Vector<Dimensions,int>& relativePositionToFather)  {
   _relativePositionToFather = (relativePositionToFather);
}


peano4::grid::GridTraversalEventPacked::GridTraversalEventPacked() {
   if ((TwoPowerD+48 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((TwoPowerD+48 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((54 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((54 < (8 * sizeof(long int))));
   if ((18 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((18 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::GridTraversalEventPacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._x, persistentRecords._h, persistentRecords.getIsRefined(), persistentRecords.getVertexDataFrom(), persistentRecords.getVertexDataTo(), persistentRecords.getFaceDataFrom(), persistentRecords.getFaceDataTo(), persistentRecords.getCellData(), persistentRecords.getSendReceiveVertexData(), persistentRecords.getSendReceiveFaceData(), persistentRecords.getSendReceiveCellData(), persistentRecords._sendReceiveVertexDataRank, persistentRecords._sendReceiveFaceDataRank, persistentRecords._sendReceiveCellDataRank, persistentRecords._relativePositionToFather) {
   if ((TwoPowerD+48 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((TwoPowerD+48 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((54 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((54 < (8 * sizeof(long int))));
   if ((18 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((18 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::GridTraversalEventPacked(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const std::bitset<TwoPowerD>& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom, const tarch::la::Vector<TwoPowerD,int>& vertexDataTo, const tarch::la::Vector<TwoTimesD,int>& faceDataFrom, const tarch::la::Vector<TwoTimesD,int>& faceDataTo, const int& cellData, const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData, const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData, const DataExchangeType& sendReceiveCellData, const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank, const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank, const int& sendReceiveCellDataRank, const tarch::la::Vector<Dimensions,int>& relativePositionToFather):
_persistentRecords(x, h, isRefined, vertexDataFrom, vertexDataTo, faceDataFrom, faceDataTo, cellData, sendReceiveVertexData, sendReceiveFaceData, sendReceiveCellData, sendReceiveVertexDataRank, sendReceiveFaceDataRank, sendReceiveCellDataRank, relativePositionToFather) {
   if ((TwoPowerD+48 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((TwoPowerD+48 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   if ((54 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((54 < (8 * sizeof(long int))));
   if ((18 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((18 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::~GridTraversalEventPacked() { }


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::getX() const  {
   return _persistentRecords._x;
}



 void peano4::grid::GridTraversalEventPacked::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _persistentRecords._x = (x);
}



 double peano4::grid::GridTraversalEventPacked::getX(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._x[elementIndex];
   
}



 void peano4::grid::GridTraversalEventPacked::setX(int elementIndex, const double& x)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._x[elementIndex]= x;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::getH() const  {
   return _persistentRecords._h;
}



 void peano4::grid::GridTraversalEventPacked::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _persistentRecords._h = (h);
}



 double peano4::grid::GridTraversalEventPacked::getH(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._h[elementIndex];
   
}



 void peano4::grid::GridTraversalEventPacked::setH(int elementIndex, const double& h)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._h[elementIndex]= h;
   
}



 std::bitset<TwoPowerD> peano4::grid::GridTraversalEventPacked::getIsRefined() const  {
   long int mask = (long int) (1 << (TwoPowerD)) - 1 ;
   mask = static_cast<long int>(mask << (0));
   long int tmp = static_cast<long int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<long int>(tmp >> (0));
   std::bitset<TwoPowerD> result = tmp;
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setIsRefined(const std::bitset<TwoPowerD>& isRefined)  {
   long int mask = (long int) (1 << (TwoPowerD)) - 1 ;
   mask = static_cast<long int>(mask << (0));
   _persistentRecords._packedRecords0 = static_cast<long int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<long int>(_persistentRecords._packedRecords0 | isRefined.to_ulong() << (0));
}



 bool peano4::grid::GridTraversalEventPacked::getIsRefined(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   int mask = 1 << (0);
   mask = mask << elementIndex;
   return (_persistentRecords._packedRecords0& mask);
}



 void peano4::grid::GridTraversalEventPacked::setIsRefined(int elementIndex, const bool& isRefined)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   assertion(!isRefined || isRefined==1);
   int shift        = 0 + elementIndex; 
   long int mask         = 1     << (shift);
   long int shiftedValue = isRefined << (shift);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 |  shiftedValue;
}



 void peano4::grid::GridTraversalEventPacked::flipIsRefined(int elementIndex)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   long int mask = 1 << (0);
   mask = mask << elementIndex;
   _persistentRecords._packedRecords0^= mask;
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::getVertexDataFrom() const  {
   long int mask = 15;
   mask = mask << (0);
   tarch::la::Vector<TwoPowerD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _persistentRecords._packedRecords1 & mask;
      tmp = tmp >> (0 + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= -4 && result[i] <= 9));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setVertexDataFrom(const tarch::la::Vector<TwoPowerD,int>& vertexDataFrom)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((vertexDataFrom[i] >= -4 && vertexDataFrom[i] <= 9));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 15;
      mask = mask << (0 + i * 4 );
      _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 & ~mask;
      long int tmp = static_cast<long int>(vertexDataFrom[i]);
      _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 | ( tmp - -4) << (0 + i * 4);
   }
}



 int peano4::grid::GridTraversalEventPacked::getVertexDataFrom(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   long int mask = 15;
   mask = mask << (0 + elementIndex * 4);
   long int tmp = _persistentRecords._packedRecords1 & mask;
   tmp = tmp >> (0 + elementIndex * 4);
   tmp = tmp + -4;
   assertion((tmp >= -4) && (tmp <= 9));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setVertexDataFrom(int elementIndex, const int& vertexDataFrom)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   assertion((vertexDataFrom >= -4 && vertexDataFrom <= 9));
   long int mask = 15;
   mask = mask << (0 + elementIndex * 4);
   _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 & ~mask;
   long int tmp = static_cast<long int>(vertexDataFrom);
   _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 | ( tmp - -4) << (0 + elementIndex * 4);
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::getVertexDataTo() const  {
   long int mask = 15;
   mask = mask << (0);
   tarch::la::Vector<TwoPowerD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _persistentRecords._packedRecords2 & mask;
      tmp = tmp >> (0 + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= -4 && result[i] <= 9));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setVertexDataTo(const tarch::la::Vector<TwoPowerD,int>& vertexDataTo)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((vertexDataTo[i] >= -4 && vertexDataTo[i] <= 9));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 15;
      mask = mask << (0 + i * 4 );
      _persistentRecords._packedRecords2 = _persistentRecords._packedRecords2 & ~mask;
      long int tmp = static_cast<long int>(vertexDataTo[i]);
      _persistentRecords._packedRecords2 = _persistentRecords._packedRecords2 | ( tmp - -4) << (0 + i * 4);
   }
}



 int peano4::grid::GridTraversalEventPacked::getVertexDataTo(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   long int mask = 15;
   mask = mask << (0 + elementIndex * 4);
   long int tmp = _persistentRecords._packedRecords2 & mask;
   tmp = tmp >> (0 + elementIndex * 4);
   tmp = tmp + -4;
   assertion((tmp >= -4) && (tmp <= 9));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setVertexDataTo(int elementIndex, const int& vertexDataTo)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   assertion((vertexDataTo >= -4 && vertexDataTo <= 9));
   long int mask = 15;
   mask = mask << (0 + elementIndex * 4);
   _persistentRecords._packedRecords2 = _persistentRecords._packedRecords2 & ~mask;
   long int tmp = static_cast<long int>(vertexDataTo);
   _persistentRecords._packedRecords2 = _persistentRecords._packedRecords2 | ( tmp - -4) << (0 + elementIndex * 4);
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::getFaceDataFrom() const  {
   long int mask = 15;
   mask = mask << (TwoPowerD);
   tarch::la::Vector<TwoTimesD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _persistentRecords._packedRecords0 & mask;
      tmp = tmp >> (TwoPowerD + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= -4 && result[i] <= 7));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setFaceDataFrom(const tarch::la::Vector<TwoTimesD,int>& faceDataFrom)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((faceDataFrom[i] >= -4 && faceDataFrom[i] <= 7));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 15;
      mask = mask << (TwoPowerD + i * 4 );
      _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
      long int tmp = static_cast<long int>(faceDataFrom[i]);
      _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 | ( tmp - -4) << (TwoPowerD + i * 4);
   }
}



 int peano4::grid::GridTraversalEventPacked::getFaceDataFrom(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   long int mask = 15;
   mask = mask << (TwoPowerD + elementIndex * 4);
   long int tmp = _persistentRecords._packedRecords0 & mask;
   tmp = tmp >> (TwoPowerD + elementIndex * 4);
   tmp = tmp + -4;
   assertion((tmp >= -4) && (tmp <= 7));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setFaceDataFrom(int elementIndex, const int& faceDataFrom)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   assertion((faceDataFrom >= -4 && faceDataFrom <= 7));
   long int mask = 15;
   mask = mask << (TwoPowerD + elementIndex * 4);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
   long int tmp = static_cast<long int>(faceDataFrom);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 | ( tmp - -4) << (TwoPowerD + elementIndex * 4);
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::getFaceDataTo() const  {
   long int mask = 15;
   mask = mask << (TwoPowerD + 24);
   tarch::la::Vector<TwoTimesD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _persistentRecords._packedRecords0 & mask;
      tmp = tmp >> (TwoPowerD + 24 + i * 4);
      tmp = tmp + -4;
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= -4 && result[i] <= 7));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setFaceDataTo(const tarch::la::Vector<TwoTimesD,int>& faceDataTo)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((faceDataTo[i] >= -4 && faceDataTo[i] <= 7));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 15;
      mask = mask << (TwoPowerD + 24 + i * 4 );
      _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
      long int tmp = static_cast<long int>(faceDataTo[i]);
      _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 | ( tmp - -4) << (TwoPowerD + 24 + i * 4);
   }
}



 int peano4::grid::GridTraversalEventPacked::getFaceDataTo(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   long int mask = 15;
   mask = mask << (TwoPowerD + 24 + elementIndex * 4);
   long int tmp = _persistentRecords._packedRecords0 & mask;
   tmp = tmp >> (TwoPowerD + 24 + elementIndex * 4);
   tmp = tmp + -4;
   assertion((tmp >= -4) && (tmp <= 7));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setFaceDataTo(int elementIndex, const int& faceDataTo)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   assertion((faceDataTo >= -4 && faceDataTo <= 7));
   long int mask = 15;
   mask = mask << (TwoPowerD + 24 + elementIndex * 4);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
   long int tmp = static_cast<long int>(faceDataTo);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 | ( tmp - -4) << (TwoPowerD + 24 + elementIndex * 4);
}



 int peano4::grid::GridTraversalEventPacked::getCellData() const  {
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (0));
   long int tmp = static_cast<long int>(_persistentRecords._packedRecords3 & mask);
   tmp = static_cast<long int>(tmp >> (0));
   tmp = tmp + -4;
   assertion(( tmp >= -4 &&  tmp <= 1));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setCellData(const int& cellData)  {
   assertion((cellData >= -4 && cellData <= 1));
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (0));
   _persistentRecords._packedRecords3 = static_cast<long int>(_persistentRecords._packedRecords3 & ~mask);
   _persistentRecords._packedRecords3 = static_cast<long int>(_persistentRecords._packedRecords3 | (static_cast<long int>(cellData) - -4) << (0));
}



 tarch::la::Vector<TwoPowerD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEventPacked::getSendReceiveVertexData() const  {
   long int mask = 7;
   mask = mask << (3);
   tarch::la::Vector<TwoPowerD,DataExchangeType> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _persistentRecords._packedRecords3 & mask;
      tmp = tmp >> (3 + i * 3);
      result[i] = (DataExchangeType) tmp;
      mask = mask << 3;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= 0 && result[i] <= 4));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveVertexData(const tarch::la::Vector<TwoPowerD,DataExchangeType>& sendReceiveVertexData)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((sendReceiveVertexData[i] >= 0 && sendReceiveVertexData[i] <= 4));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 7;
      mask = mask << (3 + i * 3 );
      _persistentRecords._packedRecords3 = _persistentRecords._packedRecords3 & ~mask;
      long int tmp = static_cast<long int>(sendReceiveVertexData[i]);
      _persistentRecords._packedRecords3 = _persistentRecords._packedRecords3 | ( tmp << (3 + i * 3));
   }
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEventPacked::getSendReceiveVertexData(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   long int mask = 7;
   mask = mask << (3 + elementIndex * 3);
   long int tmp = _persistentRecords._packedRecords3 & mask;
   tmp = tmp >> (3 + elementIndex * 3);
   assertion((tmp >= 0) && (tmp <= 4));
   return (DataExchangeType) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveVertexData(int elementIndex, const DataExchangeType& sendReceiveVertexData)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   assertion((sendReceiveVertexData >= 0 && sendReceiveVertexData <= 4));
   long int mask = 7;
   mask = mask << (3 + elementIndex * 3);
   _persistentRecords._packedRecords3 = _persistentRecords._packedRecords3 & ~mask;
   long int tmp = static_cast<long int>(sendReceiveVertexData);
   _persistentRecords._packedRecords3 = _persistentRecords._packedRecords3 | ( tmp << (3 + elementIndex * 3));
}



 tarch::la::Vector<TwoTimesD,peano4::grid::GridTraversalEvent::DataExchangeType> peano4::grid::GridTraversalEventPacked::getSendReceiveFaceData() const  {
   long int mask = 7;
   mask = mask << (0);
   tarch::la::Vector<TwoTimesD,DataExchangeType> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _persistentRecords._packedRecords4 & mask;
      tmp = tmp >> (0 + i * 3);
      result[i] = (DataExchangeType) tmp;
      mask = mask << 3;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= 0 && result[i] <= 4));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveFaceData(const tarch::la::Vector<TwoTimesD,DataExchangeType>& sendReceiveFaceData)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((sendReceiveFaceData[i] >= 0 && sendReceiveFaceData[i] <= 4));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 7;
      mask = mask << (0 + i * 3 );
      _persistentRecords._packedRecords4 = _persistentRecords._packedRecords4 & ~mask;
      long int tmp = static_cast<long int>(sendReceiveFaceData[i]);
      _persistentRecords._packedRecords4 = _persistentRecords._packedRecords4 | ( tmp << (0 + i * 3));
   }
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEventPacked::getSendReceiveFaceData(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   long int mask = 7;
   mask = mask << (0 + elementIndex * 3);
   long int tmp = _persistentRecords._packedRecords4 & mask;
   tmp = tmp >> (0 + elementIndex * 3);
   assertion((tmp >= 0) && (tmp <= 4));
   return (DataExchangeType) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveFaceData(int elementIndex, const DataExchangeType& sendReceiveFaceData)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   assertion((sendReceiveFaceData >= 0 && sendReceiveFaceData <= 4));
   long int mask = 7;
   mask = mask << (0 + elementIndex * 3);
   _persistentRecords._packedRecords4 = _persistentRecords._packedRecords4 & ~mask;
   long int tmp = static_cast<long int>(sendReceiveFaceData);
   _persistentRecords._packedRecords4 = _persistentRecords._packedRecords4 | ( tmp << (0 + elementIndex * 3));
}



 peano4::grid::GridTraversalEvent::DataExchangeType peano4::grid::GridTraversalEventPacked::getSendReceiveCellData() const  {
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (51));
   long int tmp = static_cast<long int>(_persistentRecords._packedRecords3 & mask);
   tmp = static_cast<long int>(tmp >> (51));
   assertion(( tmp >= 0 &&  tmp <= 4));
   return (DataExchangeType) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveCellData(const DataExchangeType& sendReceiveCellData)  {
   assertion((sendReceiveCellData >= 0 && sendReceiveCellData <= 4));
   long int mask =  (1 << (3)) - 1;
   mask = static_cast<long int>(mask << (51));
   _persistentRecords._packedRecords3 = static_cast<long int>(_persistentRecords._packedRecords3 & ~mask);
   _persistentRecords._packedRecords3 = static_cast<long int>(_persistentRecords._packedRecords3 | static_cast<long int>(sendReceiveCellData) << (51));
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::getSendReceiveVertexDataRank() const  {
   return _persistentRecords._sendReceiveVertexDataRank;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveVertexDataRank(const tarch::la::Vector<TwoPowerD,int>& sendReceiveVertexDataRank)  {
   _persistentRecords._sendReceiveVertexDataRank = (sendReceiveVertexDataRank);
}



 int peano4::grid::GridTraversalEventPacked::getSendReceiveVertexDataRank(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   return _persistentRecords._sendReceiveVertexDataRank[elementIndex];
   
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveVertexDataRank(int elementIndex, const int& sendReceiveVertexDataRank)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._sendReceiveVertexDataRank[elementIndex]= sendReceiveVertexDataRank;
   
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::getSendReceiveFaceDataRank() const  {
   return _persistentRecords._sendReceiveFaceDataRank;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveFaceDataRank(const tarch::la::Vector<TwoTimesD,int>& sendReceiveFaceDataRank)  {
   _persistentRecords._sendReceiveFaceDataRank = (sendReceiveFaceDataRank);
}



 int peano4::grid::GridTraversalEventPacked::getSendReceiveFaceDataRank(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   return _persistentRecords._sendReceiveFaceDataRank[elementIndex];
   
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveFaceDataRank(int elementIndex, const int& sendReceiveFaceDataRank)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   _persistentRecords._sendReceiveFaceDataRank[elementIndex]= sendReceiveFaceDataRank;
   
}



 int peano4::grid::GridTraversalEventPacked::getSendReceiveCellDataRank() const  {
   return _persistentRecords._sendReceiveCellDataRank;
}



 void peano4::grid::GridTraversalEventPacked::setSendReceiveCellDataRank(const int& sendReceiveCellDataRank)  {
   _persistentRecords._sendReceiveCellDataRank = sendReceiveCellDataRank;
}



 tarch::la::Vector<Dimensions,int> peano4::grid::GridTraversalEventPacked::getRelativePositionToFather() const  {
   return _persistentRecords._relativePositionToFather;
}



 void peano4::grid::GridTraversalEventPacked::setRelativePositionToFather(const tarch::la::Vector<Dimensions,int>& relativePositionToFather)  {
   _persistentRecords._relativePositionToFather = (relativePositionToFather);
}



 int peano4::grid::GridTraversalEventPacked::getRelativePositionToFather(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._relativePositionToFather[elementIndex];
   
}



 void peano4::grid::GridTraversalEventPacked::setRelativePositionToFather(int elementIndex, const int& relativePositionToFather)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._relativePositionToFather[elementIndex]= relativePositionToFather;
   
}


std::string peano4::grid::GridTraversalEventPacked::toString(const DataExchangeType& param) {
   return peano4::grid::GridTraversalEvent::toString(param);
}

std::string peano4::grid::GridTraversalEventPacked::getDataExchangeTypeMapping() {
   return peano4::grid::GridTraversalEvent::getDataExchangeTypeMapping();
}



std::string peano4::grid::GridTraversalEventPacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::GridTraversalEventPacked::toString (std::ostream& out) const {
   out << "("; 
   out << "x:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(Dimensions-1) << "]";
   out << ",";
   out << "h:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getH(i) << ",";
   }
   out << getH(Dimensions-1) << "]";
   out << ",";
   out << "isRefined:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getIsRefined(i) << ",";
   }
   out << getIsRefined(TwoPowerD-1) << "]";
   out << ",";
   out << "vertexDataFrom:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getVertexDataFrom(i) << ",";
   }
   out << getVertexDataFrom(TwoPowerD-1) << "]";
   out << ",";
   out << "vertexDataTo:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getVertexDataTo(i) << ",";
   }
   out << getVertexDataTo(TwoPowerD-1) << "]";
   out << ",";
   out << "faceDataFrom:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getFaceDataFrom(i) << ",";
   }
   out << getFaceDataFrom(TwoTimesD-1) << "]";
   out << ",";
   out << "faceDataTo:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getFaceDataTo(i) << ",";
   }
   out << getFaceDataTo(TwoTimesD-1) << "]";
   out << ",";
   out << "cellData:" << getCellData();
   out << ",";
   out << "sendReceiveVertexData:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getSendReceiveVertexData(i) << ",";
   }
   out << getSendReceiveVertexData(TwoPowerD-1) << "]";
   out << ",";
   out << "sendReceiveFaceData:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getSendReceiveFaceData(i) << ",";
   }
   out << getSendReceiveFaceData(TwoTimesD-1) << "]";
   out << ",";
   out << "sendReceiveCellData:" << toString(getSendReceiveCellData());
   out << ",";
   out << "sendReceiveVertexDataRank:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getSendReceiveVertexDataRank(i) << ",";
   }
   out << getSendReceiveVertexDataRank(TwoPowerD-1) << "]";
   out << ",";
   out << "sendReceiveFaceDataRank:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getSendReceiveFaceDataRank(i) << ",";
   }
   out << getSendReceiveFaceDataRank(TwoTimesD-1) << "]";
   out << ",";
   out << "sendReceiveCellDataRank:" << getSendReceiveCellDataRank();
   out << ",";
   out << "relativePositionToFather:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getRelativePositionToFather(i) << ",";
   }
   out << getRelativePositionToFather(Dimensions-1) << "]";
   out <<  ")";
}


peano4::grid::GridTraversalEventPacked::PersistentRecords peano4::grid::GridTraversalEventPacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::GridTraversalEvent peano4::grid::GridTraversalEventPacked::convert() const{
   return GridTraversalEvent(
      getX(),
      getH(),
      getIsRefined(),
      getVertexDataFrom(),
      getVertexDataTo(),
      getFaceDataFrom(),
      getFaceDataTo(),
      getCellData(),
      getSendReceiveVertexData(),
      getSendReceiveFaceData(),
      getSendReceiveCellData(),
      getSendReceiveVertexDataRank(),
      getSendReceiveFaceDataRank(),
      getSendReceiveCellDataRank(),
      getRelativePositionToFather()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridTraversalEventPacked::_log( "peano4::grid::GridTraversalEventPacked" );
   
   MPI_Datatype peano4::grid::GridTraversalEventPacked::Datatype = 0;
   MPI_Datatype peano4::grid::GridTraversalEventPacked::FullDatatype = 0;
   
   
   void peano4::grid::GridTraversalEventPacked::initDatatype() {
      {
         GridTraversalEventPacked dummyGridTraversalEventPacked[16];
         
         #ifdef MPI2
         const int Attributes = 11;
         #else
         const int Attributes = 11+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_INT		 //sendReceiveVertexDataRank
            , MPI_INT		 //sendReceiveFaceDataRank
            , MPI_INT		 //sendReceiveCellDataRank
            , MPI_INT		 //relativePositionToFather
            , MPI_LONG		 //_packedRecords0
            , MPI_LONG		 //_packedRecords1
            , MPI_LONG		 //_packedRecords2
            , MPI_LONG		 //_packedRecords3
            , MPI_LONG		 //_packedRecords4
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , TwoPowerD		 //sendReceiveVertexDataRank
            , TwoTimesD		 //sendReceiveFaceDataRank
            , 1		 //sendReceiveCellDataRank
            , Dimensions		 //relativePositionToFather
            , 1		 //_packedRecords0
            , 1		 //_packedRecords1
            , 1		 //_packedRecords2
            , 1		 //_packedRecords3
            , 1		 //_packedRecords4
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords2))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords2))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords3))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords3))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords4))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords4))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEventPacked::Datatype );
         MPI_Type_commit( &GridTraversalEventPacked::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEventPacked::Datatype);
         MPI_Type_commit( &GridTraversalEventPacked::Datatype );
         #endif
         
      }
      {
         GridTraversalEventPacked dummyGridTraversalEventPacked[16];
         
         #ifdef MPI2
         const int Attributes = 11;
         #else
         const int Attributes = 11+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_INT		 //sendReceiveVertexDataRank
            , MPI_INT		 //sendReceiveFaceDataRank
            , MPI_INT		 //sendReceiveCellDataRank
            , MPI_INT		 //relativePositionToFather
            , MPI_LONG		 //_packedRecords0
            , MPI_LONG		 //_packedRecords1
            , MPI_LONG		 //_packedRecords2
            , MPI_LONG		 //_packedRecords3
            , MPI_LONG		 //_packedRecords4
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , TwoPowerD		 //sendReceiveVertexDataRank
            , TwoTimesD		 //sendReceiveFaceDataRank
            , 1		 //sendReceiveCellDataRank
            , Dimensions		 //relativePositionToFather
            , 1		 //_packedRecords0
            , 1		 //_packedRecords1
            , 1		 //_packedRecords2
            , 1		 //_packedRecords3
            , 1		 //_packedRecords4
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveVertexDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveFaceDataRank[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._sendReceiveCellDataRank))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._relativePositionToFather[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords2))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords2))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords3))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords3))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords4))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords4))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEventPacked::FullDatatype );
         MPI_Type_commit( &GridTraversalEventPacked::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEventPacked::FullDatatype);
         MPI_Type_commit( &GridTraversalEventPacked::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::grid::GridTraversalEventPacked::shutdownDatatype() {
      MPI_Type_free( &GridTraversalEventPacked::Datatype );
      MPI_Type_free( &GridTraversalEventPacked::FullDatatype );
      
   }
   
   void peano4::grid::GridTraversalEventPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridTraversalEventPacked " 
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result); 
         _log.error( "send(int)",msg.str() ); 
       } 
    } 
    break; 
   case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    {
      MPI_Request* sendRequestHandle = new MPI_Request(); 
      int          flag = 0; 
       int          result; 
       clock_t      timeOutWarning   = -1; 
       clock_t      timeOutShutdown  = -1; 
       bool         triggeredTimeoutWarning = false;  
       result = MPI_Isend(  
         this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination,  
         tag, tarch::mpi::Rank::getInstance().getCommunicator(), 
         sendRequestHandle  
       ); 
       if  (result!=MPI_SUCCESS) {  
         std::ostringstream msg;  
         msg << "was not able to send message peano4::grid::GridTraversalEventPacked "  
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result);  
         _log.error( "send(int)",msg.str() );  
       }  
       result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
       while (!flag) { 
         if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
         result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
           std::ostringstream msg; 
           msg << "testing for finished send task for peano4::grid::GridTraversalEventPacked " 
               << toString() 
               << " sent to node " << destination 
               << " failed: " << tarch::mpi::MPIReturnValueToString(result); 
           _log.error("send(int)", msg.str() ); 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
           (clock()>timeOutWarning) && 
           (!triggeredTimeoutWarning) 
         ) { 
           tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
             "peano4::grid::GridTraversalEventPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridTraversalEventPacked", 
             "send(int)", destination,tag,1 
           ); 
         } 
 	       tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
       } 
       delete sendRequestHandle; 
     }  
     break; 
   case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    assertionMsg(false,"should not be called"); 
    break; 
} 
 // ============================= 
// end injected snippet/aspect 
// ============================= 

      
   }
   
   
   
   void peano4::grid::GridTraversalEventPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
MPI_Status status; 
switch (mode) { 
  case ExchangeMode::Blocking: 
    { 
      const int   result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEventPacked from rank " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    } 
    break; 
  case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    { 
      int          flag = 0; 
      int          result; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      MPI_Request* sendRequestHandle = new MPI_Request(); 
       result = MPI_Irecv( 
        this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, 
        tarch::mpi::Rank::getInstance().getCommunicator(), sendRequestHandle 
      ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEventPacked from rank " 
             << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
      result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEventPacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      delete sendRequestHandle; 
    }    break; 
  case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    {
      int flag; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      int result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
       if (result!=MPI_SUCCESS) { 
        std::ostringstream msg; 
        msg << "testing for finished receive task for peano4::grid::GridTraversalEventPacked failed: " 
            << tarch::mpi::MPIReturnValueToString(result); 
        _log.error("receive(int)", msg.str() ); 
      } 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEventPacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEventPacked from rank " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    }
    break; 
  } 
// =========================== 
// end injected snippet/aspect 
// =========================== 

      
     _senderDestinationRank = source==MPI_ANY_SOURCE ? status.MPI_SOURCE : source;
   }
   
   
   
   bool peano4::grid::GridTraversalEventPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Status status;
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, &status
      );
      if (flag) {
         int  messageCounter;
         if (exchangeOnlyAttributesMarkedWithParallelise) {
            MPI_Get_count(&status, Datatype, &messageCounter);
         }
         else {
            MPI_Get_count(&status, FullDatatype, &messageCounter);
         }
         return messageCounter > 0;
      }
      else return false;
      
   }
   
   int peano4::grid::GridTraversalEventPacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



