#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
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

    enemigos.append(new enemigo(700.0f, 360.0f, true));
    enemigos.append(new enemigo(1050.0f, 270.0f, true));;

    Plataforma.clear();
    Plataforma.append(new plataformas(300.0f, 520.0f, 220.0f, 20.0f));
    Plataforma.append(new plataformas(650.0f, 420.0f, 250.0f, 20.0f));
    Plataforma.append(new plataformas(1000.0f, 330.0f, 200.0f, 20.0f));

}

MainWindow::~MainWindow() {
    delete jugador;
    qDeleteAll(proyectiles);
    proyectiles.clear();
    qDeleteAll(Plataforma);
    Plataforma.clear();
    delete ui;
}

void MainWindow::actualizarJuego(){
    jugador->mover(0.016f);

    bool enAlgunaPlataforma = false;

    for (plataformas* plat : Plataforma) {
        QRectF hitboxplataforma = plat->GETHitbox();
        QRectF hitboxjugador = jugador->GETHitbox();
        bool colisionX = (hitboxjugador .right() > hitboxplataforma.left() && hitboxjugador .left() < hitboxplataforma.right());

        if (colisionX) {
            float yPlataforma = plat->GETY();

            if (jugador->GETY() >= yPlataforma - 10.0f && jugador->GETY() <= yPlataforma + 15.0f && jugador->GETVy() >= 0.0f) {
                jugador->setY(yPlataforma);
                jugador->setVy(0.0f);
                jugador->setEnAire(false);
                enAlgunaPlataforma = true;
                break;
            }
        }
    }
    if (!enAlgunaPlataforma && jugador->GETY() < 660.0f) {
        jugador->setEnAire(true);
    }
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
        bool proyectilEliminado = false;

        for (enemigo* enemigo : enemigos) {
            if (enemigo->GETEstado() != EstadoEnemigo::destruido) {
                QRectF hitboxenemigo= enemigo->GETHitbox();

                if (proyectiles[i]->GETDeJugador() && hitboxenemigo.contains(QPointF(proyectiles[i]->GETX(), proyectiles[i]->GETY()))) {
                    enemigo->recibirdano(25);
                    delete proyectiles[i];
                    proyectiles.removeAt(i);
                    proyectilEliminado = true;
                    break;
                }
            }
        }
        if (proyectilEliminado) continue;

        if (proyectiles[i]->GETX() < camaraX - 200 || proyectiles[i]->GETX() > camaraX + width() + 200) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }
    for (enemigo* enemigo : enemigos) {
        enemigo->actualizar(jugador->GETX(), jugador->GETY());

        if (enemigo->puededisparar()) {
            float origenX = enemigo->GETX();
            float origenY = enemigo->GETY() + 20;
            float dirX = jugador->GETX() - origenX;
            float dirY = jugador->GETY() - origenY;
            float distancia = std::sqrt(dirX * dirX + dirY * dirY);

            float velocidadBala = 600.0f;
            float velocidadBalaX = 600.0f;
            float velocidadBalaY = 0.0f;

            if (distancia > 0.0f) {
                velocidadBalaX = (dirX / distancia) * velocidadBala;
                velocidadBalaY = (dirY / distancia) * velocidadBala;
            }

            proyectiles.append(new Proyectil(origenX, origenY, velocidadBalaX, velocidadBalaY, false, TipoMovimientoProyectil::Rectilineo));
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

    int indiceFrame = 0;
    int spriteY = 1669;
    int totalFramesFila = 12;
    float anchoTiraFila = 932.0f;
    int offsetXInicial = 4;
    int spriteAlto = 97;
    int spriteX = 0;
    int spriteAncho = 0;
    bool usarcoordenadasmanuales=false;

    if (jugador->GETAnimacionGolpes()) {
        spriteY = 251;
        spriteAlto = 88;
        totalFramesFila = 5;
        indiceFrame = jugador->GETFrameGolpes() % totalFramesFila;
        offsetXInicial = -1;

        static const int coordenadasX[5] = {2, 180, 380, 587, 773};
        static const int anchos[5]  = {77, 106, 89, 81, 76};

        spriteX = coordenadasX[indiceFrame];
        spriteAncho = anchos[indiceFrame];
        usarcoordenadasmanuales = true;

    } else {
        if (jugador->GETEnElAire()) {
            if (jugador->GETDisparandoAnim()) {
                spriteY=1779;
                totalFramesFila=6;
                anchoTiraFila=780.0f;
                offsetXInicial= -4;
                indiceFrame=1;
            } else {
                spriteY=1438;
                totalFramesFila=3;
                anchoTiraFila=185.0f;
                offsetXInicial=362;

                static int frameSaltoActual=0;
                static int temporizadorSalto=0;

                temporizadorSalto++;
                if (temporizadorSalto>=16) {
                    frameSaltoActual=(frameSaltoActual+1) % 3;
                    temporizadorSalto= 0;
                }
                indiceFrame=frameSaltoActual;
            }
        } else if (jugador->GETDisparandoAnim()) {
            spriteY=1779;
            totalFramesFila=6;
            anchoTiraFila=780.0f;
            offsetXInicial= -4;
            indiceFrame=jugador->GETFrameDisparo() % 6;

        } else {
            if (jugador->GETMunicion() > 0) {
                if (jugador->GETEnMovimiento() && !jugador->GETEnElAire()) {
                    indiceFrame = jugador->GETFrameActual() % 12;
                    spriteY= 1669;
                    spriteAlto= 97;
                    totalFramesFila= 12;
                    anchoTiraFila= 932.0f;
                    offsetXInicial= 4;

                } else {
                    spriteY = 2937;
                    spriteAlto = 97;
                    totalFramesFila = 2;

                    static int frameIdlePistolaCustom = 0;
                    static int contadorIdlePistola = 0;
                    contadorIdlePistola++;

                    if (contadorIdlePistola >= 60) {
                        frameIdlePistolaCustom = (frameIdlePistolaCustom + 1) % 2;
                        contadorIdlePistola = 0;
                    }
                    indiceFrame = frameIdlePistolaCustom;
                    static const int coordsX[5] = {408, 21};
                    static const int anchos[5]  = {64, 64};

                    spriteX = coordsX[indiceFrame];
                    spriteAncho = anchos[indiceFrame];

                    usarcoordenadasmanuales = true;
                }
            } else {
                if (jugador->GETEnMovimiento() && !jugador->GETEnElAire()) {
                    indiceFrame= jugador->GETFrameActual() % 12;
                    spriteY=137;
                    spriteAlto=86;
                    totalFramesFila=12;
                    anchoTiraFila=1180.0f;
                    offsetXInicial=3;

                } else {
                    static int frameIdleSinBalas= 0;
                    static int contadorIdleSinBalas= 0;
                    contadorIdleSinBalas++;

                    if (contadorIdleSinBalas>=15) {
                        frameIdleSinBalas=(frameIdleSinBalas+1)% 3;
                        contadorIdleSinBalas=0;
                    }
                    indiceFrame=frameIdleSinBalas;
                    spriteY=18;
                    spriteAlto=99;
                    totalFramesFila=3;
                    anchoTiraFila=245.0f;
                    offsetXInicial=28;
                }
            }
        }
        if (!usarcoordenadasmanuales) {
            spriteX = offsetXInicial + qRound(indiceFrame * (anchoTiraFila / totalFramesFila));
            int siguienteX = offsetXInicial + qRound((indiceFrame + 1) * (anchoTiraFila / totalFramesFila));
            spriteAncho = (siguienteX - spriteX);
        }
    }

    if (camaraX > anchoMaximoNivel - width()) {
        camaraX = anchoMaximoNivel - width();
    }

    if (camaraX < 0.0f) {
        camaraX = 0.0f;
    }



    QPixmap framesPunisher = spriteSheetPunisher.copy(spriteX, spriteY, spriteAncho, spriteAlto);

    float escalaAumento = 1.3f;

    QPixmap framesPunisherEscalado = framesPunisher.scaled(qRound(spriteAncho * escalaAumento),qRound(spriteAlto * escalaAumento),Qt::KeepAspectRatio,Qt::SmoothTransformation);

    int pantallaX = static_cast<int>(jugador->GETX() - camaraX);
    int offsetPisoVisual = 30;
    if (jugador->GETMunicion() == 0 && !jugador->GETAnimacionGolpes()) {
        offsetPisoVisual = 38;
    }
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

    for (Proyectil* p : proyectiles) {
        int projPantallaX = static_cast<int>(p->GETX() - camaraX);
        int projPantallaY = static_cast<int>(p->GETY());

        painter.setBrush(Qt::yellow);
        painter.drawEllipse(projPantallaX, projPantallaY, 8, 4);
    }

    for (plataformas* plat : Plataforma) {
        int platX = static_cast<int>(plat->GETX() - camaraX);
        int platY = static_cast<int>(plat->GETY());
        QRectF caja = plat->GETHitbox();
        painter.fillRect(platX, platY, static_cast<int>(caja.width()), static_cast<int>(caja.height()), Qt::darkGreen);
    }
}