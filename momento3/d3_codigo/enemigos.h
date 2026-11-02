#ifndef ENEMIGOS_H
#define ENEMIGOS_H
#include "entidad.h"
#include <QElapsedTimer>

enum EstadoEnemigo{ idle, alerta, disparando, destruido};

class enemigo:public entidad{
private:
    EstadoEnemigo estadoActual;
    QElapsedTimer temporizadorDisparo;
    float rangoVision;
    bool TieneArma;

    bool MostrarAlerta;
    QElapsedTimer temporizadoralerta;

public:
    enemigo();
    enemigo(float _x, float _y, bool _tienearma=true);
    virtual ~enemigo() override;
    virtual void mover(float dt) override;
    void actualizar(float xjugador, float yjugador);
    bool puededisparar();
    void disparar();
    void recibirdano(int dano);

    EstadoEnemigo GETEstado() const;
    bool MostrarLaAlerta() const;
};

#endif // ENEMIGOS_H
