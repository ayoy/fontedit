//
//  FECTestFaceData.m
//  FontEditCoreTests
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import "FECTestFaceData.h"

@implementation FECTestFaceData {
    NSArray<NSNumber *> *_pixels;
}

- (instancetype)init
{
    if (self = [super init]) {
        _pixels = @[
            @0, @0, @0, @0,
            @0, @0, @0, @1,
            @0, @0, @1, @0,
            //
            @0, @1, @0, @0,
            @1, @0, @1, @0,
            @1, @0, @0, @1,
            //
            @1, @1, @1, @1,
            @1, @1, @0, @1,
            @1, @0, @1, @1,
            //
            @1, @0, @0, @1,
            @1, @0, @1, @0,
            @0, @0, @1, @1,
            //
            @0, @1, @1, @1,
            @1, @1, @0, @0,
            @1, @0, @1, @0
        ];
    }
    return self;
}

- (FECSize)fontSize
{
    return FECSizeMake(4, 3);
}

- (NSUInteger)numberOfGlyphs
{
    return 5;
}

- (BOOL)isPixelSetAtPoint:(FECPoint)point forGlyphWithID:(NSUInteger)glyphID
{
    NSUInteger glyphOffset = glyphID * self.fontSize.width * self.fontSize.height;
    return [_pixels[glyphOffset + FECPointOffset(point, self.fontSize)] boolValue];
}

@end
