#include "plataformas.h"
#include <cmath>

plataformas::plataformas(float x, float y, float w, float h, float amplitud, float omega, float fase) {
    x0 = x;
    this->y = y;
    ancho = w;
    alto = h;
    this->amplitud = amplitud;
    this->omega = omega;
    this->fase = fase;
    t = 0.0f;
    deltaX = 0.0f;
    this->x = x0 + amplitud * std::sin(fase);   // posición en t = 0
}

void plataformas::mover(float dt) {
    if (amplitud == 0.0f) {      // plataforma quieta (los toldos del nivel 1)
        deltaX = 0.0f;
        return;
    }
    t += dt;
    float xNueva = x0 + amplitud * std::sin(omega * t + fase);
    deltaX = xNueva - x;
    x = xNueva;
}

QRectF plataformas::GETHitbox() const{
    return QRectF(x, y, ancho, alto);
}

float plataformas::GETX() const {
    return x;
}
float plataformas::GETY() const {
    return y;
}
float plataformas::GETDeltaX() const {
    return deltaX;
}