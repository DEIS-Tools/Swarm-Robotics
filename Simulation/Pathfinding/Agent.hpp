#ifndef AGENT_HPP
#define AGENT_HPP

#include "Path.hpp"
#include "point.hpp"
#include <queue>

class Agent {
  public:
    Path getPath();
    void createPath(std::vector<Point> plan);
    void recreatePath();
  private:
    Path path;
    std::vector<Point> plan;
};

#endif