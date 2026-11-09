#ifndef PROYECTIL_H
#define PROYECTIL_H
#include "entidad.h"

enum TipoMovimientoProyectil {
    Rectilineo,
    Parabolico,
    Oscilatorio
};

class Proyectil : public entidad {
private:
    float velocidadX;
    float velocidadY;
    bool deJugador;
    TipoMovimientoProyectil tipoMovimiento;
    float tiempoAcumulado;

public:
    Proyectil(float x, float y, float vx, float vy, bool esDelJugador, TipoMovimientoProyectil tipo);

    void mover(float dt) override;
    void movRectilineo(float dt);
    void movParabolico(float dt);
    void movOscilatorio(float dt);
    bool GETDeJugador() const;
    QRectF GETHitbox() const override;
    TipoMovimientoProyectil GETTipoMovimiento() const;
};

#endif // PROYECTIL_H