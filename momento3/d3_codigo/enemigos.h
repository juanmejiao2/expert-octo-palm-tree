#ifndef ENEMIGOS_H
#define ENEMIGOS_H
#include <QElapsedTimer>
#include <QRectF>

enum EstadoEnemigo{ idle, alerta, disparando, destruido};

enum TipoEnemigoArma {conservador = 0, liberal = 1, esmadpistolon =2, lanzallamas = 3, lanzagranadas = 4};

class enemigo{
protected:
    float x, y;               // posición
    float vx, vy;             // velocidad
    short vida;
private:
    EstadoEnemigo estadoActual;
    QElapsedTimer temporizadorDisparo;
    QElapsedTimer temporizadorRafagaLlamas;
    float rangoVision;
    float rangoVisionY;
    bool TieneArma;
    float tiempoPatrulla;
    bool MostrarAlerta;
    QElapsedTimer temporizadoralerta;
    TipoEnemigoArma TipoEnemigo ;

public:
    enemigo();
    enemigo(float _x, float _y, bool _tienearma=true, TipoEnemigoArma _tipo= TipoEnemigoArma::conservador, short vidaInicial = 100);
    virtual ~enemigo();
    virtual void mover(float dt);
    void actualizar(float xjugador, float yjugador);
    bool puededisparar();
    void disparar();
    void recibirdano(int dano);
    EstadoEnemigo GETEstado() const;
    TipoEnemigoArma GETTipoEnemigo() const;
    bool MostrarLaAlerta() const;
    virtual QRectF GETHitbox() const;
    float GETX() const;
    float GETY() const;
    void setX(float nuevaX);
    short GETVida() const;
    bool estaLanzandoLlama() const;
    long long tiempoDesdeDisparo() const;
    void setRangoVisionY(float nuevoRango);
    int frameAnimActual(int relojAnim) const;
    bool estaEnFrameDeImpacto(int relojAnim) const;
};

#endif // ENEMIGOS_H
