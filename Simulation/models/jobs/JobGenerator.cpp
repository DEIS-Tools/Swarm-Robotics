

#include "JobGenerator.hpp"

#include <random>
#include <set>
#include <exception>

JobGenerator::JobGenerator(int numOfStations, std::set<int> endStations, int numOfJobs) :
    numOfStations(numOfStations),
    endStations(endStations),
    numOfEndStations((int)endStations.size()),
    numOfJobs(numOfJobs){}

// This function is hardcored in the way that end stations have the first IDs in the map and the stations have the
// next IDs. After the stations, the waypoints gets IDs.
Job JobGenerator::generateJob() {
    //************creates random jobs in json file************
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(numOfEndStations, numOfStations - 1);   // define the range of stations ids
    std::uniform_int_distribution<> distrEnd(2, 4); // define the range of how many stations to visit

    int amountPickups = distrEnd(eng);
    std::set<int> stationsToVisit{};

    while (stationsToVisit.size() < amountPickups){
        stationsToVisit.insert(distr(eng));
    }

    return Job{stationsToVisit, endStations};
}

bool JobGenerator::anyJobsLeft() {
    return numOfJobs > jobsGenerated;
}

bool JobGenerator::allJobsCompleted() {
    return numOfStations - jobsCompleted == 0;
}

Job JobGenerator::getNextJob() {
    return generateJob();
}

void JobGenerator::completedJob() {
    jobsCompleted++;

    if(jobsCompleted > numOfJobs)
        throw std::logic_error("More completed jobs than jobs generated");
}