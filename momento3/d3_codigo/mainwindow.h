#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <random>
#include <QTimer>
#include <QSet>
#include <QPair>
#include "jugador.h"
#include "enemigos.h"
#include "proyectil.h"
#include "plataformas.h"
#include "jefegaitan.h"
#include "camionesmad.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct BalaSuelo {
    float x;
    float yBase;
    bool recolectada;
};
struct CuraSuelo{
    float x;
    float ybase;
    bool recolectada;
};

struct ExplosionVisual {
    float x, y;
    int frameActual;
    float tiempoFrame;
    float escala= 1.0f;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizarJuego();
private:
    Ui::MainWindow *ui;
    QTimer *timer;
    jugador *jugador;
    bool moviendoDerecha;
    bool moviendoIzquierda;
    float camaraX;
    float camaraY;
    QPixmap fondoLvl1Escalado;
    QPixmap spriteGameOver;
    QPixmap spriteSheetPunisher;
    QPixmap spriteToldo;
    QPixmap spriteBalaRecoger;
    QPixmap spritePantallaInicio;
    QPixmap spriteCura;
    QPixmap spriteConservador;
    QPixmap spriteLiberal;
    QPixmap spriteLanzallamas;
    QPixmap spriteFuego;
    QPixmap spritePistolero;
    QPixmap spriteGranadero;
    QPixmap spriteGranada;
    QPixmap spriteCamionEsmad;
    QPixmap spriteSierra;
    QList<CuraSuelo> curasEnMapa;
    QList<enemigo*> enemigos;
    QList<QPair<enemigo*, plataformas*>> enemigosEnNube;
    QList<Proyectil*>proyectiles;
    QList<plataformas*> Plataforma;
    QList<BalaSuelo> balasEnMapa;
    QPixmap spriteExplosion;
    QList<ExplosionVisual> explosionesActivas;
    float limiteIzquierdoCamara = 0.0f;
    float limiteDerechoCamara = 1400.0f;
    bool sectorLimpio = false;
    int enemigosEnSectorActual = 0;
    QPixmap pixmapGo;
    bool juegoTerminado;
    void reiniciarNivel();
    void cargarNivel2();
    int nivelActual;
    JefeGaitan *jefe;
    bool peleaActiva;
    bool victoria;
    float recargaMunicion;
    void borrarJefe();
    bool enSeleccionDificultad;
    int dificultadJuego;
    int contadorFramesGO;
    CamionEsmad *camionEsmad;
    bool peleaCamionActiva;
    bool camionDerrotadoPendiente;
    float tiempoTransicionNivel2;
    float timerSiguienteExplosionCamion;
    int explosionesCamionEmitidas;
    std::mt19937 rngDrops;
    QSet<enemigo*> enemigosDropsGenerados;
};
#endif // MAINWINDOW_H
