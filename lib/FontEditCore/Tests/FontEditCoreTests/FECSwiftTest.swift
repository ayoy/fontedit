import XCTest
@testable import FontEditCore

class TestData: FaceReader {
    func isPixelSetAt(point: Point, forGlyphWithID glyphID: UInt) -> Bool {
        return pixels[Int(glyphID)][Int(point.offsetInArray(forGlyphSize: fontSize))]
    }
    
    let fontSize: Size = Size(width: 4, height: 3)
    
    let numberOfGlyphs: UInt = 5
    
    let pixels: [[Bool]] = [
        [
            false, false, false, false,
            false, false, false, true,
            false, false, true, false
        ],
        [
            false, true, false, false,
            true, false, true, false,
            true, false, false, true
        ],
        [
            true, true, true, true,
            true, true, false, true,
            true, false, true, true
        ],
        [
            true, false, false, true,
            true, false, true, false,
            false, false, true, true
        ],
        [
            false, true, true, true,
            true, true, false, false,
            true, false, true, false
        ]
    ]
}

class FECSwiftTest: XCTestCase {

    func testExample() {
        let testData = TestData()
        let face = Face(faceReader: testData)
        XCTAssertEqual(face.numberOfGlyphs, 5);
        
        let glyph = face.glyph(at: 2)!
        XCTAssertEqual(glyph.size.height, testData.fontSize.height);
        XCTAssertEqual(glyph.size.width, testData.fontSize.width);

        XCTAssertEqual(Glyph(size: Size(width: 5, height: 5)).size, Size(width: 5, height: 5))
    }

    static var allTests = [
        ("testExample", testExample),
    ]
}
