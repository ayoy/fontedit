//
//  FontEditCoreTests.m
//  FontEditCoreTests
//
//  Created by Dominik Kapusta on 13/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "FontEditCore.h"
#import "FECTestFaceData.h"

@interface FontEditCoreTests : XCTestCase

@end

@implementation FontEditCoreTests

- (void)setUp {
}

- (void)tearDown {
}

- (void)testSize {
    FECTestFaceData *testData = [FECTestFaceData new];
    FECFace *face = [[FECFace alloc] initWithFaceReader:testData];
    XCTAssertEqual(face.numberOfGlyphs, 5);
    
    FECGlyph *glyph = [face glyphAtIndex:2];
    XCTAssertEqual(glyph.size.height, testData.fontSize.height);
    XCTAssertEqual(glyph.size.width, testData.fontSize.width);
}

@end
