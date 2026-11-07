#include "proyectil.h"
#include <cmath>

Proyectil::Proyectil(float x, float y, float vx, float vy, bool esDelJugador, TipoMovimientoProyectil tipo)
    : entidad(x, y, 1) {
    velocidadX = vx;
    velocidadY = vy;
    deJugador = esDelJugador;
    tipoMovimiento = tipo;
    tiempoAcumulado = 0.0f;
}

void Proyectil::mover(float dt) {
    tiempoAcumulado += dt;

    switch (tipoMovimiento) {
    case Rectilineo:
        movRectilineo(dt);
        break;
    case Parabolico:
        movParabolico(dt);
        break;
    case Oscilatorio:
        movOscilatorio(dt);
        break;
    }
}

void Proyectil::movRectilineo(float dt) {
    x += velocidadX * dt;
    y += velocidadY * dt;
}

void Proyectil::movParabolico(float dt) {
    x += velocidadX * dt;
    velocidadY += 980.0f * dt;
    y += velocidadY * dt;
}

void Proyectil::movOscilatorio(float dt) {
    x += velocidadX * dt;
    y += std::sin(tiempoAcumulado * 10.0f) * 10.0f;
}

bool Proyectil::GETDeJugador() const {
    return deJugador;
}
QRectF Proyectil::GETHitbox() const {
    return QRectF(x, y, 8.0f, 4.0f);
}