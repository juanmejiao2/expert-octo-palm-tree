#include "plataformas.h"

plataformas::plataformas(float x, float y, float w, float h) {
    this->x = x;
    this->y = y;
    ancho=w;
    alto=h;
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