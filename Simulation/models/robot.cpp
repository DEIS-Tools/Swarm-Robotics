#include "robot.hpp"
#include "map_structure.hpp"

Robot::Robot(CFootBotEntity *footBot, Point *initialLoc) {
    initialLocation = initialLoc;
    this->footBot = footBot;
    status = Status::available;
    etaNextStation = 0.0;
    currentPositionId = initialLoc;

}
Robot &Robot::operator=(const Robot &that) {
    footBot = that.footBot;
    initialLocation = that.initialLocation;
    status = Status::available;
    etaNextStation = 0.0;
    currentPositionId = initialLocation;
    return *this;
}
Point& Robot::getInitialLoc() {
    return *initialLocation;
}

Point Robot::getNextStation() {
    if (!remainingStations.empty()) {
        return remainingStations[0];
    } else
        return getInitialLoc();
}

void Robot::increment(int i) { stopWatch = i; }
void Robot::setEta(double time) { etaNextStation = time; }
void Robot::setJob(std::vector<Point> &jobs) {
    for (auto i = 0u; i < jobs.size(); i++) {
        job.push_back(jobs[i]);
    }
}
void Robot::setCurrStationTarget(){
    Map_Structure &sMap = Map_Structure::get_instance();
    currTarget = &sMap.getPointByID(remainingStations.front().getId());


}
double Robot::getEta() {
    Map_Structure &sMap = Map_Structure::get_instance();
    etaNextStation= 0;
    argos::CVector3 curr = footBot->GetEmbodiedEntity().GetOriginAnchor().Position;
    for(long unsigned i = 0; i < getRemainingWaypoints().size(); i++){
        if(i == 0) etaNextStation = argos::Distance(remainingWaypoints.front(),curr);
        else etaNextStation += (remainingWaypoints[i-1] - remainingWaypoints[i]).Length();
    }
    if(getRemainingWaypoints().size() == 1){ // meaning that the next waypoint is already station
        std::vector<Point> path = sMap.findPath(remainingStations.front().getId(),remainingStations[1].getId() );
        ////std::cout<< "Path for <ETA"<< std::endl;
        for(long unsigned i =0; i < path.size(); i++){
            if(i == 0 ) etaNextStation = argos::Distance(remainingStations.front(), path.front());
            else etaNextStation += argos::Distance(remainingStations[i-1],remainingStations[i]);
        }
    }
    ////std::cout <<etaNextStation<<std::endl;
    return etaNextStation = etaNextStation/ VELOCITY *100; }
argos::CVector3 getPositionHelper(CVector3 start, CVector3 end, argos::Real x, argos::Real y){
    argos::Real result_X, result_Y;
    result_X = start.GetX() + x;
    result_Y = start.GetY() + y;
    if(start.GetX() <= result_X && result_X <= end.GetX() &&
       start.GetY() <= result_Y && result_Y <= end.GetY()){
        return argos::CVector3(result_X, result_Y, 0);
    }
    if(start.GetX() >= result_X && result_X >= end.GetX() &&
       start.GetY() >= result_Y && result_Y >= end.GetY()){
        return argos::CVector3(result_X, result_Y, 0);
    }
    if(start.GetX() <= result_X && result_X <= end.GetX() &&
       start.GetY() >= result_Y && result_Y >= end.GetY()){
        return argos::CVector3(result_X, result_Y, 0);
    }
    if(start.GetX() >= result_X && result_X >= end.GetX() &&
       start.GetY() <= result_Y && result_Y <= end.GetY()){
        return argos::CVector3(result_X, result_Y, 0);
    }
    result_X = start.GetX() - x;
    result_Y = start.GetY() - y;
    return argos::CVector3(result_X, result_Y, 0);
}
argos::CVector3 Robot::getPosition(CVector3 start, CVector3 end, double distance){
    double x, y, teta;
    y = start.GetY() - end.GetY();
    x = start.GetX() - end.GetX();
    if(x != 0) teta = atan(y/x);
    else return getPositionHelper(start, end, 0, distance);
    if(y != 0) teta = atan(y/x);
    else return getPositionHelper(start, end, distance, 0);
    return getPositionHelper(start, end, cos(teta) * distance, sin(teta)*distance);
}
bool checkDelayMatch(double timeToDelay, bool once, double temp){
    if (timeToDelay - temp < 0 && once){
        return true;
    }
    return false;
}
timeResult* Robot::getEtaHelper(std::string id, std::vector<Point> waypoints, argos::CVector3 currPosition, double timeToDelay,
                                double temp, std::vector<Point>& allWaypoints) {
    bool once = true;
    struct added{bool added = false;
        bool station;};
    added check;
    argos::CVector3 position;
    int passedStations =0;
    std::vector<Point> passedWaypoints;
    for(long unsigned i = 0; i < waypoints.size(); i++){
        if(i == 0 && temp == 0) temp = argos::Distance(currPosition,waypoints.front());
        else if(i == 0 && temp !=0) temp += argos::Distance(currPosition,waypoints.front());
        else temp += argos::Distance(waypoints[i-1], waypoints[i]);
        if(!checkDelayMatch(timeToDelay,once,temp)){
            if(waypoints[i].getType()==pointType::station) {temp+=7.5* VELOCITY /100; // addition of delay + rotation
                check.station= true;
                check.added= true;
            }//requires to convert 6 times units to distance;
            else{temp+=2;check.station= false;} // adding 2 time units for the robots rotation
        }
        if(checkDelayMatch(timeToDelay,once,temp)){
            temp = (timeToDelay - temp) * -1;
            once = false;
            passedStations = i;
            if(check.added){position = waypoints.back();
                Point p = waypoints[i];
                allWaypoints.push_back(p);}
            else if(i == 0) position =getPosition(currPosition,waypoints.front(), temp);
            else position = getPosition(waypoints[i-1], waypoints[i], temp);
        }
        check.added = false;
        if(once) {Point p = waypoints[i];
            passedWaypoints.push_back(p);
            allWaypoints.push_back(p);}
    }
    //std::cout<< "-------------------------"<< std::endl;
    return new timeResult{id,temp, passedStations, passedWaypoints, position, once,allWaypoints   };

}
void Robot::addEndPoint(){
    Map_Structure &sMap = Map_Structure::get_instance();
    remainingStations.push_back(sMap.getPointByID(1));
}
timeResult* Robot::getEtaNextRobot(Robot r, double timeToDelay) {
    Map_Structure &sMap = Map_Structure::get_instance();
    timeToDelay = timeToDelay* VELOCITY /100; // back to distance
    double temp = 0;
    if(r.getWatch()!= -1) {temp = (stationDelay- r.getWatch())/10;} // convert to distance
    argos::CVector3 currPosition;
    std::vector<Point> passedWaypoints;
    int passedStations = 0;
    std::vector<Point> allPassedPoints;
    currPosition = r.getfootBot()->GetEmbodiedEntity().GetOriginAnchor().Position;
    allPassedPoints.push_back(Point(currPosition,pointType::via,"CurrentPosition"));
    if(timeToDelay < argos::Distance(currPosition,r.getRemainingWaypoints().front())){
        argos::CVector3 position = getPosition(currPosition,r.getRemainingWaypoints().front(),timeToDelay );
        return new timeResult{r.getfootBot()->GetId(),timeToDelay, 0, passedWaypoints, position, false,allPassedPoints   };
    }

    if(temp >= timeToDelay){
        return new timeResult{r.getfootBot()->GetId(),temp, passedStations, passedWaypoints, currPosition, false ,allPassedPoints};
    }
    timeResult* result = getEtaHelper(r.getfootBot()->GetId(),r.getRemainingWaypoints(), currPosition,timeToDelay,temp, allPassedPoints);
    if(r.remainingStations.size() == 1){
        return nullptr;
    }


    if(result->found){
        for(long unsigned i = 1; i < r.getRemainingStations().size();i++){
            std::vector<Point> path = sMap.findPath(r.getRemainingStations()[i-1].getId(),r.getRemainingStations()[i].getId());
            result = getEtaHelper(r.getfootBot()->GetId(), path, r.getRemainingStations()[i-1],timeToDelay,result->distance, allPassedPoints);

            if(!result->found) break;
        }
    }
    if(result->found) return nullptr;

    return result;
}


void Robot::addSinglePickUp(Point pickup) { job.push_back(pickup); }
void Robot::removeFirstStation() {
    remainingStations.erase(remainingStations.begin());
}
void Robot::removeFirstWaypoint() {

    currentPositionId = &Map_Structure::get_instance().getPointByID(remainingWaypoints[0].getId());
    remainingWaypoints.erase(remainingWaypoints.begin());
}
void Robot::cleanJob() { job.clear(); }

Point *Robot::getNextWayPoint() {
    if (!remainingWaypoints.empty()) {
        return &remainingWaypoints[0];
    } else
        return nullptr;
}
void Robot::changeStatus(Status stat) { status = stat; }
bool Robot::contains(int id, std::vector<Point>& points) {
    for (auto& point: points) {
        if (point.getId() == id)
            return true;
    }return false;
}

std::vector<Point> Robot::setRemainingStations(std::vector<Point> allPoints) {
    remainingStations.clear();
    for (long unsigned i = 0; i < stationPath[2].runs[0].values.size(); i++) {
        for (long unsigned j = 0; j < allPoints.size(); j++) {
            if (allPoints[j].getId() == stationPath[2].runs[0].values[i].value) {
                if (stationPath[2].runs[0].values[i].time == 0 &&
                    stationPath[2].runs[0].values[i].value == 0) {
                } else if (!contains(allPoints[j].getId(), remainingStations)) {
                    remainingStations.push_back(allPoints[j]);
                }
            }
        }
    }
    return remainingStations;
}
void Robot::addWaypoints(std::vector<Point> path) {
    for(auto& p: path){
        remainingWaypoints.push_back(std::move(p));
    }


}
void Robot::updateCurrent(Point *target){
    currentPositionId = target;
}
std::vector<Point> Robot::setRemainingWaypoints(std::vector<Point> &allPoints) {
    remainingWaypoints.clear();
    for (long unsigned i = 0; i < waypointPath[1].runs[0].values.size(); i++) {
        for (long unsigned j = 0; j < allPoints.size(); j++) {
            if (allPoints[j].getId() == waypointPath[1].runs[0].values[i].value) {
                if ((waypointPath[1].runs[0].values[i].time == 0 &&
                    waypointPath[1].runs[0].values[i].value == 0) ||
                    waypointPath[1].runs[0].values[i].value == -1) {
                } else if (!contains(allPoints[j].getId(), remainingWaypoints)) {
                    remainingWaypoints.push_back(allPoints[j]);
                }
            }
        }
    }
    if (remainingWaypoints.empty())
        remainingWaypoints.push_back(allPoints[0]);

    return remainingWaypoints;
}
void Robot::converJSONStation(std::string robotId, std::string choice) {
    std::string path;
    Map_Structure sMap = Map_Structure::get_instance();
    if (choice == "Stations") {
        stationPath.clear();
        path = sMap.folderPath + "/" + robotId + "/" + robotId + "Stations.json";
    }
    if (choice == "Waypoints") {
        waypointPath.clear();
        path = sMap.folderPath + "/" + robotId + "/" + robotId + "Waypoints.json";
    }
    std::ifstream i(path);

    nlohmann::json js = nlohmann::json::parse(i);
    for (long unsigned i = 0; i < js.size(); i++) {
        SimulationExpression se;
        se.name = js[i].value("name", "");
        SimulationTrace smt;
        for (long unsigned j = 0; j < js[i]["run"].size(); j++) {
            smt.number = js[i]["run"][j].value("number", 0);
            for (long unsigned k = 0; k < js[i]["run"][j]["values"].size(); k++) {
                TimeValuePair tvp;
                tvp.time = js[i]["run"][j]["values"][k].value("time", 0.0);
                tvp.value = js[i]["run"][j]["values"][k].value("value", 0);
                smt.values.push_back(tvp);
            }
            se.runs.push_back(smt);
        }
        if (choice == "Stations")
            stationPath.push_back(se);
        if (choice == "Waypoints")
            waypointPath.push_back(se);
    }
}
/*
std::string Robot::createDynamicJson(std::vector<Robot> &robots, Robot &robot, bool stations){
    // What we need from the other robots are:
    // - Their plan
    // - Current location: either point or edge including time spend on the edge.
    std::vector<std::vector<int>> plans{};
    std::vector<std::pair<bool,int>> current_locations{};

    // This gets the plans of the robots.
    for(auto& other_robot: robots){
        if(other_robot != robot){
            plans.emplace_back(std::vector<int>{});
            for(Point& p : other_robot.getRemainingStations()){
                plans.back().push_back(p.getId());
            }
        }
    }

    //Get the information about the whereabouts of the robot.
    for(auto& other_robot: robots){
        if(other_robot.atPoint()){
            current_locations.push_back(std::make_pair(true, other_robot.getCurrentID().getId()));
        }
        else{
            // Edges does not have a unique ID. (We can give it on instantiation)
            // Last point through Robot::getCurrentID() and latest through Robot::getCurrentTarget().
            // Map_Structure has a vector of lines.
            current_locations.push_back(std::make_pair(false, other_robot.getCurrentID().getId()));
        }
    }

    //Get the

    //Get other robots plans.


    return "";
}

*/

std::string Robot::createDynamicJson(std::vector<Robot> &robots, Robot &robot, bool stations) {
    nlohmann::json jsonObj;
    if(stations){
        jsonObj["next_station"] = remainingStations[1].getId();
        jsonObj["next_waypoint"] = remainingStations[0].getId();
    }
    else {
        jsonObj["next_station"] = remainingStations[0].getId();
        jsonObj["next_waypoint"] = remainingWaypoints[0].getId();
    }

    std::vector<nlohmann::json> visitedWayPoints;
    jsonObj["visited_waypoints"] = visitedWayPoints;
    std::vector<nlohmann::json> stationsToVisit;

    for (long unsigned i = 1; i < remainingStations.size();i++) {
        stationsToVisit.push_back(remainingStations[i].getId());
    }

    jsonObj["station_eta"] = getEta();
    jsonObj["stations_to_visit"] = stationsToVisit;
    otherRobotsInf.clear();

    for (auto& other_robot : robots) {
        if (other_robot.getStatus() != Status::available && other_robot != robot) {
            std::vector<Point> p;
            argos::Real dist = argos::Distance(getRemainingWaypoints().front(),
                                               getfootBot()->GetEmbodiedEntity().GetOriginAnchor().Position);
            dist = dist / VELOCITY *100;
            if(stopWatch != -1) dist = dist + ((stationDelay-stopWatch)/10* VELOCITY /100);
            auto time_result = getEtaNextRobot(other_robot, dist);
            if(time_result != nullptr){
                otherRobotsInf.push_back(time_result);
                nlohmann::json outerInfMap;
                nlohmann::json infMap;
                infMap["eta"] = time_result->distance/ VELOCITY *100;// convert from distance to time
                infMap["id"] = other_robot.getfootBot()->GetId();
                nlohmann::json loc;
                loc["x"] = time_result->currPosition.GetX();
                loc["y"] = time_result->currPosition.GetY();
                infMap["location"] = loc;
                std::vector<int> ids;
                for (long unsigned j = time_result->stationsPassed; j < other_robot.getRemainingStations().size(); j++) {
                    ids.push_back(other_robot.getRemainingStations()[j].getId());
                }
                infMap["station_plan"] = ids;
                std::vector<nlohmann::json> wayPointL;
                for (long unsigned j = 0; j < time_result->waypointsToPass.size(); j++) {
                    nlohmann::json waypoint;
                    waypoint["type"] = "Waypoint";
                    waypoint["value"] = time_result->waypointsToPass[j].getId();
                    wayPointL.push_back(waypoint);
                }

                infMap["waypoint_plan"] = wayPointL;
                jsonObj["robot_info_map"][other_robot.getfootBot()->GetId()] = infMap;
            }
        }
    }
    Map_Structure sMap = Map_Structure::get_instance();
    std::ofstream out(sMap.folderPath + "/" + footBot->GetId() + "/" +
                      "dynamic_config.json");
    out << std::setw(4) << jsonObj;
    return jsonObj.dump();
}


void Robot::sortJob(std::vector<std::vector<float>> shortestDistances)
{
    for(long unsigned i=0;i<job.size()-1;i++)
    {
        int k=i;
        float min = INF;
        for(long unsigned j=i;j<job.size()-1;j++)
        {
            float temp;
            if(i == 0){
                temp =shortestDistances[currentPositionId->getId()][job[j].getId()];
            }else temp=shortestDistances[job[i-1].getId()][job[j].getId()];

            if(temp<min){
                min = temp;
                k = j;
            }
        }
        std::swap(job[i], job[k]);
    }
    for(auto& j : job)
        remainingStations.push_back(j);

}