#include "plataformas.h"

plataformas::plataformas(float x, float y, float w, float h): entidad(x, y , 100) {
    ancho=w;
    alto=h;
}

QRectF plataformas::GETHitbox() const{
    return QRectF(x, y, ancho, alto);
}