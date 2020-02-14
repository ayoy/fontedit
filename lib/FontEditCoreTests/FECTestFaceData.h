//
//  FECTestFaceData.h
//  FontEditCoreTests
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FontEditCore.h"

NS_ASSUME_NONNULL_BEGIN

@interface FECTestFaceData : NSObject <FECFaceReader>

- (BOOL)isPixelSetAtPoint:(FECPoint)point forGlyphWithID:(NSUInteger)glyph_id;

@property (nonatomic, assign, readonly) FECSize fontSize;
@property (nonatomic, assign, readonly) NSUInteger numberOfGlyphs;

@end


NS_ASSUME_NONNULL_END
