#include "enemigos.h"
#include <cmath>

enemigo::enemigo(float _x, float _y, bool _tieneArma, TipoEnemigoArma _tipo, short vidaInicial) : entidad(_x, _y, vidaInicial) {
    estadoActual=EstadoEnemigo::idle;
    TipoEnemigo = _tipo;

    if (TipoEnemigo == TipoEnemigoArma::lanzallamas) {
        rangoVision = 220.0f;
    } else if (TipoEnemigo == TipoEnemigoArma::esmadpistolon ) {
        rangoVision = 300.0f;
    }else if(TipoEnemigo == TipoEnemigoArma::lanzagranadas){
        rangoVision= 310.0f;
    }
    else {
        rangoVision = 300.0f;
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

    if (TipoEnemigo == TipoEnemigoArma::lanzagranadas || TipoEnemigo == TipoEnemigoArma::esmadpistolon) {
        vx = 0.0f;
        return;
    }

    if (estadoActual == EstadoEnemigo::alerta || estadoActual == EstadoEnemigo::disparando) {
        float velocidadPresion = 45.0f;
        vx = (TipoEnemigo == TipoEnemigoArma::conservador || TipoEnemigo == TipoEnemigoArma::liberal) ? 40.0f : 0.0f;
    } else {
        static float tiempoPatrulla = 0.0f;
        tiempoPatrulla += dt;
        vx = std::sin(tiempoPatrulla * 1.5f + x) * 25.0f;
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
            if (temporizadoralerta.elapsed()>800){
                estadoActual= EstadoEnemigo::disparando;
                MostrarAlerta=false;
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
    TipoEnemigoArma tipo = GETTipoEnemigo();

    if (tipo == TipoEnemigoArma::lanzallamas) intervaloDisparo = 1000;
    else if (tipo == TipoEnemigoArma::esmadpistolon) intervaloDisparo = 1500;
    else if (tipo == TipoEnemigoArma::lanzagranadas) intervaloDisparo = 2500;

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