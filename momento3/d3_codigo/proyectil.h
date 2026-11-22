#ifndef PROYECTIL_H
#define PROYECTIL_H
#include <QRectF>

enum TipoMovimientoProyectil {
    Rectilineo,
    Parabolico,
    Oscilatorio
};

class Proyectil{
private:
    float x, y;
    float velocidadX;
    float velocidadY;
    bool deJugador;
    TipoMovimientoProyectil tipoMovimiento;
    float tiempoAcumulado;

public:
    Proyectil(float x, float y, float vx, float vy, bool esDelJugador, TipoMovimientoProyectil tipo);

    void mover(float dt) ;
    void movRectilineo(float dt);
    void movParabolico(float dt);
    void movOscilatorio(float dt);
    bool GETDeJugador() const;
    QRectF GETHitbox() const;
    float GETX() const;
    float GETY() const;
    TipoMovimientoProyectil GETTipoMovimiento() const;
};

#endif // PROYECTIL_H