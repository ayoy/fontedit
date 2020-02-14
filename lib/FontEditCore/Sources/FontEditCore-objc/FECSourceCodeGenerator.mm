//
//  FECSourceCodeGenerator.mm
//  FontEditCore
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import "FECSourceCodeGenerator.h"
#import "FECFontData_Private.h"

#import "fontsourcecodegenerator.h"
#import "format.h"
#import <memory>

@implementation FECSourceCodeOptions

- (instancetype)initWithBitNumbering:(FECBitNumbering)bitNumbering
                    shouldInvertBits:(BOOL)shouldInvertBits
{
    if (self = [super init]) {
        self.bitNumbering = bitNumbering;
        self.shouldInvertBits = shouldInvertBits;
    }
    return self;
}

- (SourceCodeOptions)__sourceCodeOptions
{
    SourceCodeOptions::BitNumbering nb = self.bitNumbering == FECBitNumberingLSB
        ? SourceCodeOptions::LSB : SourceCodeOptions::MSB;
    return SourceCodeOptions { nb, static_cast<bool>(self.shouldInvertBits) };
}

@end

@implementation FECSourceCodeGenerator {
    std::unique_ptr<FontSourceCodeGenerator> _object;
};

- (instancetype)initWithFormatIdentifier:(NSString *)formatIdentifier
                       sourceCodeOptions:(nonnull FECSourceCodeOptions *)options
{
    if (self = [super init]) {
        _object = std::make_unique<FontSourceCodeGenerator>(options.__sourceCodeOptions);
        _formatIdentifier = formatIdentifier;
    }
    return self;
}

- (NSString *)generateCodeForFace:(FECFace *)face
{
    std::string output;

    if ([self.formatIdentifier isEqualToString:FormatIdentifierC]) {
        output = _object->generate<Format::C>(*face.__face);
    } else if ([self.formatIdentifier isEqualToString:FormatIdentifierArduino]) {
        output = _object->generate<Format::Arduino>(*face.__face);
    } else if ([self.formatIdentifier isEqualToString:FormatIdentifierPythonList]) {
        output = _object->generate<Format::PythonList>(*face.__face);
    } else if ([self.formatIdentifier isEqualToString:FormatIdentifierPythonBytes]) {
        output = _object->generate<Format::PythonBytes>(*face.__face);
    }

    return [NSString stringWithUTF8String:output.c_str()];
}

@end
