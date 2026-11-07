#ifndef JUGADOR_H
#define JUGADOR_H
#include "entidad.h"
#include "arma.h"
#include "proyectil.h"

class jugador: public entidad{

private:
    int frameActual;
    float tiempoAnimacion;
    bool enMovimiento;
    int contadorFrames;
    int contadorFramesDisparo;
    int frameDisparo;
    short municion;
    float pesoBase;
    bool enAire;
    bool mirandoDerecha;
    arma *armaJugador;
    bool disparandoAnimacion;
    int contadorAnimacionDisparo;
    bool animacionpuno;
    int frameGolpes;
    int contadorFramesGolpes;
    float cooldownDisparo;

public:
    jugador(float posicionX, float posicionY);
    ~jugador();
    void setVelocidadX(float nuevaVx);
    void saltar();
    Proyectil* disparar();
    void mover(float dt) override;
    bool GETEnElAire() const;
    bool GETmirandoDerecha() const;
    int GETFrameActual() const;
    void setEnMovimiento(bool mov);
    short GETMunicion() const;
    bool GETDisparandoAnim() const;
    int GETFrameDisparo() const;
    bool GETAnimacionGolpes() const;
    int GETFrameGolpes() const;
    bool GETEnMovimiento() const;
    QRectF GETHitbox() const override;
    void setEnAire(bool aire);
};

#endif // JUGADOR_H
