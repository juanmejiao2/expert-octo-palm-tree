#include "enemigos.h"
#include <cmath>

enemigo::enemigo(float _x, float _y, bool _tieneArma, TipoEnemigoArma _tipo, short vidaInicial) {
    x = _x;
    y = _y;
    vx = 0;
    vy = 0;
    vida = vidaInicial;
    estadoActual=EstadoEnemigo::idle;
    TipoEnemigo = _tipo;
    rangoVisionY = 380.0f;
    tiempoPatrulla = _x * 0.01f;
    animacionDano = false;
    temporizadorDano.start();
    Stagger=0.0f;

    if (TipoEnemigo == TipoEnemigoArma::lanzallamas) {
        rangoVision = 220.0f;
    } else if (TipoEnemigo == TipoEnemigoArma::esmadpistolon ) {
        rangoVision = 300.0f;
    }else if(TipoEnemigo == TipoEnemigoArma::lanzagranadas){
        rangoVision= 500.0f;
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
    if (Stagger > 0.0f) {
        Stagger = std::max(0.0f, Stagger - dt);
    }

    if (TipoEnemigo == TipoEnemigoArma::lanzagranadas|| TipoEnemigo == TipoEnemigoArma::lanzallamas) {
        vx = 0.0f;
        return;
    }

    if (estadoActual == EstadoEnemigo::idle) {
        vx = 0.0f;
        return;
    }

    float nuevaX = x + vx * dt;

    if (nuevaX < 20.0f) {
        nuevaX = 20.0f;
        vx = 0.0f;
    }

    x = nuevaX;

}


void enemigo::actualizar(float xjugador, float yjugador){

    if (estadoActual== EstadoEnemigo::destruido) return;

    if (animacionDano && temporizadorDano.elapsed() > 220) {
        animacionDano = false;
    }

    float distanciaX= xjugador-x;
    float distanciaAbsX= std::abs(distanciaX);
    float distanciaAbsY = std::abs(yjugador - y);

    if (distanciaAbsX <= rangoVision && distanciaAbsY <= rangoVisionY){
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
                temporizadorRafagaLlamas.restart();
            }
        }
    }else{
        estadoActual=EstadoEnemigo::idle;
        MostrarAlerta=false;
        vx = 0.0f;
        temporizadorRafagaLlamas.restart();
    }
    if (estadoActual == EstadoEnemigo::alerta || estadoActual == EstadoEnemigo::disparando) {
        if (TipoEnemigo == TipoEnemigoArma::conservador || TipoEnemigo == TipoEnemigoArma::liberal || TipoEnemigo == TipoEnemigoArma::esmadpistolon) {
            bool estaEnPlataforma = (y < 650.0f);
            float margenSeparacion = 58.0f;
            if (distanciaAbsX > margenSeparacion && !estaEnPlataforma) {
                vx = (distanciaX > 0) ? 45.0f : -45.0f;
            } else {
                vx = 0.0f;
            }
        } else {
            vx = 0.0f;
        }
    }
}

bool enemigo::puededisparar(){
    if (estadoActual != EstadoEnemigo::disparando || !TieneArma) return false;
    if (Stagger > 0.0f) return false;

    int intervaloDisparo = 1200;
    TipoEnemigoArma tipo = GETTipoEnemigo();

    if (tipo == TipoEnemigoArma::lanzallamas) {
        if (!estaLanzandoLlama()) return false;
        intervaloDisparo = 260;

    } else if (tipo == TipoEnemigoArma::esmadpistolon) {
    intervaloDisparo = 1400;

    } else if (tipo == TipoEnemigoArma::lanzagranadas) {
    intervaloDisparo = 1800;
    }

    if (temporizadorDisparo.elapsed() > intervaloDisparo){
        temporizadorDisparo.restart();
        return true;
    }
    return false;
}

void enemigo::disparar() {
}

void enemigo::recibirdano(int dano) {
    if (dano <= 0 || estadoActual == EstadoEnemigo::destruido) return;

    vida -= static_cast<short>(dano);
    if (vida < 0) vida = 0;

    animacionDano = true;
    temporizadorDano.restart();
    Stagger = std::min(0.85f, Stagger + 0.28f);

    if (vida <= 0) {
        estadoActual = EstadoEnemigo::destruido;
        animacionDano = false;
        Stagger = 0.0f;
    }
}

bool enemigo::estaEnAnimacionDano() const {
    return animacionDano;
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
float enemigo::GETX() const {
    return x;
}
float enemigo::GETY() const {
    return y;
}
void enemigo::setX(float nuevaX) {
    x = nuevaX;
}
short enemigo::GETVida() const {
    return vida;
}
bool enemigo::estaLanzandoLlama() const {
    if (TipoEnemigo != TipoEnemigoArma::lanzallamas || estadoActual != EstadoEnemigo::disparando) {
        return false;
    }
    long long ciclo = temporizadorRafagaLlamas.elapsed() % 2250;
    return ciclo < 750;
}
    long long enemigo::tiempoDesdeDisparo() const {
    return temporizadorDisparo.elapsed();
}
    void enemigo::setRangoVisionY(float nuevoRango) {
        rangoVisionY = nuevoRango;
    }