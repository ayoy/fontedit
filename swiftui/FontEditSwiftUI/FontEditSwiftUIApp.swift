import SwiftUI

@main
struct FontEditSwiftUIApp: App {
    @StateObject private var document = FontDocumentViewModel()

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(document)
        }
        .windowStyle(.titleBar)
        .commands {
            CommandGroup(replacing: .newItem) { }
        }
    }
}
