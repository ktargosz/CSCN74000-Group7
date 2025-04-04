#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>

namespace flight {

    struct FlightData {
        int flightID;
        int aircraftID;
        std::string arrivalTime;
        std::string departureTime;
        std::string zone;
        std::string status;
    };

    // Future shared functions can be declared here
    // e.g. std::string serializeFlight(const FlightData& data);

}  // namespace flight

#endif
