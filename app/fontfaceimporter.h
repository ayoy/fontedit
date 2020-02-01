#ifndef FONTFACEIMPORTER_H
#define FONTFACEIMPORTER_H

#include <QFont>
#include "f2b.h"

class FontFaceImporter
{
public:
    FontFaceImporter();

    Font::Face import_face(const QFont &font);
};

#endif // FONTFACEIMPORTER_H
