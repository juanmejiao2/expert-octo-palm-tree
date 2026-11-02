#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    moviendoDerecha = false;
    moviendoIzquierda = false;
    camaraX = 0.0f;

    this->setFixedSize(1280,720);

    spriteSheetPunisher.load("Punisher_sprites.png");

    jugador=new ::jugador(100.0f, 660.0f);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    timer->start(16);

    enemigos.append(new enemigo(800.0f, 400.0f, true));
    enemigos.append(new enemigo(1600.0f, 350.0f, true));
}

MainWindow::~MainWindow() {
    delete jugador;
    qDeleteAll(proyectiles);
    proyectiles.clear();
    delete ui;
}

void MainWindow::actualizarJuego(){
    jugador->mover(0.016f);

    if (jugador->GETX() - camaraX > 640.0f) {
        camaraX = jugador->GETX() - 640.0f;
    }
    else if (jugador->GETX() - camaraX < 400.0f && camaraX > 0.0f) {
        camaraX = jugador->GETX() - 400.0f;
    }

    QPixmap fondo1bog("BG_nivel1.png");
    QPixmap fondoEscalado = fondo1bog.scaledToHeight(height(), Qt::SmoothTransformation);
    float anchoMaximoNivel = fondoEscalado.width() * 5.0f;

    if (camaraX < 0.0f){
        camaraX = 0.0f;
    }
    if (camaraX > anchoMaximoNivel - width()) {
        camaraX = anchoMaximoNivel - width();
    }
    float margenVisual = 25.0f;
    if (jugador->GETX() < camaraX + margenVisual) {
        jugador->setX(camaraX + margenVisual);
        jugador->setVelocidadX(0.0f);
    }
    float margenDerecho = 80.0f;

    if (jugador->GETX() > camaraX + width() - margenDerecho) {
        jugador->setX(camaraX + width() - margenDerecho);
        jugador->setVelocidadX(0.0f);
    }

    for (enemigo* enemigo : enemigos) {
        enemigo->actualizar(jugador->GETX(), jugador->GETY());
    }

    for (int i = proyectiles.size() - 1; i >= 0; --i) {
        proyectiles[i]->mover(0.016f);
        if (proyectiles[i]->GETX() < camaraX - 200 || proyectiles[i]->GETX() > camaraX + width() + 200) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }

    for (enemigo* enemigo : enemigos) {
        enemigo->actualizar(jugador->GETX(), jugador->GETY());

        if (enemigo->puededisparar()) {
            float velocidadBalaX = (jugador->GETX() < enemigo->GETX()) ? -600.0f : 600.0f;

            proyectiles.append(new Proyectil(
            enemigo->GETX(),
            enemigo->GETY() + 20,
            velocidadBalaX,
            0.0f,
            false,
            TipoMovimientoProyectil::Rectilineo));

        }
    }
    update();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->isAutoRepeat()) return;

    if(event->key() == Qt::Key_Space){
        jugador->saltar();
    }
    else if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) {
        moviendoDerecha=true;
        jugador->setVelocidadX(300.0f);
    }
    else if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) {
        moviendoIzquierda=true;
        jugador->setVelocidadX(-300.0f);
    }
    else if (event->key() == Qt::Key_J || event->key()==Qt::Key_Z) {
        Proyectil* nuevoProyectil = jugador->disparar();
        if (nuevoProyectil != nullptr) {
            proyectiles.append(nuevoProyectil);
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) {
        moviendoDerecha = false;
        if (moviendoIzquierda) {
            jugador->setVelocidadX(-300.0f);
        } else {
            jugador->setVelocidadX(0.0f);
        }
    }
    else if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) {
        moviendoIzquierda = false;
        if (moviendoDerecha) {
            jugador->setVelocidadX(300.0f);
        } else {
            jugador->setVelocidadX(0.0f);
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPixmap fondo1bog("BG_lvl1.jpg");
    QPixmap fondoEscalado = fondo1bog.scaledToHeight(height(), Qt::SmoothTransformation);

    int anchoFondo = fondoEscalado.width();
    float anchoMaximoNivel = anchoFondo * 5.0f;


    if (camaraX > anchoMaximoNivel - width()) {
        camaraX = anchoMaximoNivel - width();
    }
    if (camaraX < 0.0f) {
        camaraX = 0.0f;
    }

    int desplazamientoX = -static_cast<int>(camaraX);
    int indiceFondo = desplazamientoX % anchoFondo;
    if (indiceFondo > 0) indiceFondo -= anchoFondo;

    for (int xPos = indiceFondo; xPos < width(); xPos += anchoFondo) {
        painter.drawPixmap(xPos, 0, fondoEscalado);
    }

    int indiceFrame= 0;
    int spriteY= 1669;
    int totalFramesFila= 12;
    float anchoTiraFila= 932.0f;
    int offsetXInicial= 4;
    int offsetPiesY= 0;

    if (jugador->GETEnElAire()) {
        if (jugador->GETDisparandoAnim()) {
            // --- DISPARO EN EL AIRE (Frame fijo de la balacera) ---
            spriteY = 1779;          // Fila del disparo
            totalFramesFila = 6;
            anchoTiraFila = 780.0f;
            offsetXInicial = -4;
            indiceFrame = 1;         // Frame 1 fijo para que muestre el fogonazo en el aire
        } else {
            // --- SALTO NORMAL ---
            spriteY = 1438;
            totalFramesFila = 3;
            anchoTiraFila = 185.0f;
            offsetXInicial = 362;

            static int frameSaltoActual = 0;
            static int temporizadorSalto = 0;

            temporizadorSalto++;
            if (temporizadorSalto >= 16) {
                frameSaltoActual = (frameSaltoActual + 1) % 3;
                temporizadorSalto = 0;
            }
            indiceFrame = frameSaltoActual;
        }
    } else if (jugador->GETDisparandoAnim()) {
        // --- DISPARO EN EL SUELO (Animación completa de 6 frames) ---
        spriteY = 1779;
        totalFramesFila = 6;
        anchoTiraFila = 780.0f;
        offsetXInicial = -4;
        indiceFrame = jugador->GETFrameDisparo() % 6;
    } else {
        // --- SUELO NORMAL (Caminar / Quieto) ---
        indiceFrame = jugador->GETFrameActual() % 12;
        spriteY = 1669;
        totalFramesFila = 12;
        anchoTiraFila = 932.0f;
        offsetXInicial = 4;
    }

    if (camaraX > anchoMaximoNivel - width()) {
        camaraX = anchoMaximoNivel - width();
    }

    if (camaraX < 0.0f) {
        camaraX = 0.0f;
    }

    int spriteX = offsetXInicial + qRound(indiceFrame * (anchoTiraFila / totalFramesFila));
    int siguienteX = offsetXInicial + qRound((indiceFrame + 1) * (anchoTiraFila / totalFramesFila));
    int spriteAncho = (siguienteX - spriteX) ;
    int spriteAlto = 97;

    QPixmap framesPunisher = spriteSheetPunisher.copy(spriteX, spriteY, spriteAncho, spriteAlto);

    float escalaAumento = 1.3f;
    QPixmap framesPunisherEscalado = framesPunisher.scaled(
        qRound(spriteAncho * escalaAumento),
        qRound(spriteAlto * escalaAumento),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    int pantallaX = static_cast<int>(jugador->GETX() - camaraX);
    int offsetPisoVisual = 30;
    int pantallaY = static_cast<int>(jugador->GETY() - (spriteAlto * escalaAumento)) + offsetPisoVisual;

    painter.save();

    if (!jugador->GETmirandoDerecha()) {
        painter.scale(-1, 1);
        pantallaX = -pantallaX - (spriteAncho * escalaAumento);
    }

    painter.drawPixmap(pantallaX, pantallaY, framesPunisherEscalado);

    painter.restore();

    for (enemigo* enemigo : enemigos) {
        if (enemigo->GETEstado() != EstadoEnemigo::destruido) {
            int enemigoPantallaX = static_cast<int>(enemigo->GETX() - camaraX);
            int enemigoPantallaY = static_cast<int>(enemigo->GETY());
            painter.fillRect(enemigoPantallaX, enemigoPantallaY, 40, 60, Qt::blue);

            if (enemigo->MostrarLaAlerta()) {
                QFont fuenteAlerta("Arial", 50, QFont::Bold);
                painter.setFont(fuenteAlerta);
                painter.setPen(Qt::red);
                int anchoRectangulo = 40;
                int centroEnemigoX = enemigoPantallaX + (anchoRectangulo / 2);
                int alturaAlertaY = enemigoPantallaY - 30;
                painter.drawText(centroEnemigoX - 10, alturaAlertaY, "!");
            }
        }
    }

    painter.setPen(Qt::white);
    QFont fuenteHud("Arial", 16, QFont::Bold);
    painter.setFont(fuenteHud);
    painter.drawText(30, 40, "MUNICIÓN: " + QString::number(jugador->GETMunicion()));
    painter.restore();
    for (Proyectil* p : proyectiles) {
        int projPantallaX = static_cast<int>(p->GETX() - camaraX);
        int projPantallaY = static_cast<int>(p->GETY());

        painter.setBrush(Qt::yellow);
        painter.drawEllipse(projPantallaX, projPantallaY, 8, 4);
    }

}