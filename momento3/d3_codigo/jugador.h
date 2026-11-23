#ifndef JUGADOR_H
#define JUGADOR_H
#include <QRectF>
#include "proyectil.h"

class jugador{

private:
    int frameActual;
    float tiempoAnimacion;
    bool enMovimiento;
    int contadorFrames;
    int contadorFramesDisparo;
    int frameDisparo;
    short maxMunicion;
    short municion;
    float pesoBase;
    bool enAire;
    bool mirandoDerecha;
    short danoBala;
    float velocidadBala;
    bool disparandoAnimacion;
    int contadorAnimacionDisparo;
    bool animacionpuno;
    int frameGolpes;
    int contadorFramesGolpes;
    float cooldownDisparo;
    bool animacionDano;
    int contadorDano;
    float x, y;            // posición
    float vx, vy;          // velocidad
    short vida;
    float sueloY;          // altura del piso (cambia según el nivel)

public:
    jugador(float posicionX, float posicionY);
    ~jugador();
    void setVelocidadX(float nuevaVx);
    void saltar();
    Proyectil* disparar();
    void mover(float dt);
    bool GETEnElAire() const;
    bool GETmirandoDerecha() const;
    int GETFrameActual() const;
    void setEnMovimiento(bool mov);
    short GETMunicion() const;
    int GETMaxMunicion() const;
    bool GETDisparandoAnim() const;
    int GETFrameDisparo() const;
    bool GETAnimacionGolpes() const;
    int GETFrameGolpes() const;
    bool GETEnMovimiento() const;
    QRectF GETHitbox() const;
    void setEnAire(bool aire);
    void setMunicion(short m);
    void setMaxMunicion(short maxM);
    void iniciarGolpe();
    void reiniciarJugador(float posX, float posY);
    bool GETAnimacionDano() const;
    void recibirDanoJugador(short cantidadDano);
    bool apuntandoArriba;
    void setApuntandoArriba(bool arriba);
    bool GETApuntandoArriba() const;
    float GETX() const;
    float GETY() const;
    float GETVy() const;
    void setX(float nuevaX);
    void setY(float nuevaY);
    void setVy(float nuevaVy);
    short GETVida() const;
    short GETDanoBala() const;
    void setVida(short nuevaVida);
    void setSueloY(float nuevoSuelo);
};

#endif // JUGADOR_H
