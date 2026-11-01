#ifndef JUGADOR_H
#define JUGADOR_H
#include "entidad.h"

class jugador: public entidad{

private:
    int frameActual;
    float tiempoAnimacion;
    bool enMovimiento;
    int contadorFrames;
    short municion;
    float pesoBase;
    bool enAire;
    bool mirandoDerecha;
public:
    jugador(float posicionX, float posicionY);
    void setVelocidadX(float nuevaVx);
    void saltar();
    void disparar();
    void mover(float dt) override;
    bool GETEnElAire() const;
    bool GETmirandoDerecha() const;
    int GETFrameActual() const;
    void setEnMovimiento(bool mov);

};

#endif // JUGADOR_H
