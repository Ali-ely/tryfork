//
//  GraphViewModel.swift
//  TestMap
//

import Foundation
import CoreLocation
import SwiftUI
import Combine

@MainActor
final class GraphViewModel: ObservableObject {
    @Published var graph: GraphJSON?
    @Published var status: String = "Loading graph..."   // transient status (overrides everything)
    @Published var nodeNameIndex: [String] = []
    @Published var nodesById: [String: Node] = [:]

    /// Persistent route status (set when a route is computed). Used as a fallback when `status` is empty.
    @Published var persistentRouteStatus: String? = nil

    /// Computed display status:
    /// 1) If `status` (transient) is non-empty -> show it.
    /// 2) Else if there is a persistent route status -> show that.
    /// 3) Else fallback to "Graph loaded..." (or Graph not loaded.)
    var displayStatus: String {
        let trimmed = status.trimmingCharacters(in: .whitespacesAndNewlines)
        if !trimmed.isEmpty {
            return trimmed
        }
        if let route = persistentRouteStatus, !route.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty {
            return route
        }
        if let g = graph {
            return "Graph loaded. Nodes: \(g.nodes.count), Edges: \(g.edges.count)"
        } else {
            return "Graph not loaded."
        }
    }

    init() {
        Task { await loadGraphJSON() }
    }

    func loadGraphJSON() async {
        // Try bundle resource first (production)
        if let url = Bundle.main.url(forResource: "auc_graph_with_names", withExtension: "json") {
            do {
                let data = try Data(contentsOf: url)
                let g = try JSONDecoder().decode(GraphJSON.self, from: data)
                await applyLoadedGraph(g)
                do {
                    try GraphBridge.loadGraph(fromBundleJSON: "auc_graph_with_names.json")
                } catch {
                    // keep status but continue
                    status = "Graph loaded but C++ bridge failed: \(error.localizedDescription)"
                }
                return
            } catch {
                status = "Parse error (bundle): \(error.localizedDescription)"
                return
            }
        }

        // No fallback path: expect the graph to be bundled with the app.
        status = "Graph file not found in bundle."
    }

    private func applyLoadedGraph(_ g: GraphJSON) async {
        self.graph = g

        var names: [String] = []
        var byId: [String: Node] = [:]
        for n in g.nodes {
            byId[n.id] = n
            if let nm = n.name, !nm.trimmingCharacters(in: .whitespaces).isEmpty {
                names.append(nm)
            } else {
                names.append(n.id)
            }
        }
        self.nodeNameIndex = names.sorted { $0.localizedCaseInsensitiveCompare($1) == .orderedAscending }
        self.nodesById = byId
        self.status = "Graph loaded. Nodes: \(g.nodes.count), Edges: \(g.edges.count)"
    }

    func suggestions(for text: String, limit: Int = 8) -> [String] {
        let q = text.trimmingCharacters(in: .whitespacesAndNewlines)
        if q.isEmpty { return [] }
        let lower = q.lowercased()

        var res: [String] = nodeNameIndex.filter { $0.lowercased().hasPrefix(lower) }
        if res.count < limit {
            let additional = nodeNameIndex.filter { $0.lowercased().contains(lower) && !res.contains($0) }
            res.append(contentsOf: additional)
        }
        if res.count > limit { res = Array(res.prefix(limit)) }
        return res
    }

    func findNodeId(for text: String) -> String? {
        guard let g = graph else { return nil }
        let trimmed = text.trimmingCharacters(in: .whitespacesAndNewlines)
        if trimmed.isEmpty { return nil }

        if g.nodes.contains(where: { $0.id == trimmed }) { return trimmed }
        if let n = g.nodes.first(where: { $0.name?.lowercased() == trimmed.lowercased() }) { return n.id }
        if let n = g.nodes.first(where: { ($0.name ?? "").lowercased().hasPrefix(trimmed.lowercased()) }) { return n.id }

        // ask C++ bridge
        if let found = GraphBridge.findNodeId(byName: trimmed) {
            return found
        }
        return nil
    }

    func nearestNode(to coord: CLLocationCoordinate2D) -> (id: String, name: String?, distanceMeters: Double)? {
        guard let g = graph, !g.nodes.isEmpty else { return nil }

        func haversineMeters(_ lat1: Double, _ lon1: Double, _ lat2: Double, _ lon2: Double) -> Double {
            let R = 6_371_000.0
            let φ1 = lat1 * .pi / 180.0
            let φ2 = lat2 * .pi / 180.0
            let Δφ = (lat2 - lat1) * .pi / 180.0
            let Δλ = (lon2 - lon1) * .pi / 180.0
            let a = sin(Δφ/2) * sin(Δφ/2) + cos(φ1) * cos(φ2) * sin(Δλ/2) * sin(Δλ/2)
            let c = 2 * atan2(sqrt(a), sqrt(max(0, 1-a)))
            return R * c
        }

        var best: (id: String, name: String?, d: Double)? = nil
        for node in g.nodes {
            let d = haversineMeters(coord.latitude, coord.longitude, node.lat, node.lon)
            if best == nil || d < best!.d {
                best = (node.id, node.name, d)
            }
        }
        if let b = best { return (b.id, b.name, b.d) }
        return nil
    }
}
