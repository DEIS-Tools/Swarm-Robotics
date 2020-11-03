//
// Created by martin on 20/10/20.
//

#ifndef SWARMSIMULATOR_UPPAAL_MODEL_PARSING_H
#define SWARMSIMULATOR_UPPAAL_MODEL_PARSING_H

#include "models/robot/robot.hpp"
#include "models/map/map_structure.hpp"

#include <string>
#include <vector>
#include <map>
#include <cmath>

struct abs_robot_info {
    std::string name;
    std::vector<int> remaining_stations;

    abs_robot_info(std::string& name, std::vector<int>& remaining_stations) :
    name(name),
    remaining_stations(remaining_stations)
    {}

};

struct robot_at_point : abs_robot_info {
    int current_location;

    robot_at_point(std::string& name, std::vector<int>& remaining_stations, int current_location) :
    abs_robot_info{name, remaining_stations},
    current_location(current_location)
    {}
};

struct robot_moving : abs_robot_info {
    int latest_point;
    int current_target;
    int clock; // Not a float since we do not want that level of detail.

    robot_moving(std::string& name, std::vector<int>& remaining_stations, int latest_point, int current_target,
        int clock) : abs_robot_info(name, remaining_stations), latest_point(latest_point), current_target(current_target),
        clock(clock)
        {}
};


std::string constructUppaalModel(std::vector<Robot> &robots, Robot &currentRobot, bool stations);


// Functions for getting the information to the Uppaal XML from map_structure
std::vector<abs_robot_info> get_robot_plans_and_positions(std::vector<Robot> &robots, Robot &currentRobot);
std::string number_of_stations(const Map_Structure &map_structure);
int number_of_waypoints(const Map_Structure &map_structure);
int number_of_robots(const Map_Structure& map_structure);
std::vector<int> get_end_stations(const Map_Structure &map_structure);
std::vector<std::vector<int>> get_distances(const Map_Structure& map_structure);


// Functions for getting the strings needed for the placeholders in the Uppaal XML
void configure_static_settings_of_Uppaal_model(const Map_Structure& map_structure);

#endif //SWARMSIMULATOR_UPPAAL_MODEL_PARSING_H