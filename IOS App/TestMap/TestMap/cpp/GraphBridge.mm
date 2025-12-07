//
//  GraphBridge.mm
//  TestMap
//

#import "GraphBridge.h"
#import "Graph.hpp"
#import "ShortestPath.hpp"
#import "CoreTypes.h"
#import <Foundation/Foundation.h>
#include <cmath>

// Global instance of the custom C++ Graph
static Graph *g_graph = nullptr;

// Helper: build linked geometry list from NSArray of point dictionaries.
// Returns head (caller must manage lifetime; caller is responsible to free)
static GeomNode* geomListFromNSArray(NSArray *geomArr) {
    if (!geomArr || ![geomArr isKindOfClass:[NSArray class]] || geomArr.count == 0) return nullptr;
    GeomNode *head = nullptr;
    GeomNode *tail = nullptr;
    for (id item in geomArr) {
        if (![item isKindOfClass:[NSDictionary class]]) continue;
        NSDictionary *pt = (NSDictionary *)item;
        double lat = [pt[@"lat"] doubleValue];
        double lon = [pt[@"lon"] doubleValue];
        GeomNode *n = new GeomNode(lat, lon, nullptr);
        if (!head) {
            head = n;
            tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
    }
    return head;
}

// Helper: free GeomNode list
static void freeGeomList(GeomNode *head) {
    GeomNode *it = head;
    while (it) {
        GeomNode *tmp = it;
        it = it->next;
        delete tmp;
    }
}

// Helper: free PathNode linked list
static void freePathNodes(PathNode *head) {
    PathNode *it = head;
    while (it) {
        PathNode *tmp = it;
        it = it->next;
        delete tmp;
    }
}

@implementation GraphBridge

+ (BOOL)loadGraphFromBundleJSON:(NSString *)bundleFilename error:(NSError **)error {
    if (!bundleFilename) return NO;
    NSString *nameWithoutExt = [bundleFilename stringByDeletingPathExtension];
    NSString *ext = [bundleFilename pathExtension];
    NSString *path = nil;
    if (ext.length > 0) {
        path = [[NSBundle mainBundle] pathForResource:nameWithoutExt ofType:ext];
    } else {
        path = [[NSBundle mainBundle] pathForResource:bundleFilename ofType:nil];
    }
    if (!path) return NO;
    return [self loadGraphFromFilePath:path error:error];
}

+ (BOOL)loadGraphFromFilePath:(NSString *)filePath error:(NSError **)error {
    if (!filePath) return NO;
    NSData *data = [NSData dataWithContentsOfFile:filePath];
    if (!data) return NO;

    NSError *jsonErr = nil;
    NSDictionary *root = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonErr];
    if (!root || jsonErr) {
        if (error) *error = jsonErr;
        return NO;
    }

    NSArray *nodes = root[@"nodes"];
    NSArray *edges = root[@"edges"];

    if (g_graph) { delete g_graph; g_graph = nullptr; }
    g_graph = new Graph();

    // 1. Add Nodes (id + name)
    if (nodes && [nodes isKindOfClass:[NSArray class]]) {
        for (id obj in nodes) {
            if (![obj isKindOfClass:[NSDictionary class]]) continue;
            NSDictionary *n = (NSDictionary *)obj;
            NSString *nid = n[@"id"];
            if (!nid) continue;
            double lat = [n[@"lat"] doubleValue];
            double lon = [n[@"lon"] doubleValue];
            NSString *name = n[@"name"];
            if (!name || name.length == 0) name = nid;
            g_graph->addVertex([nid UTF8String], [name UTF8String], lat, lon);
        }
    }

    // 2. Add Edges (with geometry linked lists)
    if (edges && [edges isKindOfClass:[NSArray class]]) {
        for (id obj in edges) {
            if (![obj isKindOfClass:[NSDictionary class]]) continue;
            NSDictionary *e = (NSDictionary *)obj;
            NSString *eid = e[@"id"];
            NSString *from = e[@"from"];
            NSString *to = e[@"to"];
            if (!from || !to) continue;
            double weight = [e[@"weight"] doubleValue];

            NSArray *geomArr = e[@"geometry"];
            GeomNode *geomHead = geomListFromNSArray(geomArr);

            // Resolve from/to into actual vertex IDs (strings owned here)
            std::string fromIdStr = std::string([from UTF8String]);
            if (g_graph->findVertexNode(fromIdStr.c_str()) == nullptr) {
                std::string resolved = g_graph->findIdByName(fromIdStr);
                if (!resolved.empty()) fromIdStr = resolved;
            }

            std::string toIdStr = std::string([to UTF8String]);
            if (g_graph->findVertexNode(toIdStr.c_str()) == nullptr) {
                std::string resolved = g_graph->findIdByName(toIdStr);
                if (!resolved.empty()) toIdStr = resolved;
            }

            if (!fromIdStr.empty() && !toIdStr.empty() &&
                g_graph->findVertexNode(fromIdStr.c_str()) &&
                g_graph->findVertexNode(toIdStr.c_str())) {

                std::string edgeIdStr = eid ? std::string([eid UTF8String]) : std::string();
                // Graph::addEdge will clone and then we free geomHead here
                g_graph->addEdge(fromIdStr, toIdStr, weight, edgeIdStr, geomHead);
                // Graph::addEdge clones and frees the passed geomHead (per our implementation),
                // so nothing to free here.
            } else {
                // couldn't resolve endpoints; free geomHead to avoid leak
                freeGeomList(geomHead);
            }
        }
    }

    return YES;
}

+ (nullable NSString *)findNodeIdByName:(NSString *)name {
    if (!g_graph || !name) return nil;
    std::string query = std::string([name UTF8String]);
    std::string resultId = g_graph->findIdByName(query);
    if (resultId.empty()) return nil;
    return [NSString stringWithUTF8String:resultId.c_str()];
}

+ (nullable NSArray<NSString *> *)shortestPathFrom:(NSString *)nodeIdFrom to:(NSString *)nodeIdTo {
    if (!g_graph || !nodeIdFrom || !nodeIdTo) return nil;

    ShortestPath solver;
    PathResult result;

    bool found = solver.compute(*g_graph, [nodeIdFrom UTF8String], [nodeIdTo UTF8String], result);

    if (!found || !result.reachable || !result.head) {
        // free any path nodes if created by solver in failed scenarios
        if (result.head) freePathNodes(result.head);
        return nil;
    }

    // Convert PathNode linked list to NSArray<NSString *>
    NSMutableArray *arr = [NSMutableArray array];
    PathNode *it = result.head;
    while (it) {
        [arr addObject:[NSString stringWithUTF8String:it->id.c_str()]];
        it = it->next;
    }

    // free result.pathNodes now (we returned NSArray) to avoid leaks
    freePathNodes(result.head);
    result.head = nullptr;

    return arr;
}

+ (nullable NSArray<NSDictionary *> *)geometryForPathNodeIds:(NSArray<NSString *> *)path {
    if (!g_graph || path.count < 2) return @[];

    NSMutableArray *fullPolyline = [NSMutableArray array];

    for (NSUInteger i = 0; i < path.count - 1; ++i) {
        NSString *n1 = path[i];
        NSString *n2 = path[i + 1];

        const GeomNode *geom = g_graph->getEdgeGeometry([n1 UTF8String], [n2 UTF8String]);

        if (geom) {
            // append all geom points
            const GeomNode *it = geom;
            while (it) {
                [fullPolyline addObject:@{@"lat": @(it->lat), @"lon": @(it->lon)}];
                it = it->next;
            }
        } else {
            // fallback: straight line between node coordinates; avoid duplicating points
            VertexNode *v1 = g_graph->findVertexNode([n1 UTF8String]);
            VertexNode *v2 = g_graph->findVertexNode([n2 UTF8String]);
            if (v1 && v2) {
                NSDictionary *pt1 = @{@"lat": @(v1->lat), @"lon": @(v1->lon)};
                NSDictionary *pt2 = @{@"lat": @(v2->lat), @"lon": @(v2->lon)};

                if (fullPolyline.count == 0) {
                    [fullPolyline addObject:pt1];
                } else {
                    NSDictionary *last = fullPolyline.lastObject;
                    BOOL sameAsLast = (fabs([last[@"lat"] doubleValue] - v1->lat) < 1e-12 &&
                                       fabs([last[@"lon"] doubleValue] - v1->lon) < 1e-12);
                    if (!sameAsLast) [fullPolyline addObject:pt1];
                }
                [fullPolyline addObject:pt2];
            }
        }
    }

    return fullPolyline;
}

@end
