#include "enemigos.h"
#include <cmath>

enemigo::enemigo(float _x, float _y, bool _tieneArma) : entidad(_x, _y, 100) {
    estadoActual=EstadoEnemigo::idle;
    rangoVision= 450.0f;
    TieneArma= _tieneArma;
    MostrarAlerta=false;

    temporizadorDisparo.start();
    temporizadoralerta.start();
}

enemigo::~enemigo(){

}

void enemigo::mover(float dt){
    (void)dt;
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
    }else{
        estadoActual=EstadoEnemigo::idle;
        MostrarAlerta=false;
    }
}

bool enemigo::puededisparar(){
    if (estadoActual !=EstadoEnemigo::disparando|| !TieneArma) return false;

    if (temporizadorDisparo.elapsed()>1500){
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
    return QRectF(x, y - 100.0f, 40.0f, 100.0f);
}