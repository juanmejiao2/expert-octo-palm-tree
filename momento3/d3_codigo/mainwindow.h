#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "jugador.h"
#include "enemigos.h"
#include "proyectil.h"
#include "plataformas.h"

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
    QPixmap spriteSheetPunisher;
    QPixmap spriteToldo;
    QPixmap spriteBalaRecoger;
    QList<enemigo*> enemigos;
    QList<Proyectil*>proyectiles;
    QList<plataformas*> Plataforma;
    QList<BalaSuelo> balasEnMapa;
    float limiteIzquierdoCamara = 0.0f;
    float limiteDerechoCamara = 1400.0f;
    bool sectorLimpio = false;
    int enemigosEnSectorActual = 0;
    QPixmap pixmapGo;
};
#endif // MAINWINDOW_H
