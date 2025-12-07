//
//  MapView.swift
//  TestMap
//

import SwiftUI
import MapKit

struct MapView: UIViewRepresentable {
    @Binding var route: [CLLocationCoordinate2D]
    @Binding var center: CLLocationCoordinate2D

    var namedNodes: [String: Node] = [:]
    // NEW: reset trigger to force region/zoom/pan reset
    var resetTrigger: UUID = UUID()
    var highlightedNodeId: String? = nil

    func makeUIView(context: Context) -> MKMapView {
        let mv = MKMapView(frame: .zero)
        mv.delegate = context.coordinator
        mv.showsUserLocation = true
        mv.userTrackingMode = .none
        mv.mapType = .standard
        let region = MKCoordinateRegion(center: center, latitudinalMeters: 600, longitudinalMeters: 600)
        mv.setRegion(region, animated: false)
        return mv
    }

    func updateUIView(_ uiView: MKMapView, context: Context) {
        // If resetTrigger changed, force-reset region and remove overlays before doing annotations.
        if context.coordinator.lastResetId != resetTrigger {
            // remove overlays & annotations except the user location
            uiView.removeOverlays(uiView.overlays)
            _ = uiView.annotations.filter { $0 is MKUserLocation }
            uiView.removeAnnotations(uiView.annotations.filter { !($0 is MKUserLocation) })

            // ensure we are not following user
            uiView.userTrackingMode = .none

            // set region to the requested center with the initial zoom (no animation)
            let region = MKCoordinateRegion(center: center, latitudinalMeters: 600, longitudinalMeters: 600)
            uiView.setRegion(region, animated: false)

            // re-add node annotations below (we removed them above)
            for (id, node) in namedNodes {
                let ann = MKPointAnnotation()
                ann.coordinate = CLLocationCoordinate2D(latitude: node.lat, longitude: node.lon)
                ann.title = node.name ?? node.id
                ann.subtitle = id
                uiView.addAnnotation(ann)
            }

            // keep the user location annotation visible (MKMapView handles it)
            context.coordinator.lastResetId = resetTrigger
            return
        }

        // Normal update path (not a forced reset)

        // Update node annotations (preserve MKUserLocation)
        let existingNodeAnnotations = uiView.annotations.filter { !($0 is MKUserLocation) }
        uiView.removeAnnotations(existingNodeAnnotations)

        var annotations: [MKPointAnnotation] = []
        for (id, node) in namedNodes {
            let ann = MKPointAnnotation()
            ann.coordinate = CLLocationCoordinate2D(latitude: node.lat, longitude: node.lon)
            ann.title = node.name ?? node.id
            ann.subtitle = id
            annotations.append(ann)
        }
        uiView.addAnnotations(annotations)

        // Overlays / polyline
        uiView.removeOverlays(uiView.overlays)
        if route.count > 0 {
            let poly = MKPolyline(coordinates: route, count: route.count)
            uiView.addOverlay(poly)
            uiView.setVisibleMapRect(poly.boundingMapRect, edgePadding: UIEdgeInsets(top: 90, left: 60, bottom: 90, right: 60), animated: true)
        } else {
            // no route -> ensure region matches the center state (animated)
            let region = MKCoordinateRegion(center: center, latitudinalMeters: 600, longitudinalMeters: 600)
            uiView.setRegion(region, animated: true)
        }
    }

    func makeCoordinator() -> Coordinator { Coordinator(self) }

    class Coordinator: NSObject, MKMapViewDelegate {
        var parent: MapView
        var lastResetId: UUID? = nil

        init(_ parent: MapView) { self.parent = parent }

        func mapView(_ mapView: MKMapView, rendererFor overlay: MKOverlay) -> MKOverlayRenderer {
            if let poly = overlay as? MKPolyline {
                let r = MKPolylineRenderer(polyline: poly)
                r.lineWidth = 5
                r.alpha = 0.95
                r.strokeColor = UIColor.systemBlue
                return r
            }
            return MKOverlayRenderer(overlay: overlay)
        }

        func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
            // Use default for user location (blue dot)
            if annotation is MKUserLocation { return nil }

            let id = "nodePin"
            var v = mapView.dequeueReusableAnnotationView(withIdentifier: id) as? MKMarkerAnnotationView
            if v == nil {
                v = MKMarkerAnnotationView(annotation: annotation, reuseIdentifier: id)
                v?.canShowCallout = true
                v?.markerTintColor = UIColor.systemOrange
            } else {
                v?.annotation = annotation
            }

            if let nodeId = annotation.subtitle ?? nil, nodeId == parent.highlightedNodeId {
                v?.markerTintColor = UIColor.systemRed
                v?.glyphText = "★"
            } else {
                v?.markerTintColor = UIColor.systemOrange
                v?.glyphText = nil
            }
            return v
        }
    }
}
