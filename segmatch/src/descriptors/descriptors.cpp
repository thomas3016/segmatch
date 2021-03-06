#include "segmatch/descriptors/descriptors.hpp"

#include <Eigen/Dense>
#include <glog/logging.h>
#include <laser_slam/common.hpp>

#include "segmatch/descriptors/eigenvalue_based.hpp"
#include "segmatch/descriptors/ensemble_shape_functions.hpp"

namespace segmatch {

// Descriptors methods definition
Descriptors::Descriptors() {
  //TODO: Implement default construction from default parameters.
  CHECK(false) << "Construction of descriptors without parameters is not implemented.";
}

Descriptors::~Descriptors() {}

Descriptors::Descriptors(const DescriptorsParameters& parameters) {
  CHECK_GT(parameters.descriptor_types.size(), 0) << "Description impossible without a descriptor.";

  // Create the descriptors.
  for (size_t i = 0u; i < parameters.descriptor_types.size(); ++i) {
    if (parameters.descriptor_types[i] == "EigenvalueBased") {
      descriptors_.push_back(std::unique_ptr<Descriptor>(
          new EigenvalueBasedDescriptor(parameters)));
    } else if (parameters.descriptor_types[i] == "EnsembleShapeFunctions") {
      descriptors_.push_back(std::unique_ptr<Descriptor>(new EnsembleShapeFunctions(parameters)));
    } else {
      CHECK(false) << "The descriptor '" << parameters.descriptor_types[i] <<
          "' was not implemented.";
    }
  }
}

void Descriptors::describe(const Segment& segment, Features* features) {
  CHECK_NOTNULL(features)->clear();
  CHECK_GT(descriptors_.size(), 0) << "Description impossible without a descriptor.";
  for (size_t i = 0u; i < descriptors_.size(); ++i) {
    descriptors_[i]->describe(segment, features);
  }
}

void Descriptors::describe(SegmentedCloud* segmented_cloud_ptr,
                           std::vector<double>* timings) {
  if (timings != NULL) {
    timings->clear();
  }
  CHECK_NOTNULL(segmented_cloud_ptr);
  CHECK_GT(descriptors_.size(), 0) << "Description impossible without a descriptor.";
  for (size_t i = 0u; i < descriptors_.size(); ++i) {
    laser_slam::Clock clock_descriptor;
    descriptors_[i]->describe(segmented_cloud_ptr);
    if (timings != NULL) {
      clock_descriptor.takeTime();
      timings->push_back(clock_descriptor.getRealTime());
      LOG(INFO) << "Decriptor " << i << " took " << clock_descriptor.getRealTime() << " ms.";
      clock_descriptor.start();
    }
  }
}

unsigned int Descriptors::dimension() const {
  CHECK_GT(descriptors_.size(), 0) << "Description impossible without a descriptor.";
  unsigned int dimension = 0;
  for (size_t i = 0u; i < descriptors_.size(); ++i) {
    dimension += descriptors_[i]->dimension();
  }
  return dimension;
}

} // namespace segmatch
