import Foundation
import Combine

/// Keeps direct read-only access to user-selected game data.
@MainActor
final class GameFolderAccess: ObservableObject {
    @Published private(set) var folderName = "No folder selected"
    @Published private(set) var lastError: String?
    private var folderURL: URL?
    private var hasSecurityScope = false
    private let bookmarkKey = "MW3NativeShell.bookmark"

    init() { restore() }

    func select(_ url: URL) {
        close()
        hasSecurityScope = url.startAccessingSecurityScopedResource()
        folderURL = url
        folderName = url.lastPathComponent
        lastError = nil

        do {
            let bookmark = try url.bookmarkData(
                options: [], includingResourceValuesForKeys: nil, relativeTo: nil
            )
            UserDefaults.standard.set(bookmark, forKey: bookmarkKey)
        } catch {
            // Some file providers do not support persistent bookmarks.
            lastError = "Selected folder is available for this session only."
        }
    }

    func restore() {
        guard let bookmark = UserDefaults.standard.data(forKey: bookmarkKey) else { return }
        do {
            var stale = false
            let url = try URL(
                resolvingBookmarkData: bookmark, options: [],
                relativeTo: nil, bookmarkDataIsStale: &stale
            )
            select(url)
        } catch {
            lastError = "Previous folder access expired. Choose the folder again."
        }
    }

    /// Caller owns and closes returned handle; never copies assets.
    func openReadOnly(relativePath: String) throws -> FileHandle {
        guard let root = folderURL else {
            throw NSError(domain: "MW3NativeShell", code: 1,
                          userInfo: [NSLocalizedDescriptionKey: "No game folder selected"])
        }
        let components = relativePath.split(separator: "/", omittingEmptySubsequences: false)
        guard !relativePath.hasPrefix("/") && !components.contains("..") && !relativePath.contains("\\") else {
            throw NSError(domain: "MW3NativeShell", code: 2,
                          userInfo: [NSLocalizedDescriptionKey: "Invalid relative game path"])
        }
        let target = root.appendingPathComponent(relativePath)
        return try FileHandle(forReadingFrom: target)
    }

    func close() {
        if hasSecurityScope { folderURL?.stopAccessingSecurityScopedResource() }
        folderURL = nil
        hasSecurityScope = false
    }
}
