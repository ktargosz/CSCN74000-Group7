#pragma once
#include <string>

enum class FlightStatus {
    Scheduled,
    Delayed,
    Cancelled,
    Boarding,
    InFlight,
    Landed
};

struct FlightInfo {
    int flightID;
    int aircraftID;
    std::string departureTime;
    std::string arrivalTime;
    std::string zone;
    FlightStatus status;
};
