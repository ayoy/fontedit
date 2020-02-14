//
//  FECFormat.mm
//  FontEditCore
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import "FECFormat.h"
#import "f2b.h"

NSString *FormatIdentifierC = [NSString stringWithUTF8String: std::string(Format::C::identifier).c_str()];

NSString *FormatIdentifierArduino = [NSString stringWithUTF8String: std::string(Format::Arduino::identifier).c_str()];

NSString *FormatIdentifierPythonList = [NSString stringWithUTF8String: std::string(Format::PythonList::identifier).c_str()];

NSString *FormatIdentifierPythonBytes = [NSString stringWithUTF8String: std::string(Format::PythonBytes::identifier).c_str()];

@implementation FECFormat

+ (NSArray<NSString *> *)availableIdentifiers
{
    return @[FormatIdentifierC,
             FormatIdentifierArduino,
             FormatIdentifierPythonList,
             FormatIdentifierPythonBytes];
}

@end
