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
        var firstArchiveEntries = 0
        var sampleReadVerified = false
        var firstArchiveError: String?
        var mainFound = false
        var zoneFound = false

        var summary: String {
            if !mainFound || !zoneFound {
                return "Game folder incomplete: select the directory containing main and zone."
            }
            let archive = firstArchiveError ?? "First IWD index: \(firstArchiveEntries) entries; in-memory CRC sample: \(sampleReadVerified ? "OK" : "not verified")"
            return "IWD: \(zipHeaders)/\(iwdFiles) ZIP headers | FastFiles: \(fastFiles) (signed \(signedFastFiles), unsigned \(unsignedFastFiles), unknown \(invalidFastFiles)). \(archive). IW5 engine not linked."
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
        if result.mainFound {
            // One sample IWD only; do not scan/decompress every game archive on the UI thread.
            if let archive = files(in: main)
                .filter({ $0.pathExtension.lowercased() == "iwd" })
                .sorted(by: { $0.lastPathComponent < $1.lastPathComponent }).first {
                var metadata = MW3IWDStats()
                let code = archive.path.withCString { mw3_iwd_probe($0, &metadata) }
                if code == MW3_IWD_OK {
                    result.firstArchiveEntries = Int(metadata.entry_count)
                    let sample = withUnsafePointer(to: &metadata.sample_name) { pointer in
                        pointer.withMemoryRebound(to: CChar.self, capacity: 256) {
                            String(cString: $0)
                        }
                    }
                    let length = Int(metadata.sample_uncompressed_size)
                    if !sample.isEmpty && length > 0 && length <= 65536 {
                        var bytes = [UInt8](repeating: 0, count: length)
                        var written: Int = 0
                        let verified = archive.path.withCString { path in
                            sample.withCString { entry in
                                bytes.withUnsafeMutableBufferPointer { buffer in
                                    mw3_iwd_read_entry(path, entry, buffer.baseAddress,
                                                       buffer.count, &written)
                                }
                            }
                        }
                        result.sampleReadVerified = verified == MW3_IWD_OK && written == length
                    }
                } else {
                    result.firstArchiveError = "IWD directory read: code \(code.rawValue)"
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
