//
//  SourceCodeGenerator.swift
//  FontEditCore
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

import Foundation

public struct SourceCodeOptions {
    public var bitNumbering: BitNumbering
    public var shouldInvertBits: Bool
}

public class SourceCodeGenerator {
    public let options: SourceCodeOptions
    public let format: Format

    public init(format: Format, options: SourceCodeOptions) {
        self.format = format
        self.options = options
        fecGenerator = FECSourceCodeGenerator(formatIdentifier: format.identifier,
                                              sourceCodeOptions: options.fecSourceCodeOptions)
    }
    
    public func generateCode(for face: Face) -> String {
        return fecGenerator.generateCode(for: face.fecFace)
    }
    
    private let fecGenerator: FECSourceCodeGenerator
}


private extension SourceCodeOptions {
    var fecSourceCodeOptions: FECSourceCodeOptions {
        return FECSourceCodeOptions(bitNumbering: bitNumbering,
                                    shouldInvertBits: shouldInvertBits)
    }
}
