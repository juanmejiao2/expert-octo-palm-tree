#include "jugador.h"
#include "proyectil.h"

static float direccionDeseada = 0.0f;

jugador::jugador(float posicionX, float posicionY) : entidad(posicionX, posicionY, 100) {
    municion = 6;
    maxMunicion = 6;
    pesoBase = 5.0f;
    enAire = false;
    armaJugador = new arma(1, 25, 800.0f, 10);
    cooldownDisparo = 0.0f;
    animacionDano = false;
    contadorDano = 0;
}

jugador::~jugador() {
    delete armaJugador;
}

void jugador::saltar() {
    if (!enAire) {
        vy = -800.0f;
        enAire = true;
    }
}

Proyectil* jugador::disparar() {
    if (cooldownDisparo > 0.0f) {
        return nullptr;
    }
    if (municion > 0) {
        municion--;
        disparandoAnimacion = true;
        contadorAnimacionDisparo = 10;
        cooldownDisparo = 0.25f;
        float velocidadBalaX = mirandoDerecha ? 800.0f : -800.0f;
        float alturaPechoY = y - 70.0f;
        float offsetX = mirandoDerecha ? 35.0f : -15.0f;
        return new Proyectil(x + offsetX, alturaPechoY, velocidadBalaX, 0.0f, true, TipoMovimientoProyectil::Rectilineo);
    }else {
        animacionpuno=true;
        frameGolpes=0;
        contadorFramesGolpes=0;
        cooldownDisparo = 0.3f;
    }
    return nullptr;
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
        if (contadorFrames >= 6) {
            if (municion > 0) {
                frameActual = (frameActual + 1) % 12;
            } else {
                frameActual = (frameActual + 1) % 12;
            }
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
    if (disparandoAnimacion) {
        contadorFramesDisparo++;
        if (contadorFramesDisparo >= 4) {
            frameDisparo++;
            contadorFramesDisparo = 0;
            if (frameDisparo >= 6) {
                frameDisparo = 0;
                disparandoAnimacion = false;
            }
        }
    }
    if (cooldownDisparo > 0.0f) {
        cooldownDisparo -= dt;
    }

    if (animacionpuno) {
        contadorFramesGolpes++;
        if (contadorFramesGolpes >= 5) {
            frameGolpes++;
            contadorFramesGolpes = 0;
            if (frameGolpes >= 5) {
                frameGolpes = 0;
                animacionpuno = false;
            }
        }
    }
    if (animacionDano) {
        contadorDano++;
        if (contadorDano >= 15) {
            animacionDano = false;
            contadorDano = 0;
        }
    }
}

void jugador::iniciarGolpe() {
    if (!animacionpuno) {
        animacionpuno = true;
        frameGolpes = 0;
        contadorFramesGolpes = 0;
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

QRectF jugador::GETHitbox() const{
    return QRectF(x+20.0f, y - 70.0f, 45.0f, 95.0f);
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

short jugador::GETMunicion() const{
    return municion;
}

void jugador::setEnMovimiento(bool mov){
    enMovimiento=mov;
}

bool jugador::GETDisparandoAnim() const {
    return disparandoAnimacion;
}

int jugador::GETFrameDisparo() const {
    return frameDisparo;
}

bool jugador::GETAnimacionGolpes() const {
    return animacionpuno;
}

int jugador::GETFrameGolpes() const {
    return frameGolpes;
}
bool jugador::GETEnMovimiento() const {
    return enMovimiento;
}
void jugador::setEnAire(bool aire) {
    enAire = aire;
}
void jugador::setMunicion(short m) {
    municion = qMax((short)0, qMin(maxMunicion, m));
}
void jugador::reiniciarJugador(float posX, float posY) {
    x = posX;
    y = posY;
    vx = 0.0f;
    vy = 0.0f;
    vida = 100;
    municion = 6;
    enAire = false;
    animacionpuno = false;
    disparandoAnimacion = false;
}
void jugador::recibirDanoJugador(short cantidadDano) {
    vida -= cantidadDano;
    if (vida < 0) vida = 0;

    animacionDano = true;
    contadorDano = 0;
}

bool jugador::GETAnimacionDano() const {
    return animacionDano;
}

void jugador::setMaxMunicion(short maxM) {
    maxMunicion = maxM;
}
