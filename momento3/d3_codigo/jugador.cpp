#include "jugador.h"

static float direccionDeseada = 0.0f;

jugador::jugador(float posicionX, float posicionY) : entidad(posicionX, posicionY, 100) {
    municion = 30;
    pesoBase = 5.0f;
    enAire = false;
}

void jugador::saltar() {
    if (!enAire) {
        vy = -800.0f;
        enAire = true;
    }
}

void jugador::disparar() {
    if (municion > 0) {
        municion--;
    }
}

void jugador::mover(float dt) {
    float aceleracion = enAire ? 1400.0f : 1200.0f;
    float friccion = enAire ? 150.0f : 1000.0f;
    float velocidadMaxima = enAire ? 260.0f : 240.0f;

    if (direccionDeseada != 0.0f) {
        vx += direccionDeseada * aceleracion * dt;
        if (vx > velocidadMaxima) vx = velocidadMaxima;
        if (vx < -velocidadMaxima) vx = -velocidadMaxima;
    } else {
        if (vx > 0.0f) {
            vx -= friccion * dt;
            if (vx < 0.0f) vx = 0.0f;
        } else if (vx < 0.0f) {
            vx += friccion * dt;
            if (vx > 0.0f) vx = 0.0f;
        }
    }

    enMovimiento = (vx != 0.0f);
    if (!enAire && enMovimiento) {
        contadorFrames++;
        if (contadorFrames >= 5) {
            frameActual = (frameActual + 1) % 12;
            contadorFrames = 0;
        }
    } else if (!enAire) {
        frameActual = 0;
        contadorFrames = 0;
    }

    if (enAire) {
        vy += 1500.0f * dt;
    }

    x += vx * dt;
    y += vy * dt;

    if (x < 0.0f){
    x = 0.0f;
    }

    float sueloY = 660.0f;
    if (y >= sueloY) {
        y = sueloY;
        vy = 0.0f;
        enAire = false;
    }
}



void jugador::setVelocidadX(float nuevaVx) {
    if (nuevaVx > 0.0f) {
        direccionDeseada = 1.0f;
        mirandoDerecha = true;
    } else if (nuevaVx < 0.0f) {
        direccionDeseada = -1.0f;
        mirandoDerecha = false;
    } else {
        direccionDeseada = 0.0f;
    }
}

bool jugador::GETmirandoDerecha() const{
    return mirandoDerecha;
}

int jugador::GETFrameActual() const{
    return frameActual;
}

bool jugador::GETEnElAire() const {
    return enAire;
}

void jugador::setEnMovimiento(bool mov){
    enMovimiento=mov;
}

