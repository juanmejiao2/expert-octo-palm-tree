#ifndef JEFEGAITAN_H
#define JEFEGAITAN_H

#include <QList>
#include <QPointF>
#include <QRectF>
#include <random>
#include "enemigos.h"
#include "proyectil.h"
#include "jugador.h"

enum AtaqueGaitan { AtaquePendulo = 0, AtaqueLaser = 1, AtaqueLluvia = 2 };

class JefeGaitan : public enemigo {
private:
    float tasaAprendizaje;
    float exploracion;
    bool usaPrediccion;
    float cadenciaBase;
    float giroLaser;
    float duracionLaser;
    float factorDanio;
    int cantidadPendulos;
    int cantidadLluvia;

    short vidaMaxima;
    bool fase2;
    bool cambioDeFasePendiente;
    float cooldownAtaque;
    float tiempoFlotar;
    float yAncla;
    int zonaActual;
    int ataquesDesdeTeleport;
    float destello;

    int faseLaser;
    float tiempoLaser;
    float anguloLaser;
    bool laserGolpeo;
    float acumuladoLaser;

    float xJugador, yJugador;
    float vxJugador;
    float xAnterior;
    bool jugadorEnAire;
    bool primeraLectura;
    float tiempoEnZona[3];

    float efectividad[3];
    int usos[3];
    int ultimoAtaque;
    int repeticiones;
    std::mt19937 rng;

    void configurarDificultad(int dificultad);
    void percibir(const jugador &j, float dt);
    AtaqueGaitan razonar();
    void actuar(AtaqueGaitan ataque, QList<Proyectil*> &proyectiles);
    float puntoObjetivoX(float tiempoVuelo) const;
    int zonaDe(float xMundo) const;
    void teletransportar();
    float aleatorio();

public:
    static constexpr float ZONAS_X[3] = {360.0f, 640.0f, 920.0f};
    static constexpr float ALTURA_VUELO = -2540.0f;
    static constexpr float PISO_ARENA = -2280.0f;

    explicit JefeGaitan(int dificultad);
    void actualizar(float dt, const jugador &j, QList<Proyectil*> &proyectiles);
    void aprender(int ataque, bool acerto);
    void mover(float dt) override;
    QRectF GETHitbox() const override;
    bool recibirDisparo(int danio);
    bool consumirCambioDeFase();
    bool enFase2() const;
    short GETVidaMaxima() const;
    float GETDestello() const;
    int escalarDanio(int danioBase) const;

    // rayo láser
    int GETFaseLaser() const;
    QPointF GETOjos() const;
    QPointF GETFinLaser() const;
    int aplicarLaser(const QRectF &hitboxJugador, float dt);
};

#endif // JEFEGAITAN_H