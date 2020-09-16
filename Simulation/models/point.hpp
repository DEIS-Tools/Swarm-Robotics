#ifndef POINT
#define POINT

#include "nlohmann/json.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <argos3/core/utility/math/vector3.h>

//using namespace std;
using namespace argos;

enum Type { via, endpoint, station, realCorner, cStation };

class Point : public CVector3 {
  unsigned int id;
  Type pType;
  static unsigned int id_counter;
  std::vector<int> adjIDs;
  std::string name;
  //boolean used to see if anyone is occupying this station atm
  bool occupied = false;

public:
  Point(float x, float y, float z, Type type, std::string name);
  Point(CVector3 c, Type type, std::string name);
  ~Point();
  Point(Point && p);
  Point& operator=(Point const& obj);
  Point( const Point &obj);
  Point();
  //returns the type of a point, for more info check enum Type
  int getType() const { return pType; }
  std::string getName() const { return name; }
  int getId() const { return id; }
  void setAdjIDs(std::vector<int> adjID);
  void pushAdjID(int adjID);
  static void resetIdCount();
  bool isOccupied(){return occupied;}
  void setOccupied(bool occupied){this->occupied = occupied;}
  std::vector<int> getAdjIDs() const { return adjIDs; }
  //returns the cVector3 argos class
  CVector3 getCVector() {
    return CVector3(this->GetX(), this->GetY(), this->GetZ());
  }
};

#endif