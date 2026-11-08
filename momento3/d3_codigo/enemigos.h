#ifndef ENEMIGOS_H
#define ENEMIGOS_H
#include "entidad.h"
#include <QElapsedTimer>

enum EstadoEnemigo{ idle, alerta, disparando, destruido};

enum TipoEnemigoArma {
    conservador = 0,
    liberal = 1,
    lanzallamas = 2,
    lanzagranadas = 3
};

class enemigo:public entidad{
private:
    EstadoEnemigo estadoActual;
    QElapsedTimer temporizadorDisparo;
    float rangoVision;
    bool TieneArma;

    bool MostrarAlerta;
    QElapsedTimer temporizadoralerta;
    TipoEnemigoArma TipoEnemigo ;

public:
    enemigo();
    enemigo(float _x, float _y, bool _tienearma=true, TipoEnemigoArma _tipo= TipoEnemigoArma::conservador);
    virtual ~enemigo() override;
    virtual void mover(float dt) override;
    void actualizar(float xjugador, float yjugador);
    bool puededisparar();
    void disparar();
    void recibirdano(int dano);
    EstadoEnemigo GETEstado() const;
    TipoEnemigoArma GETTipoEnemigo() const;
    bool MostrarLaAlerta() const;
    QRectF GETHitbox() const override;
};

#endif // ENEMIGOS_H
