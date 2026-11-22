#ifndef PLATAFORMAS_H
#define PLATAFORMAS_H

#include <QRectF>

class plataformas {
private:
    float x, y;       // posición
    float ancho;
    float alto;

public:
    plataformas(float x, float y, float w, float h);
    QRectF GETHitbox() const;
    float GETX() const;
    float GETY() const;
};

#endif // PLATAFORMAS_H