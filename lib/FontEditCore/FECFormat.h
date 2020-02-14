//
//  FECFormat.h
//  FontEditCore
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface FECFormat : NSObject
+ (NSArray<NSString *> *)availableIdentifiers;
@end

extern NSString *FormatIdentifierC;
extern NSString *FormatIdentifierArduino;
extern NSString *FormatIdentifierPythonList;
extern NSString *FormatIdentifierPythonBytes;

NS_ASSUME_NONNULL_END
