#ifndef CFootBotDiffusion_H
#define CFootBotDiffusion_H

#include "../connection/connector.hpp"
#include "models/map/map_structure.hpp"


#include "argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h"
/* Definition of the CCI_Controller class. */
#include "argos3/core/control_interface/ci_controller.h"
/* Definition of the differential steering actuator */
#include "argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h"
/* Definition of the foot-bot proximity sensor */
#include "argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h"
#include "argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h"
#include "argos3/core/simulator/loop_functions.h"
//  ???, this was added in order to get  GetID() robot
#include "argos3/plugins/robots/foot-bot/simulator/footbot_entity.h"

/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CFootBotDiffusion : public argos::CCI_Controller {

public:
    /* Class constructor. */
    CFootBotDiffusion();

    /* Class destructor. */
    virtual ~CFootBotDiffusion() {}

    /*
     * This function initializes the controller.
     * The 't_node' variable points to the <parameters> section in the XML
     * file in the <controllers><CFootBotDiffusion_controller> section.
     */
    virtual void Init(argos::TConfigurationNode &t_node);

    /*
     * This function is called once every time step.
     * The length of the time step is set in the XML file.
     */
    virtual void ControlStep();

private:
    void createUppaalTask(Robot &robot, std::string choice, int threadNr, bool stations);
    void extractUppaalTask(int n, std::string choice, int threadNr);
    void movementLogic(int n);
    void controlStep(double per, double dotProd, float velocity);
    bool lookForJob(int n);
    bool lookForJob(Robot &robot);
    void getShortestPath(int n, bool stations);
    void plotData();
    std::string m_id;
    /* Pointer to the differential steering actuator */
    argos::CCI_DifferentialSteeringActuator *m_pcWheels;
    /* Pointer to the foot-bot proximity sensor */
    argos::CCI_FootBotProximitySensor *m_pcProximity;

    argos::CCI_PositioningSensor *m_pcPosition;

    /*
     * The following variables are used as parameters for the
     * algorithm. You can set their value in the <parameters> section
     * of the XML configuration file, under the
     * <controllers><CFootBotDiffusion_controller> section.
     */

    /* Maximum tolerance for the angle between
     * the robot heading direction and
     * the closest obstacle detected. */
    argos::CDegrees m_cAlpha;
    /* Maximum tolerance for the proximity reading between
     * the robot and the closest obstacle.
     * The proximity reading is 0 when nothing is detected
     * and grows exponentially to 1 when the obstacle is
     * touching the robot.
     */
    argos::Real m_fDelta;
    /* Wheel speed. */
    argos::Real m_fWheelVelocity;
    /* Angle tolerance range to go straight.
     * It is set to [-alpha,alpha]. */
    argos::CRange<argos::CRadians> m_cGoStraightAngleRange;

    static int counter;
    Map_Structure &sMap = Map_Structure::get_instance();
};

struct arg_struct {
    std::string id;
    std::string choice;
    std::string result;
    std::string dynamic;
    std::string path;
};

#endif
