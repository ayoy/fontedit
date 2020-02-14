//
//  FECFontData_Private.h
//  FontEditCore
//
//  Created by Dominik Kapusta on 14/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FECFontData.h"
#import "fontdata.h"

NS_ASSUME_NONNULL_BEGIN

@interface FECFace ()

- (Font::Face *)__face;

@end

NS_ASSUME_NONNULL_END
