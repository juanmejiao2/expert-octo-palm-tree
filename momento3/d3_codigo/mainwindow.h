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
    QList<enemigo*> enemigos;
    QList<Proyectil*>proyectiles;
    QList<plataformas*> Plataforma;
};
#endif // MAINWINDOW_H
