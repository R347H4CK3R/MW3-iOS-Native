import Foundation

/// Checks IW5 container headers in-place through a user-approved Files folder.
/// No copies, extraction, asset deserialization, or game execution.
enum GameAssetInventory {
    struct Report {
        var iwdFiles = 0
        var zipHeaders = 0
        var fastFiles = 0
        var signedFastFiles = 0
        var unsignedFastFiles = 0
        var invalidFastFiles = 0
        var mainFound = false
        var zoneFound = false

        var summary: String {
            if !mainFound || !zoneFound {
                return "Game folder incomplete: select the directory containing main and zone."
            }
            return "IWD: \(zipHeaders)/\(iwdFiles) ZIP headers | FastFiles: \(fastFiles) (signed \(signedFastFiles), unsigned \(unsignedFastFiles), unknown \(invalidFastFiles)). Containers only; IW5 engine not linked."
        }
    }

    private static func files(in directory: URL) -> [URL] {
        (try? FileManager.default.contentsOfDirectory(
            at: directory,
            includingPropertiesForKeys: [.isRegularFileKey],
            options: [.skipsHiddenFiles]
        )) ?? []
    }

    private static func readHeader(_ url: URL, count: Int) -> Data? {
        guard let handle = try? FileHandle(forReadingFrom: url) else { return nil }
        defer { try? handle.close() }
        return try? handle.read(upToCount: count)
    }

    static func scan(folder: URL) -> Report {
        var result = Report()
        let main = folder.appendingPathComponent("main", isDirectory: true)
        let zone = folder
            .appendingPathComponent("zone", isDirectory: true)
            .appendingPathComponent("english", isDirectory: true)

        var isDir: ObjCBool = false
        result.mainFound = FileManager.default.fileExists(atPath: main.path, isDirectory: &isDir)
            && isDir.boolValue
        isDir = false
        result.zoneFound = FileManager.default.fileExists(atPath: zone.path, isDirectory: &isDir)
            && isDir.boolValue

        if result.mainFound {
            for file in files(in: main) where file.pathExtension.lowercased() == "iwd" {
                result.iwdFiles += 1
                let first = readHeader(file, count: 4)
                // A local-file ZIP signature is a format hint, NOT integrity verification.
                if first == Data([0x50, 0x4b, 0x03, 0x04]) {
                    result.zipHeaders += 1
                }
            }
        }
        if result.zoneFound {
            for file in files(in: zone) where file.pathExtension.lowercased() == "ff" {
                result.fastFiles += 1
                guard let header = readHeader(file, count: 8),
                      let magic = String(data: header, encoding: .ascii)
                else {
                    result.invalidFastFiles += 1
                    continue
                }
                switch magic {
                case "IWffu100": result.unsignedFastFiles += 1
                case "IWff0100": result.signedFastFiles += 1
                default: result.invalidFastFiles += 1
                }
            }
        }
        return result
    }
}
