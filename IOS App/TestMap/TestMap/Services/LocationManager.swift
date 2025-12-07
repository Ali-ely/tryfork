//
//  LocationManager.swift
//  TestMap
//

//
//  LocationManager.swift
//  TestMap
//

import Foundation
import CoreLocation
import Combine

@MainActor
final class LocationManager: NSObject, ObservableObject {
    @Published var location: CLLocationCoordinate2D? = nil
    @Published var authorizationStatus: CLAuthorizationStatus = .notDetermined
    @Published var lastError: Error? = nil

    private let manager = CLLocationManager()

    override init() {
        super.init()
        manager.delegate = self
        manager.desiredAccuracy = kCLLocationAccuracyBest
        manager.distanceFilter = 5

        // Do NOT call requestWhenInUseAuthorization() here.
        // We'll call it only in response to explicit user action.
    }

    /// Request permission as a direct user action (exposed to UI).
    /// It's OK to call this from the main thread as a result of a user gesture,
    /// but avoid calling it repeatedly or inside tight UI state-change loops.
    func requestPermission() {
        manager.requestWhenInUseAuthorization()
    }

    /// Start updating only if services are enabled (delegate will also call this when permission changes).
    func startUpdating() {
        if CLLocationManager.locationServicesEnabled() {
            manager.startUpdatingLocation()
        } else {
            lastError = NSError(
                domain: "LocationManager",
                code: 1,
                userInfo: [NSLocalizedDescriptionKey: "Location services disabled"]
            )
        }
    }

    func stopUpdating() {
        manager.stopUpdatingLocation()
    }

    func requestLocationOnce() {
        manager.requestLocation()
    }
}

extension LocationManager: CLLocationManagerDelegate {
    func locationManagerDidChangeAuthorization(_ manager: CLLocationManager) {
        authorizationStatus = manager.authorizationStatus

        switch authorizationStatus {
        case .authorizedWhenInUse, .authorizedAlways:
            // Permission granted: start updating location here (delegate-driven).
            startUpdating()
        case .denied, .restricted:
            // Permission denied: clear location and notify
            lastError = NSError(
                domain: "LocationManager",
                code: 2,
                userInfo: [NSLocalizedDescriptionKey: "Location permission denied."]
            )
            // Important: clear stored location to remove "You" pin in the UI
            location = nil
            stopUpdating()
        default:
            break
        }
    }

    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let loc = locations.last else { return }
        lastError = nil
        location = loc.coordinate
    }

    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        lastError = error
    }
}
