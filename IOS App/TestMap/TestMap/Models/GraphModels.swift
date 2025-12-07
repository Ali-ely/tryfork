//
//  GraphModels.swift
//  TestMap
//

import Foundation
import CoreLocation

struct Node: Codable {
    let id: String
    let lat: Double
    let lon: Double
    let name: String?
}

struct EdgePoint: Codable {
    let lat: Double
    let lon: Double
}

struct Edge: Codable {
    let id: String
    let from: String
    let to: String
    let weight: Double
    let geometry: [EdgePoint]?
}

struct GraphJSON: Codable {
    let nodes: [Node]
    let edges: [Edge]
}
