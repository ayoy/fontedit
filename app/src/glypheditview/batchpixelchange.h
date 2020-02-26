#ifndef GLYPHEDITCOMMAND_H
#define GLYPHEDITCOMMAND_H

#include <QUndoCommand>
#include <unordered_map>
#include <memory>
#include <functional>
#include <f2b.h>

struct PointHash {
    size_t operator()(const Font::Point &p) const {
        return std::hash<std::size_t>()(p.x) ^ std::hash<std::size_t>()(p.y);
    }
};

struct BatchPixelChange {
    enum class ChangeType {
        Normal, Reverse
    };

    std::unordered_map<Font::Point,bool,PointHash> changes;

    void add(const Font::Point &pixel, bool value) {
        auto i = changes.find(pixel);
        if (i == changes.end()) {
            changes.insert({ pixel, value });
        } else if (i->second != value) {
            changes.erase(pixel);
        }
    }

    void apply(Font::Glyph& glyph, ChangeType type = ChangeType::Normal) const {
        for (const auto& i : changes) {
            glyph.set_pixel_set(i.first, type == ChangeType::Normal ? i.second : !i.second);
        }
    }
};


class GlyphEditCommand : public QUndoCommand
{
public:
    GlyphEditCommand(std::function<void()> undo,
                     std::function<void()> redo,
                     QUndoCommand *parent = nullptr);

    void undo() override {
        undo_();
    }

    void redo() override {
        redo_();
    }

private:
    std::function<void()> undo_;
    std::function<void()> redo_;
};

#endif // GLYPHEDITCOMMAND_H
