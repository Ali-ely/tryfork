//
//  ContentView.swift
//  TestMap
//


import SwiftUI
import MapKit

struct ContentView: View {
    @StateObject private var vm = GraphViewModel()

    // UI fields
    @State private var startText = ""
    @State private var endText = ""

    // Map state
    @State private var routeCoords: [CLLocationCoordinate2D] = []
    @State private var mapCenter = CLLocationCoordinate2D(latitude: 30.0190, longitude: 31.4997)
    @State private var mapResetTrigger = UUID()        // NEW: forces map reset

    // Node annotations
    @State private var showNamedNodes: Bool = false

    // Location manager
    @StateObject private var locationManager = LocationManager()
    @State private var liveLocationOn: Bool = false

    // Nearest node info
    @State private var lastNearest: (id: String, name: String?, distanceMeters: Double)? = nil
    private let distanceThresholdMeters: Double = 100.0

    // Debounce + timers
    @State private var debounceTask: Task<Void, Never>? = nil
    private let debounceIntervalMs: UInt64 = 600

    @State private var pendingOneShotStart: Bool = false

    @State private var transientStatusClearTask: Task<Void, Never>? = nil
    @State private var oneShotTimeoutTask: Task<Void, Never>? = nil
    private let oneShotTimeoutSeconds: UInt64 = 5

    var body: some View {
        NavigationView {
            VStack(spacing: 12) {

                //---------------------------------------------------------
                // Header
                //---------------------------------------------------------
                header

                //---------------------------------------------------------
                // Text fields + search
                //---------------------------------------------------------
                VStack(spacing: 10) {

                    HStack(spacing: 8) {
                        AutocompleteTextField(
                            text: $startText,
                            placeholder: "Start (name or id)",
                            suggestions: { vm.suggestions(for: $0.first ?? "") }
                        ) { _ in computeRouteIfPossible() }
                        .frame(maxWidth: .infinity)

                        Button(action: { oneShotStartButtonTapped() }) {
                            Image(systemName: "location.fill")
                                .padding(8)
                                .background(RoundedRectangle(cornerRadius: 8)
                                    .stroke(Color.secondary, lineWidth: 1))
                        }
                        .help("Use nearest node to my location")
                    }

                    AutocompleteTextField(
                        text: $endText,
                        placeholder: "End (name or id)",
                        suggestions: { vm.suggestions(for: $0.first ?? "") }
                    ) { _ in computeRouteIfPossible() }

                    HStack(spacing: 12) {
                        Button(action: { computeRouteIfPossible(force: true) }) {
                            Label("Go", systemImage: "arrow.right.circle.fill")
                                .font(.headline)
                                .frame(maxWidth: 120)
                                .padding(.vertical, 10)
                                .background(RoundedRectangle(cornerRadius: 10).fill(Color.accentColor))
                                .foregroundColor(.white)
                        }

                        Button(action: {
                            cancelDebounce()
                            cancelOneShotTimeout()
                            routeCoords = []
                            vm.status = "Cleared."
                            vm.persistentRouteStatus = nil
                        }) {
                            Label("Clear", systemImage: "trash")
                                .padding(.vertical, 10)
                                .padding(.horizontal, 12)
                                .background(RoundedRectangle(cornerRadius: 10)
                                    .stroke(Color.secondary, lineWidth: 1))
                        }

                        Spacer()
                    }
                    .padding(.horizontal)

                    //-----------------------------------------------------
                    // Toggles
                    //-----------------------------------------------------
                    VStack(alignment: .leading, spacing: 10) {

                        Toggle(isOn: $liveLocationOn) {
                            VStack(alignment: .leading) {
                                Text("Live location").font(.subheadline)
                                Text("Continuously update start to nearest node")
                                    .font(.caption2)
                                    .foregroundColor(.secondary)
                            }
                        }
                        .onChange(of: liveLocationOn) { _, enabled in
                            if enabled {
                                switch locationManager.authorizationStatus {
                                case .notDetermined:
                                    locationManager.requestPermission()
                                case .authorizedWhenInUse, .authorizedAlways:
                                    locationManager.startUpdating()
                                case .denied, .restricted:
                                    showTransientStatus("Location permission denied — allow in Settings")
                                    liveLocationOn = false
                                default:
                                    locationManager.requestPermission()
                                }
                            } else {
                                // Stop live updates but keep blue dot
                                locationManager.stopUpdating()
                                vm.status = ""
                                cancelDebounce()
                                lastNearest = nil
                            }
                        }

                        Toggle(isOn: $showNamedNodes) {
                            VStack(alignment: .leading) {
                                Text("Show named nodes").font(.subheadline)
                                Text("Toggle pins for buildings/gates")
                                    .font(.caption2).foregroundColor(.secondary)
                            }
                        }
                    }
                    .padding(.horizontal)
                }
                .padding(.horizontal)

                //---------------------------------------------------------
                // Status or nearest
                //---------------------------------------------------------
                if let nearest = lastNearest,
                   nearest.distanceMeters > distanceThresholdMeters {
                    HStack(spacing: 12) {
                        Image(systemName: "exclamationmark.triangle.fill")
                            .foregroundColor(.white)
                            .padding(8)
                            .background(Circle().fill(Color.orange))

                        VStack(alignment: .leading) {
                            Text("You're far from mapped nodes").bold()
                            Text("Nearest \(nearest.name ?? nearest.id) — \(Int(nearest.distanceMeters)) m")
                                .foregroundColor(.secondary)
                                .font(.caption)
                        }
                        Spacer()
                    }
                    .padding(10)
                    .background(RoundedRectangle(cornerRadius: 12)
                        .fill(Color(.systemBackground)))
                    .overlay(RoundedRectangle(cornerRadius: 12)
                        .stroke(Color(.systemGray4)))
                    .padding(.horizontal)

                } else {
                    Text(vm.displayStatus)
                        .font(.caption)
                        .foregroundColor(.secondary)
                        .padding(.horizontal)
                }

                //---------------------------------------------------------
                // Map
                //---------------------------------------------------------
                MapView(route: $routeCoords,
                        center: $mapCenter,
                        namedNodes: showNamedNodes ? vm.nodesById : [:],
                        resetTrigger: mapResetTrigger,
                        highlightedNodeId: lastNearest?.id)
                    .cornerRadius(12)
                    .padding([.horizontal, .bottom])
                    .animation(.default, value: showNamedNodes)
            }

            //-------------------------------------------------------------
            // Navigation bar
            //-------------------------------------------------------------
            .navigationTitle("AUC Navigator")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button(action: resetView) {
                        Image(systemName: "location.north.line")
                    }
                }
            }

            //-------------------------------------------------------------
            // Location update handler (only when live or one-shot)
            //-------------------------------------------------------------
            .onReceive(locationManager.$location) { newCoord in
                if newCoord != nil { cancelOneShotTimeout() }

                guard let coord = newCoord else {
                    lastNearest = nil
                    return
                }
                guard pendingOneShotStart || liveLocationOn else { return }

                guard let nearest = vm.nearestNode(to: coord) else {
                    lastNearest = nil
                    vm.status = "No graph loaded."
                    cancelDebounce()
                    pendingOneShotStart = false
                    return
                }

                // One-shot logic
                if pendingOneShotStart {
                    pendingOneShotStart = false
                    cancelDebounce()

                    if nearest.distanceMeters <= distanceThresholdMeters {
                        let chosen = nearest.name?.isEmpty == false ? nearest.name! : nearest.id
                        startText = chosen
                        vm.status = "Set start to \(nearest.id) (\(Int(nearest.distanceMeters)) m)"
                        if !endText.isEmpty { computeRouteIfPossible() }
                        lastNearest = nil
                    } else {
                        showTransientStatus("Too far from mapped nodes (\(Int(nearest.distanceMeters)) m)")
                    }
                    return
                }

                // Live location flow
                lastNearest = (nearest.id, nearest.name, nearest.distanceMeters)

                if nearest.distanceMeters <= distanceThresholdMeters {
                    scheduleDebouncedAutoRoute(coord: coord, nearest: nearest)
                    mapCenter = coord
                } else {
                    vm.status = ""
                    cancelDebounce()
                    mapCenter = coord
                }
            }

            .onDisappear {
                cancelDebounce()
                cancelOneShotTimeout()
                locationManager.stopUpdating()
            }
        }
    }

    // MARK: - Reset View (FULL reset except text fields)
    private func resetView() {
        cancelDebounce()
        cancelOneShotTimeout()

        // Turn off live-location & node pins
        liveLocationOn = false
        showNamedNodes = false
        locationManager.stopUpdating()

        // Reset map
        routeCoords = []
        mapCenter = CLLocationCoordinate2D(latitude: 30.0190, longitude: 31.4997)
        lastNearest = nil

        // Reset statuses
        vm.status = ""
        vm.persistentRouteStatus = nil

        // Do NOT clear startText or endText
        // Force MapView to reset zoom/pan
        mapResetTrigger = UUID()
    }

    // MARK: - Location helper functions
    private func oneShotStartButtonTapped() {
        if locationManager.authorizationStatus == .denied ||
            locationManager.authorizationStatus == .restricted {
            showTransientStatus("Location permission denied")
            return
        }

        if let coord = locationManager.location {
            handleOneShotLocation(coord)
            return
        }

        pendingOneShotStart = true

        switch locationManager.authorizationStatus {
        case .notDetermined:
            locationManager.requestPermission()
            startOneShotTimeout()

        case .authorizedWhenInUse, .authorizedAlways:
            locationManager.requestLocationOnce()
            startOneShotTimeout()

        default:
            locationManager.requestPermission()
            startOneShotTimeout()
        }
    }

    private func handleOneShotLocation(_ coord: CLLocationCoordinate2D) {
        guard let nearest = vm.nearestNode(to: coord) else {
            showTransientStatus("No graph loaded.")
            return
        }
        if nearest.distanceMeters <= distanceThresholdMeters {
            let chosen = nearest.name?.isEmpty == false ? nearest.name! : nearest.id
            startText = chosen
            vm.status = "Set start to \(nearest.id)"
            if !endText.isEmpty { computeRouteIfPossible() }
        } else {
            showTransientStatus("Too far (\(Int(nearest.distanceMeters)) m)")
        }
    }

    private func startOneShotTimeout() {
        cancelOneShotTimeout()
        oneShotTimeoutTask = Task { @MainActor in
            try? await Task.sleep(nanoseconds: oneShotTimeoutSeconds * 1_000_000_000)
            if pendingOneShotStart {
                pendingOneShotStart = false
                showTransientStatus("Location timeout")
            }
            oneShotTimeoutTask = nil
        }
    }

    private func cancelOneShotTimeout() {
        oneShotTimeoutTask?.cancel()
        oneShotTimeoutTask = nil
    }

    // MARK: - Debounce logic
    private func scheduleDebouncedAutoRoute(coord: CLLocationCoordinate2D,
                                            nearest: (id: String, name: String?, distanceMeters: Double)) {
        cancelDebounce()
        debounceTask = Task.detached { [nearest, coord] in
            try? await Task.sleep(nanoseconds: debounceIntervalMs * 1_000_000)
            if Task.isCancelled { return }
            await MainActor.run {
                guard liveLocationOn else { return }
                if let last = lastNearest, last.id == nearest.id {
                    let chosen = nearest.name?.isEmpty == false ? nearest.name! : nearest.id
                    if startText != chosen {
                        startText = chosen
                        vm.status = "Auto-selected \(nearest.id)"
                    }
                    if !endText.isEmpty { computeRouteIfPossible() }
                    mapCenter = coord
                }
            }
        }
    }

    private func cancelDebounce() {
        debounceTask?.cancel()
        debounceTask = nil
    }

    // MARK: - Transient status
    private func showTransientStatus(_ text: String,
                                     duration: TimeInterval = 2.0) {
        transientStatusClearTask?.cancel()
        vm.status = text
        transientStatusClearTask = Task { @MainActor in
            try? await Task.sleep(nanoseconds: UInt64(duration * 1_000_000_000))
            vm.status = ""
            transientStatusClearTask = nil
        }
    }

    // MARK: - Routing
    private func computeRouteIfPossible(force: Bool = false) {
        if let fromId = vm.findNodeId(for: startText),
           let toId = vm.findNodeId(for: endText) {
            computeRoute(fromId: fromId, toId: toId)
        } else if force {
            if startText.isEmpty || endText.isEmpty {
                showTransientStatus("Enter both start and end")
            } else {
                showTransientStatus("Unknown node(s)")
            }
        }
    }

    private func computeRoute(fromId: String, toId: String) {
        let rawPath = GraphBridge.shortestPath(from: fromId, to: toId) ?? []

        if rawPath.isEmpty {
            showTransientStatus("No path found")
            routeCoords = []
            vm.persistentRouteStatus = nil
            return
        }

        guard let geomArr = GraphBridge.geometry(forPathNodeIds: rawPath)
                as? [[String: Any]] else {
            showTransientStatus("Failed to get geometry")
            return
        }

        var coords: [CLLocationCoordinate2D] = []
        for d in geomArr {
            if let lat = d["lat"] as? Double,
               let lon = d["lon"] as? Double {
                coords.append(CLLocationCoordinate2D(latitude: lat, longitude: lon))
            }
        }

        guard !coords.isEmpty else {
            showTransientStatus("Empty geometry")
            return
        }

        routeCoords = coords
        mapCenter = coords.first!

        let msg = "Route: \(rawPath.count) nodes, \(coords.count) points"
        vm.status = msg
        vm.persistentRouteStatus = msg
    }

    // MARK: - Header view
    private var header: some View {
        HStack(spacing: 12) {
            Image("AUC")
                .resizable()
                .frame(width: 44, height: 44)
                .padding(8)
                .background(RoundedRectangle(cornerRadius: 10)
                    .fill(Color(.systemGray6)))

            VStack(alignment: .leading, spacing: 4) {
                Text("AUC Campus").font(.headline)
                Text("Find shortest walking route")
                    .foregroundColor(.secondary)
                    .font(.subheadline)
            }
            Spacer()
        }
        .padding(.horizontal)
    }
}
