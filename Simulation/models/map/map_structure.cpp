#include "map_structure.hpp"
#include "controllers/parsing/uppaal_model_parsing.hpp"

void Map_Structure::collectAllWayPoints() {

    argos::CSpace::TMapPerType &tBotMap =
        argos::CLoopFunctions().GetSpace().GetEntitiesByType("foot-bot");
    for (auto it = tBotMap.begin(); it != tBotMap.end();
         ++it) {
        argos::CFootBotEntity *pcBot = argos::any_cast<argos::CFootBotEntity *>(it->second);
        argos::CVector3 pos = pcBot->GetEmbodiedEntity().GetOriginAnchor().Position;

        Point *p = new Point(pos, pointType::via, "S." + pcBot->GetId());
        points.push_back(*p);
        waypointsIDs.push_back(points.end()->getId());
        Robots.push_back(Robot(pcBot, p));
    }
    std::vector<Box> walls;
    argos::CSpace::TMapPerType &tBoxMap =
        argos::CLoopFunctions().GetSpace().GetEntitiesByType("box");
    for (auto& boxMap : tBoxMap) {
        argos::CBoxEntity *pcBox = argos::any_cast<argos::CBoxEntity *>(boxMap.second);
        argos::CVector3 pos = pcBox->GetEmbodiedEntity().GetOriginAnchor().Position;
        argos::CVector3 size = pcBox->GetSize();
        std::string id = boxMap.first;
        Box b{id, pos, size};
        switch (id[0]) {
            case 'w': // break; allow break in order to stop considering boundries of
                // arena
                walls.push_back(b);
                break;
            case 'c':
            case 'd':
            case 'o':
                Map_Structure::boxes.push_back(b);
                break;
        }
    }
    for (long unsigned i = 0; i < Map_Structure::boxes.size(); i++) {
        Map_Structure::boxes[i].setBoxCorner();
        for (auto j = 0; j < 4; j++) {
            Map_Structure::points.push_back(Map_Structure::boxes[i].getVCorner(j));
            Map_Structure::hardLines.push_back(Map_Structure::boxes[i].getBoxLine(j));
        }
    }
}
int Map_Structure::getRobotIdByName(std::string name) {
    for (long unsigned i = 0; i < Robots.size(); i++) {
        if (Robots[i].getfootBot()->GetId() == name) {
            return i;
        }
    }

    throw std::invalid_argument("Robot not found by NAME");
}

Robot Map_Structure::getRobotByName(std::string name) {
    for (auto& robot:  Robots) {
        if (robot.getfootBot()->GetId() == name) {
            return robot;
        }
    }

    throw std::invalid_argument("Robot not found by NAME");
}

// Set the fields `shortestDistanceMatrix`, which gives the length of the shortest path between all points
// and `shortestPath`, which tells the next Point to go to if one wants the shortest path.
void Map_Structure::setDistanceMatrix(){
    auto size = (unsigned)sqrt(Map_Structure::lines.size());

    shortestDistanceMatrix.resize(size, std::vector<float>(size));
    for(auto& line: Map_Structure::lines){
        shortestDistanceMatrix[line.Geta().getId()][line.Getb().getId()] = line.GetFloydDistance();
    }
    shortestPath.clear();
    shortestPath.resize(size, std::vector<int>(size));
    std:: cout << "Size of shortestDistanceMatrix: " << size << std::endl;
    for (unsigned i = 0; i < size; ++i) {
        for (unsigned j = 0; j < size; ++j) {
            shortestPath[i][j] = 0;
        }
        for (unsigned j = 0; j < size; ++j) {
            if (i != j) {
                shortestPath[i][j] = j + 1;
            }
        }
    }

    float inf = std::numeric_limits<float>::infinity();

    for (long unsigned k = 0; k < shortestDistanceMatrix.size(); k++) {
        for (long unsigned i = 0; i < shortestDistanceMatrix.size(); i++){
            for (long unsigned j = 0; j < shortestDistanceMatrix.size(); j++){
                float temp = shortestDistanceMatrix[i][k] + shortestDistanceMatrix[k][j];
                if (shortestDistanceMatrix[i][k]==inf || shortestDistanceMatrix[k][j]== inf)
                    temp = inf;
                if (shortestDistanceMatrix[i][k] != inf && shortestDistanceMatrix[k][j]!= inf && temp < shortestDistanceMatrix[i][j]){
                        shortestDistanceMatrix[i][j] = (shortestDistanceMatrix[i][k] + shortestDistanceMatrix[k][j]);
                        shortestPath[i][j] = shortestPath[i][k];


//                    if(i < amountOfStations && j < amountOfStations && k >= amountOfStations){
//                        shortestDistanceMatrix[i][j] = (shortestDistanceMatrix[i][k] + shortestDistanceMatrix[k][j]);
//                        shortestPath[i][j] = shortestPath[i][k];
//                    }
//                    else
//                    if(i >= amountOfStations && j >= amountOfStations){
//                        shortestDistanceMatrix[i][j] = (shortestDistanceMatrix[i][k] + shortestDistanceMatrix[k][j]);
//                        shortestPath[i][j] = shortestPath[i][k];
//                    }else {
//                        std::cout << "from "  << i << " to " << j <<"  || " << "from "  << i << " to " << k <<std::endl;
//                    }
                }
            }
        }
    }
    //DEBUG
//    for(int i = 0; i < 12; i ++){
//        for(int j = 0; j < 12; j ++){
//            if(i != j) {
//                auto result = findPath(i, j);
//                std::cout << "------Path ------" << std::endl;
//                std::cout << "from : " << getPointByID(i).getName() << std::endl;
//                for (auto &p : result) {
//                    std::cout << "ID: " << p.getName() << " -> ";
//                }
//                std::cout << "to : " << getPointByID(j).getName() << std::endl;
//            }
//        }
//    }
    //END DEBUG
}

// @Todo: Could use a refactor as this code is much more complex than the pseudo-code for
// the Floyd-Warshall Algorithm.
std::vector<std::vector<float>> Map_Structure::floydShortestOfStations() {
    std::vector<std::vector<float>> shortestDistance(amountOfStations, std::vector<float>());
    for (long unsigned i = 0; i < amountOfStations; i++) {
        for (long unsigned j = 0; j < amountOfStations; j++) {
            shortestDistance[i].push_back(shortestDistanceMatrix[i][j]);
            std::cout << "i : " << i << " | j : " << j << " | distance : "<< shortestDistanceMatrix[i][j] << std::endl;
        }
    }
    return shortestDistance;
}

Point& Map_Structure::getPointByID(int id){
    for(auto& p : points){
        if(p.getId() == id){
            return p;
        }
    }
    throw std::invalid_argument("Point not found");
}



// Combines from all the points all possibleMap_Structure::lines
void Map_Structure::setAllPossibleLines() {
    for (auto& point_a : Map_Structure::points) {
        for (long unsigned j = 0; j < Map_Structure::points.size(); j++) {
            Map_Structure::lines.push_back(
                Line(&point_a, &Map_Structure::points[j]));
        }
    }
    eliminateBadLines();
}

double cross(argos::CVector3 a, argos::CVector3 b) {
    return a.GetX() * b.GetY() - a.GetY() * b.GetX();
}

double dot(argos::CVector3 a, argos::CVector3 b) {
    return a.GetX() * b.GetX() + a.GetY() * b.GetY();
}

bool intersectionInterest(argos::CVector3 m1, argos::CVector3 m2, argos::CVector3 n1, argos::CVector3 n2) {
    if (cross((n1 - n2), (m1 - n2)) * cross((n1 - n2), (m2 - n2)) < 0 &&
        cross((m1 - m2), (n1 - m2)) * cross((m1 - m2), (n2 - m2)) < 0) {
        return true;
    }
    if (dot(m2 - n2, m2 - n1) <= 0 && dot(m1 - n2, m1 - n1) <= 0 &&
        cross(m1 - m2, n1 - n2) == 0) {
        return true;
    }
    if (cross(n1 - n2, m1 - n2) * cross(n1 - n2, m2 - n2) < 0 &&
        cross(m2 - m1, n1 - m1) * cross(m2 - m1, n2 - m1) == 0) {
        return true;
    }
    return false;
}
// Functions eliminates all theMap_Structure::lines which have intersection
void Map_Structure::eliminateBadLines() {
    std::vector<int> adjIDs;
    for (long unsigned i = 0; i < Map_Structure::lines.size(); i++) {
        for (long unsigned j = 0; j < Map_Structure::hardLines.size(); j++) {
            if (intersectionInterest(Map_Structure::lines[i].Geta(),
                                     Map_Structure::lines[i].Getb(),
                                     Map_Structure::hardLines[j].Geta(),
                                     Map_Structure::hardLines[j].Getb())) {
                Map_Structure::lines[i].setFailureline();
                break;
            }
        }
        if (i != 0) {
            if (Map_Structure::lines[i].GetDistance() != -1) {
                if (Map_Structure::lines[i].Geta().getId() ==
                    Map_Structure::lines[i - 1].Geta().getId())
                    adjIDs.push_back(Map_Structure::lines[i].Getb().getId());
                else {

                    Map_Structure::lines[i].Geta().setAdjIDs(adjIDs);
                    adjIDs.clear();
                    adjIDs.push_back(Map_Structure::lines[i].Getb().getId());
                }
            }
        } else {
            adjIDs.push_back(Map_Structure::lines[i].Getb().getId());
        }
    }

    //Checks if any of the lines intersect with the virtual lines
    for(auto& line : lines){
        //TODO move to previous loop ? Need map visualisation working
        for(auto& box : boxes){
            for(auto& vLines : box.getVirtualLines()){
                if (box.isPointInShape(vLines.Geta()) || box.isPointInShape(vLines.Getb())){
                    std::cout << "inside the box " <<std::endl;
                    continue;
                }

                if(line.Geta().getId() != vLines.Geta().getId() && line.Getb().getId() != vLines.Getb().getId()) {
                    if (intersectionInterest(line.Geta(), line.Getb(), vLines.Geta(), vLines.Getb())) {
                        line.setFailureline();
                        break;
                    }
                }
            }
        }

        if(line.Geta().getName() == "P0" && line.Getb().getName() == "P5"){
            std::cout << "P0 - P5 Distance : " << line.GetDistance() <<std::endl;
        }
        if(line.Geta().getName() == "P4" && line.Getb().getName() == "Ob1Vc2"){
            std::cout << "P4 - Ob1Vc2 Distance : " << line.GetDistance() <<std::endl;
        }
        if(line.Geta().getName() == "P4" && line.Getb().getName() == "P9"){
            std::cout << "P4 - P9 Distance : " << line.GetDistance() <<std::endl;
        }
        if(line.Geta().getName() == "P1" && line.Getb().getName() == "P0"){
            std::cout << "P1 - P0 Distance : " << line.GetDistance() <<std::endl;
        }
        if(line.Geta().getName() == "P1" && line.Getb().getName() == "Ob1Vc3"){
            std::cout << "P1 - Ob1Vc3 Distance : " << line.GetDistance() <<std::endl;
        }
        if(line.Geta().getName() == "P9" && line.Getb().getName() == "P5"){
            std::cout << "P9 - P5 Distance : " << line.GetDistance() <<std::endl;
        }
        if(line.Geta().getName() == "P9" && line.Getb().getName() == "P6"){
            std::cout << "P9 - P6 Distance : " << line.GetDistance() <<std::endl;
        }
    }
}

std::vector<Point> Map_Structure::findPath(int startId, int destinationId) {
    std::vector<Point> pts;
    if(startId == destinationId){
        return pts;
    }
    auto u = startId + 1;
    auto v = destinationId + 1;
    do {
        u = shortestPath[u - 1][v - 1];
        pts.push_back(getPointByID(u-1));
    } while (u != v);
    return pts;
}

void Map_Structure::initializeStations() {
    // get all the points defined in JSON file
    std::cout << folderPath +"points.json" <<std::endl;
    std::ifstream i(folderPath +"points.json");

    nlohmann::json j = nlohmann::json::parse(i);
    for (long unsigned i = 0; i < j.size(); i++) {
        if(j[i].value("x", 0.0)!= 0.0){
            Point p = Point(
                argos::CVector3(j[i].value("x", 0.0), j[i].value("y", 0.0), j[i].value("z", 0.0)),
                static_cast<pointType>(j[i].value("type", 0)), j[i].value("name", ""));

            points.push_back(p);
        }
        if (static_cast<pointType>(j[i].value("type", 0)) == pointType::station)
            stationIDs.push_back(points[i].getId());
        if (static_cast<pointType>(j[i].value("type", 0)) == pointType::endpoint)
            endStationIDs.push_back(points[i].getId());
    }
    amountOfStations = stationIDs.size() + endStationIDs.size();
}

void Map_Structure::createFolderForEachRobot() {
    for (long unsigned i = 0; i < Robots.size(); i++) {

        std::string temp = folderPath + Robots[i].getfootBot()->GetId();

        if (mkdir(temp.c_str(), 0777) == -1) {
        }
    }
}

void Map_Structure::setFolderPath() {
    std::string fileLoc = argos::CSimulator::GetInstance().GetExperimentFileName();
    if(fileLoc.find("/") != std::string::npos) {
        auto pos = fileLoc.find_last_of("/");
        folderPath = fileLoc.substr(0 , pos + 1);
    }
    else {
        folderPath = "/";
    }
    std::cout << folderPath <<std::endl;
}

int Map_Structure::getAmountOfStations() {return (int)amountOfStations;}