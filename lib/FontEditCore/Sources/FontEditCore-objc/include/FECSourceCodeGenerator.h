//
//  FECSourceCodeGenerator.h
//  FontEditCore
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FECFormat.h"
#import "FECFontData.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, FECBitNumbering) {
    FECBitNumberingLSB,
    FECBitNumberingMSB
} NS_SWIFT_NAME(BitNumbering);

@interface FECSourceCodeOptions : NSObject

- (instancetype)initWithBitNumbering:(FECBitNumbering)bitNumbering
                    shouldInvertBits:(BOOL)shouldInvertBits;

@property (nonatomic, assign) FECBitNumbering bitNumbering;
@property (nonatomic, assign) BOOL shouldInvertBits;
@end

@interface FECSourceCodeGenerator : NSObject

- (instancetype)initWithFormatIdentifier:(NSString *)formatIdentifier
                       sourceCodeOptions:(FECSourceCodeOptions *)options;

- (NSString *)generateCodeForFace:(FECFace *)face;

@property (nonatomic, strong, readonly) NSString *formatIdentifier;
@end

NS_ASSUME_NONNULL_END
