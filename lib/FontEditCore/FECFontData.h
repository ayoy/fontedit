//
//  FECSize.h
//  FontEditCore
//
//  Created by Dominik Kapusta on 13/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

struct FECSize {
    NSUInteger width;
    NSUInteger height;
};

struct FECPoint {
    NSUInteger x;
    NSUInteger y;
};

inline NSUInteger FECPointOffset(struct FECPoint p, struct FECSize sz)
{
    return p.y * sz.width + p.x;
}

@interface FECGlyph : NSObject


@end

NS_ASSUME_NONNULL_END
