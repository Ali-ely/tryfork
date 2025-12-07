//
//  AutocompleteTextField.swift
//  TestMap
//

import SwiftUI
import UIKit // needed for UIApplication.shared to dismiss keyboard

struct AutocompleteTextField: View {
    @Binding var text: String
    var placeholder: String
    var suggestions: ([String]) -> [String]
    var onCommitSelection: ((String) -> Void)? = nil

    @State private var showList = false
    @State private var filtered: [String] = []
    @State private var highlighted = -1

    var body: some View {
        ZStack(alignment: .top) {
            VStack(spacing: 6) {
                HStack {
                    Image(systemName: "magnifyingglass")
                    TextField(placeholder,
                              text: $text,
                              onEditingChanged: { editing in
                                showList = editing
                                updateFiltered()
                              },
                              onCommit: {
                                // user pressed Return -> dismiss keyboard and call commit
                                UIApplication.shared.sendAction(#selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
                                onCommitSelection?(text)
                                showList = false
                              })
                    .textFieldStyle(.roundedBorder)
                    .disableAutocorrection(true)
                    .autocapitalization(.none)

                    if !text.isEmpty {
                        Button(action: {
                            text = ""
                            updateFiltered()
                            showList = true
                        }) {
                            Image(systemName: "xmark.circle.fill").foregroundColor(.secondary)
                        }
                    }
                }

                if showList && !filtered.isEmpty {
                    VStack(spacing: 0) {
                        ForEach(filtered.indices, id: \.self) { i in
                            let item = filtered[i]
                            Button(action: {
                                // user tapped a suggestion -> set text, dismiss keyboard, notify parent
                                text = item
                                // dismiss keyboard
                                UIApplication.shared.sendAction(#selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
                                showList = false
                                onCommitSelection?(item)
                            }) {
                                HStack {
                                    Text(item).lineLimit(1).truncationMode(.tail)
                                    Spacer()
                                }
                                .padding(.vertical, 8)
                                .padding(.horizontal, 10)
                                .background(i == highlighted ? Color(white: 0.95) : Color.clear)
                            }
                            .buttonStyle(PlainButtonStyle())
                        }
                    }
                    .background(RoundedRectangle(cornerRadius: 8).fill(Color.white).shadow(radius: 2))
                    .transition(.opacity.combined(with: .move(edge: .top)))
                    .zIndex(1)
                }
            }
            .onChange(of: text) { _, newValue in
                updateFiltered()
                let lowered = newValue.trimmingCharacters(in: .whitespacesAndNewlines).lowercased()
                if !lowered.isEmpty {
                    if suggestions([newValue]).contains(where: { $0.lowercased() == lowered }) {
                        showList = false
                    }
                } else {
                    showList = false
                }
            }
            .onTapGesture {
                updateFiltered()
                showList = true
            }
        }
    }

    private func updateFiltered() {
        DispatchQueue.main.async {
            let input = text.trimmingCharacters(in: .whitespacesAndNewlines)
            guard !input.isEmpty else {
                self.filtered = []
                self.showList = false
                return
            }

            let items = suggestions([text])
            self.filtered = Array(items.prefix(8))

            let lowered = input.lowercased()
            if self.filtered.contains(where: { $0.lowercased() == lowered }) {
                self.showList = false
            } else {
                self.showList = !self.filtered.isEmpty
            }
        }
    }
}
