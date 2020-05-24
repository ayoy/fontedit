#ifndef GLYPHEDITCOMMAND_H
#define GLYPHEDITCOMMAND_H

#include <unordered_map>
#include <memory>
#include <functional>
#include <f2b.h>

struct PointHash {
    size_t operator()(const f2b::Font::Point &p) const {
        return std::hash<std::size_t>()(p.x) ^ std::hash<std::size_t>()(p.y);
    }
};

struct BatchPixelChange {
    enum class ChangeType {
        Normal, Reverse
    };

    std::unordered_map<f2b::Font::Point,bool,PointHash> changes;

    void add(const f2b::Font::Point &pixel, bool value) {
        auto i = changes.find(pixel);
        if (i == changes.end()) {
            changes.insert({ pixel, value });
        } else if (i->second != value) {
            changes.erase(pixel);
        }
    }

    void apply(f2b::Font::Glyph& glyph, ChangeType type = ChangeType::Normal) const {
        for (const auto& i : changes) {
            glyph.set_pixel_set(i.first, type == ChangeType::Normal ? i.second : !i.second);
        }
    }
};


#endif // GLYPHEDITCOMMAND_H
