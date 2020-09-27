// Copyright 2020 The Google Research Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Simple main to demonstrate use of libsumo interface.
//
// The inputs to the job can be generated by following the steps at:
// https://sumo.dlr.de/wiki/Tutorials/Hello_Sumo. Alternatively, these files
// have been pregenerated and are hecked in at
// //third_party/sumo/sumo-1.1.0/docs/tutorial/hello/data.
//
// Example invocation:
//     SUMO_HOME=${SUMO_PATH}/sumo/sumo-1.1.0 \
//         blaze-bin/research/simulation/traffic/libsumo_demo_main \
//         --config_file=third_party/sumo/sumo-1.1.0/docs/tutorial/hello/data\
//         /hello.sumocfg --num_steps=100 --alsologtostderr
// NB: The way that SUMO imports headers is problematic. Vehicle.h requires
// PositionVector.h, so we need to include the header before we include
// Vehicle.h.
// TODO(yusef): Devise a better solution to deal with such imports.
#include <utils/geom/PositionVector.h>

#include <gflags/gflags.h>

#include "glog/logging.h"
#include "third_party/sumo/sumo_1_6_0/src/libsumo/Simulation.h"
#include "third_party/sumo/sumo_1_6_0/src/libsumo/Vehicle.h"

DEFINE_string(
    config_file,
    "third_party/sumo/sumo_1_6_0/docs/tutorial/hello/data/hello.sumocfg",
    "Simulation configuration.");
DEFINE_int64(num_steps, 0, "Number of steps.");

namespace research {
namespace simulation {
int Main(int argc, char** argv) {
  std::vector<std::string> args = {FLAGS_config_file};
  // The following line will crash (throw an exception) if the args are invalid.
  libsumo::Simulation::load(args);

  // We demonstrate two simple use cases of the libsumo API.
  // First, we use the subscription API. 0x7a corresponds to a list of arrived
  // vehicles (https://sumo.dlr.de/wiki/TraCI/Simulation_Value_Retrieval).
  // Second, we call the Vehicle API directly without a subscription to retrieve
  // current vehicles and their speeds.
  libsumo::Simulation::subscribe({0x7a}, 0, 10000);
  for (int i = 0; i < FLAGS_num_steps; ++i) {
    libsumo::Simulation::step();
    const auto results = libsumo::Simulation::getSubscriptionResults();
    for (const auto& pair : results) {
      LOG(INFO) << pair.first;
      LOG(INFO) << pair.second->getString();
    }
    for (const auto& vehicle : libsumo::Vehicle::getIDList()) {
      LOG(INFO) << vehicle << " speed: " << libsumo::Vehicle::getSpeed(vehicle);
    }
  }
  return 0;
}

}  // namespace simulation
}  // namespace research

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  return research::simulation::Main(argc, argv);
}
