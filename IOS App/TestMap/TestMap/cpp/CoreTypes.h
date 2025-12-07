//
//  CoreTypes.h
//  TestMap
//

#ifndef CoreTypes_h
#define CoreTypes_h

#include <string>

struct EdgeGeomPoint {
    double lat;
    double lon;
};

struct GeomNode {
    double lat;
    double lon;
    GeomNode *next;
    GeomNode(double la = 0.0, double lo = 0.0, GeomNode *n = nullptr) : lat(la), lon(lo), next(n) {}
    ~GeomNode() = default;
};

struct PathNode {
    std::string id;
    PathNode *next;
    PathNode(const std::string &s = std::string(), PathNode *n = nullptr) : id(s), next(n) {}
};

#endif /* CoreTypes_h */
