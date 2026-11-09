#include "enemigos.h"
#include <cmath>

enemigo::enemigo(float _x, float _y, bool _tieneArma, TipoEnemigoArma _tipo, short vidaInicial) : entidad(_x, _y, 100) {
    estadoActual=EstadoEnemigo::idle;
    TipoEnemigo = _tipo;
    if (TipoEnemigo == TipoEnemigoArma::lanzallamas) {
        rangoVision = 220.0f;
    } else {
        rangoVision = 400.0f;
    }

    TieneArma= _tieneArma;
    MostrarAlerta=false;

    temporizadorDisparo.start();
    temporizadoralerta.start();
}

enemigo::~enemigo(){

}

void enemigo::mover(float dt){
    if (estadoActual == EstadoEnemigo::destruido) return;

    if (TipoEnemigo == TipoEnemigoArma::lanzagranadas) {
        vx = 0.0f;
        return;
    }
    static float tiempoPatrulla = 0.0f;
    tiempoPatrulla += dt;
    float velocidadLenta = 30.0f;
    if (estadoActual == EstadoEnemigo::idle || estadoActual == EstadoEnemigo::alerta) {
        vx = std::sin(tiempoPatrulla * 1.5f + x) * velocidadLenta;
    } else {
        vx = 0.0f;
    }
    x += vx * dt;
}

void enemigo::actualizar(float xjugador, float yjugador){

    if (estadoActual== EstadoEnemigo::destruido) return;

    float distanciaX= xjugador-x;
    float distanciaAbsX= std::abs(distanciaX);

    if (distanciaAbsX <= rangoVision){
        if (estadoActual==EstadoEnemigo::idle){
            estadoActual=EstadoEnemigo::alerta;
            MostrarAlerta=true;
            temporizadoralerta.restart();
        }
        else if (estadoActual== EstadoEnemigo::alerta){
            if (temporizadoralerta.elapsed()>1000){
                estadoActual= EstadoEnemigo::disparando;
                MostrarAlerta=false;
            }
        }
        else if (estadoActual == EstadoEnemigo::disparando) {
            TipoEnemigoArma tipo = GETTipoEnemigo();
            int duracionFuego = (tipo == TipoEnemigoArma::lanzallamas) ? 600 : 200;

            if (temporizadorDisparo.elapsed() > duracionFuego + 2400) {
                temporizadorDisparo.restart();
            }
        }
    }else{
        estadoActual=EstadoEnemigo::idle;
        MostrarAlerta=false;
    }
}

bool enemigo::puededisparar(){
    if (estadoActual !=EstadoEnemigo::disparando|| !TieneArma) return false;
    int intervaloDisparo = 2000;
    if (TipoEnemigo == TipoEnemigoArma::lanzallamas) intervaloDisparo = 2500;
    else if (TipoEnemigo == TipoEnemigoArma::lanzagranadas) intervaloDisparo = 3000;

    if (temporizadorDisparo.elapsed() > intervaloDisparo){
        temporizadorDisparo.restart();
        return true;
    }
    return false;
}

void enemigo::disparar() {
}

void enemigo::recibirdano(int dano) {
    recibirDanio(static_cast<short>(dano));
    if (vida <= 0) {
        estadoActual = EstadoEnemigo::destruido;
    }
}

EstadoEnemigo enemigo::GETEstado() const {
    return estadoActual;
}

bool enemigo::MostrarLaAlerta() const {
    return MostrarAlerta;
}
QRectF enemigo::GETHitbox() const {
    if (TipoEnemigo == TipoEnemigoArma::lanzagranadas || TipoEnemigo == TipoEnemigoArma::lanzallamas) {
        return QRectF(x, y - 90.0f, 90.0f, 90.0f);
    }
    return QRectF(x, y - 100.0f, 40.0f, 100.0f);
}
TipoEnemigoArma enemigo::GETTipoEnemigo() const {
    return TipoEnemigo;
}