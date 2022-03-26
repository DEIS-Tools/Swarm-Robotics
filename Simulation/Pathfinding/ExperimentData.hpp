#ifndef EXPERIMENT_DATA_HPP
#define EXPERIMENT_DATA_HPP

#include <memory>
#include <vector>
#include "Graph.hpp"
#include "Agent.hpp"

class ExperimentData{
public:
    static ExperimentData &get_instance() {
        static ExperimentData instance;
        return instance;
    }
    std::shared_ptr<Graph> getGraph();
    std::vector<std::shared_ptr<Agent>> getAgents();
    bool requestSolution(int agentId);
    // std::vector<AgentInfo> getAgentInfo();
private:
    std::shared_ptr<Graph> graph;
    std::vector<std::shared_ptr<Agent>> agents;
};

#endif