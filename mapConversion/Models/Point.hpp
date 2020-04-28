#ifndef POINT
#define POINT

#include <sys/stat.h>
#include <sys/types.h>
#include <math.h> 
#include <vector>
#include <string>
#include <memory>
#include <bits/stdc++.h> 

class Figure;

enum Type { via, endpoint, station, realCorner, cStation, center };
class Point{
  unsigned int id;
  float x,y,z;
  Type pType;
  static unsigned int id_counter;
  std::vector<int> adjIDs;
  std::string name;
  std::shared_ptr<Figure> fig;

public:
  Point(float x, float y, float z, Type type, std::string name, std::shared_ptr<Figure> fig);
  Point(Point && p);
  Point& operator=(Point const& obj);
  Point( const Point &obj);
  Point();
  int getType() const { return pType; }
  std::string getName() const { return name; }
  int getId() const { return id; }
  void setAdjIDs(std::vector<int> adjID);
  void pushAdjID(int adjID);
  static void resetIdCount();
  // returns vias which can be accessed directly from this point
  std::vector<int> getAdjIDs() const { return adjIDs; }
  float getX(){return x;}
  float getY(){return y;}
  float getZ(){return z;}
  //return parent figure, to which this point belongs 
  //(if station is within figure offset it is consired as part of a figure)
  std::shared_ptr<Figure> getFigure(){return fig;}
  void setX(float x){this->x = x;}
  void setY(float y){this->y = y;}
  void setParent(std::shared_ptr<Figure> fig){this->fig.swap(fig);}
  void incrementId(){id = id_counter; id_counter++;}
  inline Point operator-(const Point& point) const {
    Point pResult(*this);
    pResult -= point;
    return pResult;
  }
  inline Point& operator-=(const Point& point) {
    x -= point.x;
    y -= point.y;
    z -= point.z;
    return *this;
  }
};
#endif