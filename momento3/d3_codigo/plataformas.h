#ifndef PLATAFORMAS_H
#define PLATAFORMAS_H

#include <QRectF>

class plataformas {
private:
    float x, y;       // posición
    float ancho;
    float alto;
    float x0;
    float amplitud;
    float omega;
    float fase;
    float t;
    float deltaX;

public:
    plataformas(float x, float y, float w, float h, float amplitud = 0.0f, float omega = 0.0f, float fase = 0.0f);
    void mover(float dt);
    QRectF GETHitbox() const;
    float GETX() const;
    float GETY() const;
    float GETDeltaX() const;
};

#endif // PLATAFORMAS_H