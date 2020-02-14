//
//  FECSize.m
//  FontEditCore
//
//  Created by Dominik Kapusta on 13/02/2020.
//  Copyright © 2020 The Fat Cat. All rights reserved.
//

#import "FECFontData.h"
#import "fontdata.h"
#include <memory>

#pragma mark FECGlyph

@implementation FECGlyph {
    Font::Glyph *_object;
    BOOL _isObjectOwned;
}

- (instancetype)__initWithGlyph:(Font::Glyph *)glyph
{
    if (self = [super init]) {
        _object = glyph;
    }
    return self;
}

- (instancetype)initWithSize:(FECSize)size
{
    return [self initWithSize:size pixels:nil];
}

- (instancetype)initWithSize:(FECSize)size pixels:(NSArray<NSNumber *> *)pixels
{
    if (self = [super init]) {
        Font::Size fontSize { size.width, size.height };
        if (pixels == nil) {
            _object = new Font::Glyph(fontSize);
        } else {
            std::vector<bool> fontPixels;
            for (NSNumber *number in pixels) {
                fontPixels.push_back([number boolValue]);
            }
            _object = new Font::Glyph(fontSize, fontPixels);
        }
        _isObjectOwned = YES;
    }
    return self;
}

- (void)dealloc
{
    if (_isObjectOwned) {
        delete _object;
        _object = nullptr;
    }
}

- (NSArray<NSNumber *> *)pixels
{
    NSMutableArray<NSNumber *> *pixels = [NSMutableArray new];
    for (auto p : _object->pixels()) {
        [pixels addObject:@((BOOL)p)];
    }
    return pixels;
}

- (BOOL)isPixelSet:(FECPoint)point
{
    return _object->is_pixel_set({ point.x, point.y });
}

- (void)setPixelSet:(FECPoint)point
{
    _object->set_pixel_set({ point.x, point.y }, true);
}

- (FECSize)size
{
    Font::Size sz = _object->size();
    return FECSizeMake(sz.width, sz.height);
}

- (Font::Glyph *)__rawGlyph
{
    return _object;
}

@end


#pragma mark - FaceReaderImpl

class FaceReaderImpl: public Font::FaceReader {

public:
    FaceReaderImpl(void *protocol_obj) :
        Font::FaceReader(), protocol_obj_ { protocol_obj} {}

    Font::Size font_size() const {
        FECSize size = [(__bridge id)protocol_obj_ fontSize];
        return {size.width, size.height};
    }

    std::size_t num_glyphs() const {
        return [(__bridge id)protocol_obj_ numberOfGlyphs];
    }

    bool is_pixel_set(std::size_t glyph_id, Font::Point p) const {
        return [(__bridge id)protocol_obj_ isPixelSetAtPoint:FECPointMake(p.x, p.y)
                                              forGlyphWithID:glyph_id];
    }

private:
    void *protocol_obj_;
    
};


#pragma mark - FECFace

@implementation FECFace {
    std::unique_ptr<Font::Face> _object;
}

- (instancetype)initWithFaceReader:(id<FECFaceReader>)faceReader
{
    if (self = [super init]) {
        FaceReaderImpl readerImpl((__bridge void *)faceReader);
        _object = std::make_unique<Font::Face>(readerImpl);
    }
    return self;
}

- (FECGlyph *)glyphAtIndex:(NSUInteger)index
{
    return [[FECGlyph alloc] __initWithGlyph:&_object->glyph_at(index)];
}

- (void)setGlyph:(FECGlyph *)glyph atIndex:(NSUInteger)index
{
    _object->set_glyph(*[glyph __rawGlyph], index);
}

- (FECGlyph *)objectAtIndexedSubscript:(NSUInteger)index
{
    return [self glyphAtIndex:index];
}

- (void)setObject:(FECGlyph *)newValue atIndexedSubscript:(NSUInteger)index
{
    [self setGlyph:newValue atIndex:index];
}

- (FECSize)glyphSize
{
    Font::Size sz = _object->glyph_size();
    return FECSizeMake(sz.width, sz.height);
}

- (NSUInteger)numberOfGlyphs
{
    return _object->num_glyphs();
}

@end
