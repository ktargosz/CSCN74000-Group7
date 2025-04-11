#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>


int main() {
    std::ofstream file("telemetry_log.txt");
    if (!file.is_open()) {
        std::cerr << "Failed to create telemetry_log.txt\n";
        return 1;
    }

    const size_t targetSize = 1048576; // 1MB
    size_t currentSize = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> alt(29000, 41000);
    std::uniform_int_distribution<> spd(420, 520);
    std::uniform_int_distribution<> temp(-55, -30);
    std::uniform_real_distribution<> fuel(20.0, 100.0);

    while (currentSize < targetSize) {
        // Generate ISO 8601 timestamp
        time_t now = time(nullptr);
        char timeStr[30];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

        std::ostringstream line;
        line << "TIME: " << timeStr
            << " | ALT: " << alt(gen) << " ft"
            << " | SPD: " << spd(gen) << " kt"
            << " | TEMP: " << temp(gen) << " C"
            << " | FUEL: " << std::fixed << std::setprecision(1) << fuel(gen) << "%\n";

        std::string entry = line.str();
        file << entry;
        currentSize += entry.size();
    }

    file.close();
    std::cout << "Generated telemetry_log.txt (~" << currentSize << " bytes)\n";
    return 0;
}
