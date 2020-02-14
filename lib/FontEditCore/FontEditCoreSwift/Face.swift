//
//  Face.swift
//  FontEditCore
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

import Foundation

public protocol FaceReader {
    func isPixelSetAt(point: Point, forGlyphWithID glyphID: UInt) -> Bool
    
    var fontSize: Size { get }
    var numberOfGlyphs: UInt { get }
}

public class Face {
    
    public init(faceReader: FaceReader) {
        self.fecFace = FECFace(faceReader: FaceReaderWrapper(faceReader: faceReader))
    }
    
    public func glyph(at index: UInt) -> Glyph? {
        return Glyph(fecGlyph: self.fecFace.glyph(at: index))
    }
    
    public func setGlyph(_ glyph: Glyph, at index: UInt) {
        fecFace.setGlyph(glyph.fecGlyph, at: index)
    }
    
    public var glyphSize: Size {
        return fecFace.glyphSize.size
    }
    
    public var numberOfGlyphs: UInt {
        return fecFace.numberOfGlyphs
    }
    
    public var glyphs: [Glyph] {
        return fecFace.glyphs.map(Glyph.init(fecGlyph:))
    }

    private let fecFace: FECFace
}

class FaceReaderWrapper: NSObject, FECFaceReader {
    init(faceReader: FaceReader) {
        self.faceReader = faceReader
    }

    func isPixelSet(at point: FECPoint, forGlyphWithID glyphID: UInt) -> Bool {
        return faceReader.isPixelSetAt(point: Point(x: point.x, y: point.y), forGlyphWithID: glyphID)
    }
    
    var fontSize: FECSize {
        let size = faceReader.fontSize
        return FECSize(width: size.width, height: size.height)
    }

    var numberOfGlyphs: UInt {
        return faceReader.numberOfGlyphs
    }
    
    private let faceReader: FaceReader
}

