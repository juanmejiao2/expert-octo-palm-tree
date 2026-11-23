#include "proyectil.h"
#include <cmath>

Proyectil::Proyectil(float x, float y, float vx, float vy, bool esDelJugador, TipoMovimientoProyectil tipo) {
    this->x = x;
    this->y = y;
    velocidadX = vx;
    velocidadY = vy;
    deJugador = esDelJugador;
    tipoMovimiento = tipo;
    tiempoAcumulado = 0.0f;
    ataqueOrigen = -1;
    pivoteX = pivoteY = largo = theta = omegaPendulo = 0.0f;
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
    case Pendulo:
        movPendulo(dt);
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
TipoMovimientoProyectil Proyectil::GETTipoMovimiento() const{
    return tipoMovimiento;
}
float Proyectil::GETX() const {
    return x;
}
float Proyectil::GETY() const {
    return y;
}

void Proyectil::configurarPendulo(float pivX, float pivY, float largoCuerda, float anguloInicial) {
    pivoteX = pivX;
    pivoteY = pivY;
    largo = largoCuerda;
    theta = anguloInicial;
    omegaPendulo = 0.0f;
    x = pivoteX + largo * std::sin(theta);
    y = pivoteY + largo * std::cos(theta);
}

void Proyectil::movPendulo(float dt) {
    const float g = 980.0f;
    const float b = 0.05f;
    float alfa = -(g / largo) * std::sin(theta) - b * omegaPendulo;
    omegaPendulo += alfa * dt;
    theta += omegaPendulo * dt;
    pivoteX += velocidadX * dt;
    pivoteY += velocidadY * dt;
    x = pivoteX + largo * std::sin(theta);
    y = pivoteY + largo * std::cos(theta);
}

void Proyectil::setAtaqueOrigen(int ataque) {
    ataqueOrigen = ataque;
}
int Proyectil::GETAtaqueOrigen() const {
    return ataqueOrigen;
}
float Proyectil::GETPivoteX() const {
    return pivoteX;
}
float Proyectil::GETPivoteY() const {
    return pivoteY;
}