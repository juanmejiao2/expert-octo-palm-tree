#ifndef CAMIONESMAD_H
#define CAMIONESMAD_H
#include "enemigos.h"
#include "jugador.h"
#include "proyectil.h"
#include <QList>
#include <QPointF>
#include <QRectF>
#include <random>

enum AtaqueCamion {
    AtaqueSierras = 0,
    AtaqueGranadasCamion = 1,
    AtaqueRafagaContencion = 2
};

class CamionEsmad : public enemigo {
public:
    static constexpr float PISO_ARENA = 660.0f;
    static constexpr float ARENA_MIN_X = 7900.0f;
    static constexpr float ARENA_MAX_X = 9180.0f;
    static constexpr float X_CAMION_BASE = 8980.0f;
    static constexpr float Y_CAMION_BASE = 660.0f;
private:
    std::mt19937 rng;

    bool fase2;
    bool cambioDeFasePendiente;
    bool derrotaRegistrada;
    short vidaMaxima;
    float cooldownAtaque;
    float cadenciaBase;
    float cooldownSierras;
    float factorDanio;
    float tasaAprendizaje;
    float exploracion;
    bool usaPrediccion;
    int cantidadSierras;
    int cantidadGranadas;
    int balasRafaga;
    int sierrasPendientes;
    float timerSiguienteSierra;
    float xJugador;
    float yJugador;
    float vxJugador;
    float xAnterior;
    bool jugadorEnAire;
    bool primeraLectura;
    float tiempoEnMitadIzq;
    float tiempoEnMitadDer;
    float efectividad[3];
    int fallosConsecutivos[3];
    int usos[3];
    int ultimoAtaque;
    int repeticiones;
    float tiempoMotor;
    float destelloImpacto;
    float aleatorio();
    void configurarDificultad(int dificultad);
    void percibir(const jugador &j, float dt);
    float puntoObjetivoX(float tiempoVuelo) const;
    AtaqueCamion razonar();
    void actuar(AtaqueCamion ataque, QList<Proyectil*> &proyectiles);
    float tiempoFogonazo;
    float timerCicloHumo;

public:
    explicit CamionEsmad(int dificultad = 0);

    void actualizar(float dt, const jugador &j, QList<Proyectil*> &proyectiles);
    void mover(float dt) override;
    QRectF GETHitbox() const override;
    void aprender(int ataque, bool acerto);
    bool recibirDisparo(int danio);
    int escalarDanio(int danioBase) const;
    bool enFase2() const;
    bool consumirCambioDeFase();
    bool enPeligro() const;
    short GETVidaMaxima() const;
    float GETDestello() const;
    int GETUltimoAtaque() const;
    QPointF GETCanonPos() const;
    float GETTiempoFogonazo() const;
    bool estaSacandoHumo() const;
    float GETProgresoHumo() const;
};

#endif // CAMIONESMAD_H
