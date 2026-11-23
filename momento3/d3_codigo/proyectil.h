#ifndef PROYECTIL_H
#define PROYECTIL_H
#include <QRectF>

enum TipoMovimientoProyectil {
    Rectilineo,
    Parabolico,
    Oscilatorio,
    Pendulo
};

class Proyectil{
private:
    float x, y;
    float velocidadX;
    float velocidadY;
    bool deJugador;
    TipoMovimientoProyectil tipoMovimiento;
    float tiempoAcumulado;
    int ataqueOrigen;
    float pivoteX, pivoteY, largo, theta, omegaPendulo;

public:
    Proyectil(float x, float y, float vx, float vy, bool esDelJugador, TipoMovimientoProyectil tipo);

    void mover(float dt) ;
    void movRectilineo(float dt);
    void movParabolico(float dt);
    void movOscilatorio(float dt);
    void movPendulo(float dt);
    void configurarPendulo(float pivX, float pivY, float largoCuerda, float anguloInicial);
    void setAtaqueOrigen(int ataque);
    int GETAtaqueOrigen() const;
    float GETPivoteX() const;
    float GETPivoteY() const;
    bool GETDeJugador() const;
    QRectF GETHitbox() const;
    float GETX() const;
    float GETY() const;
    TipoMovimientoProyectil GETTipoMovimiento() const;
};

#endif // PROYECTIL_H