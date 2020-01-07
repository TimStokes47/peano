#include "PlotGrid.h"


examples::finitevolumes::mappings::PlotGrid::PlotGrid(int spaceTree) {
// @todo Please implement
 }


std::vector< peano4::grid::GridControlEvent > examples::finitevolumes::mappings::PlotGrid::getGridControlEvents() {
  std::vector< peano4::grid::GridControlEvent > result;


  result.push_back(peano4::grid::GridControlEvent(
    peano4::grid::GridControlEvent::RefinementControl::Refine,
	tarch::la::Vector<Dimensions,double>(0.0),
	tarch::la::Vector<Dimensions,double>(1.0),
	tarch::la::Vector<Dimensions,double>(0.1)
  ));

  return result;
}


void examples::finitevolumes::mappings::PlotGrid::beginTraversal() {
// @todo Please implement
 }


void examples::finitevolumes::mappings::PlotGrid::endTraversal() {
// @todo Please implement
 }


void examples::finitevolumes::mappings::PlotGrid::createPersistentFace(const tarch::la::Vector<Dimensions,double>& center, const tarch::la::Vector<Dimensions,double>& h, int normal, examples::finitevolumes::facedata::Q& faceQ) {
// @todo Please implement
 }


void examples::finitevolumes::mappings::PlotGrid::destroyPersistentFace(const tarch::la::Vector<Dimensions,double>& center, const tarch::la::Vector<Dimensions,double>& h, int normal, examples::finitevolumes::facedata::Q& faceQ) {
// @todo Please implement
 }


void examples::finitevolumes::mappings::PlotGrid::createHangingFace(const tarch::la::Vector<Dimensions,double>& center, const tarch::la::Vector<Dimensions,double>& h, int normal, examples::finitevolumes::facedata::Q& faceQ) {
// @todo Please implement
 }


void examples::finitevolumes::mappings::PlotGrid::destroyHangingFace(const tarch::la::Vector<Dimensions,double>& center, const tarch::la::Vector<Dimensions,double>& h, int normal, examples::finitevolumes::facedata::Q& faceQ) {
// @todo Please implement
 }


void examples::finitevolumes::mappings::PlotGrid::createCell(const tarch::la::Vector<Dimensions,double>& center, const tarch::la::Vector<Dimensions,double>& h, peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q>& facesQ, examples::finitevolumes::celldata::Q& cellQ) {
// @todo Please implement
 }


void examples::finitevolumes::mappings::PlotGrid::destroyCell(const tarch::la::Vector<Dimensions,double>& center, const tarch::la::Vector<Dimensions,double>& h, peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q>& facesQ, examples::finitevolumes::celldata::Q& cellQ) {
// @todo Please implement
 }


