#include "line.hpp"

Line::Line(Point *a, Point *b) {
    this->a = a;
    this->b = b;
    //this->distance = (a - b).Length();
    this->distance = argos::Distance(*a, *b);
}

void Line::setFailureline() {
    if (a->getId() != b->getId()) {
        this->distance = -1;
    }
}

const std::tuple<float, float, float, float> Line::getCoordinates() {
    std::tuple<float, float, float, float> coordinates{a->GetX(), a->GetY(), b->GetX(), b->GetY()};
    return coordinates;
}

float Line::GetFloydDistance() const {
    if (distance != -1)
        return distance;
    else
        return std::numeric_limits<float>::infinity();
}

double Line::distanceToLine(const Point &point) {
    double normalLength = hypot(b->getX() - a->getX(), b->getY() - a->getY());
    double d = (point.getX() - a->getX()) * (b->getY() - a->getY()) -
               (point.getY() - a->getY()) * (b->getX() - a->getX()) / normalLength;
    return d;
}

bool Line::operator==(const Line &l) {
    return (l.a->getId() == a->getId() && l.b->getId() == b->getId()) ||
           (l.a->getId() == b->getId() && l.a->getId() == b->getId());
}