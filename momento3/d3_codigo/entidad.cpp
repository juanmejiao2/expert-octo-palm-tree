#include "entidad.h"

entidad::entidad(float posicionX, float posicionY, short hp){
    x=posicionX;
    y=posicionY;
    vx=0;
    vy=0;
    vida=hp;
}

entidad::~entidad() {
}

void entidad::mover(float dt) {
    x += vx * dt;
    y += vy * dt;
}

void entidad::recibirDanio(short cant) {
    vida -= cant;
    if (vida < 0) vida = 0;
}

float entidad::GETX() const {
    return x;
}
float entidad::GETY() const {
    return y;
}
void entidad::setX(float nuevaX) {
    x = nuevaX;
}
void entidad::setY(float nuevaY) {
    y = nuevaY;
}

void entidad::setVy(float nuevaVy) {
    vy = nuevaVy;
}

float entidad::GETVy() const {
    return vy;
}