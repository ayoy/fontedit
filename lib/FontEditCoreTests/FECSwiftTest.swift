//
//  FECSwiftTest.swift
//  FontEditCoreTests
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

import XCTest
import FontEditCore

class FECSwiftTest: XCTestCase {

    override func setUp() {
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }

    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }

    func testExample() {
        XCTAssertEqual(Glyph(size: Size(width: 5, height: 5)).size, Size(width: 5, height: 5))
    }

}
