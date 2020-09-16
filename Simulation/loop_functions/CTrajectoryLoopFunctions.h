#ifndef CTrajectoryLoopFunctions_H
#define CTrajectoryLoopFunctions_H

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <argos3/plugins/simulator/entities/box_entity.h>

using namespace argos;
//using namespace std;

class CTrajectoryLoopFunctions : public CLoopFunctions {

public:
  typedef std::map<CFootBotEntity *, std::vector<CVector3>> TWaypointMap;
  typedef std::vector<CBoxEntity> BoxMap;

  TWaypointMap m_tWaypoints;
  BoxMap m_box_map;

public:
  virtual ~CTrajectoryLoopFunctions() {}

  virtual void Init(TConfigurationNode &t_tree);

  virtual void Reset();

  virtual void PostStep();

  inline const TWaypointMap &GetWaypoints() const { return m_tWaypoints; }

  inline const BoxMap &GetBoxMap() const { return m_box_map; }

private:
};

#endif