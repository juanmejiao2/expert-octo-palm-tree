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
    spriteToldo.load("toldo_rojo.png");

    jugador=new ::jugador(100.0f, 660.0f);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    timer->start(16);

    Plataforma.clear();
    Plataforma.append(new plataformas(300.0f, 530.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(520.0f, 420.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(700.0f, 340.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(900.0f, 480.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(1120.0f, 360.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(1300.0f, 500.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(1520.0f, 390.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(1700.0f, 280.0f, 180.0f, 20.0f));
    Plataforma.append(new plataformas(1920.0f, 450.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(2100.0f, 330.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(2320.0f, 490.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(2500.0f, 370.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(2720.0f, 260.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(2900.0f, 410.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(3120.0f, 300.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(3300.0f, 460.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(3520.0f, 340.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(3700.0f, 510.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(3920.0f, 390.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(4100.0f, 270.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(4320.0f, 440.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(4500.0f, 320.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(4720.0f, 470.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(4900.0f, 350.0f, 180.0f, 20.0f));
    Plataforma.append(new plataformas(5120.0f, 430.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(5300.0f, 290.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(5520.0f, 400.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(5700.0f, 320.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(5920.0f, 480.0f, 180.0f, 20.0f));
    Plataforma.append(new plataformas(6150.0f, 350.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(6380.0f, 430.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(6600.0f, 280.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(6830.0f, 410.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(7050.0f, 320.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(7280.0f, 490.0f, 180.0f, 20.0f));
    Plataforma.append(new plataformas(7500.0f, 360.0f, 160.0f, 20.0f));

    enemigos.clear();
    enemigos.append(new enemigo(700.0f, 280.0f, true));
    enemigos.append(new enemigo(1120.0f, 300.0f, true));
    enemigos.append(new enemigo(1700.0f, 220.0f, true));
    enemigos.append(new enemigo(2100.0f, 270.0f, true));
    enemigos.append(new enemigo(2720.0f, 200.0f, true));
    enemigos.append(new enemigo(3120.0f, 240.0f, true));
    enemigos.append(new enemigo(3920.0f, 330.0f, true));
    enemigos.append(new enemigo(4500.0f, 260.0f, true));
    enemigos.append(new enemigo(4900.0f, 290.0f, true));
    enemigos.append(new enemigo(5300.0f, 230.0f, true));
    enemigos.append(new enemigo(5920.0f, 420.0f, true));
    enemigos.append(new enemigo(6380.0f, 370.0f, true));
    enemigos.append(new enemigo(6830.0f, 350.0f, true));
    enemigos.append(new enemigo(7500.0f, 300.0f, true));
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
        float yPlataforma = plat->GETY();
        float piesjugador = jugador->GETY();
        bool cayendo = (jugador->GETVy() >= 0.0f);

        bool tocandoArriba = (hitboxjugador.bottom() >= yPlataforma - 5.0f && hitboxjugador.bottom() <= yPlataforma + 8.0f);
        float margenBorde = 20.0f;
        bool colisionX = (hitboxjugador.right() - margenBorde > hitboxplataforma.left() && hitboxjugador.left() + margenBorde < hitboxplataforma.right());

        if (colisionX && cayendo && tocandoArriba) {
            jugador->setY(yPlataforma-22.0f);
            jugador->setVy(0.0f);
            jugador->setEnAire(false);
            enAlgunaPlataforma = true;
            break;
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
        QRectF hitboxBala(proyectiles[i]->GETX(), proyectiles[i]->GETY(), 8.0f, 4.0f);

        for (enemigo* enemigo : enemigos) {
            if (enemigo->GETEstado() != EstadoEnemigo::destruido) {
                QRectF hitboxenemigo = enemigo->GETHitbox();

                if (proyectiles[i]->GETDeJugador() && hitboxBala.intersects(hitboxenemigo)) {
                    enemigo->recibirdano(25);
                    delete proyectiles[i];
                    proyectiles.removeAt(i);
                    proyectilEliminado = true;
                    break;
                }
            }
        }
        if (proyectilEliminado) continue;

        if (!proyectiles[i]->GETDeJugador()) {
            QRectF hitboxBala(proyectiles[i]->GETX(), proyectiles[i]->GETY(), 8.0f, 4.0f);

            if (hitboxBala.intersects(jugador->GETHitbox())) {
                //todavía no sé como hacer las barras de vida o la vida de los enemigos (o jefes), pero acá voy a darle daño a los enemigos, así como ellos lo tienen arriba :p
                delete proyectiles[i];
                proyectiles.removeAt(i);
                proyectilEliminado = true;
                break;
            }
        }
        if (proyectilEliminado) continue;

        if (proyectiles[i]->GETDeJugador()) {
            for (plataformas* plat : Plataforma) {
                if (hitboxBala.intersects(plat->GETHitbox())) {
                    delete proyectiles[i];
                    proyectiles.removeAt(i);
                    proyectilEliminado = true;
                    break;
                }
            }
        }
        if (proyectilEliminado) continue;

        if (proyectiles[i]->GETY() >= 660.0f) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
            continue;
        }

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

    if (spriteToldo.isNull()) {
        painter.setPen(Qt::red);
        painter.drawText(width()/2, height()/2, "ERROR: No se encuentra toldo_rojo.png");
    }

    for (plataformas* plat : Plataforma) {
        QRectF caja = plat->GETHitbox();

        int platPantallaX = static_cast<int>(caja.x() - camaraX);
        int platPantallaY = static_cast<int>(caja.y());

        int offsetVisualY = -5;
        int alturaVisualSprite = 60;

        painter.drawPixmap(platPantallaX, platPantallaY + offsetVisualY,static_cast<int>(caja.width()), alturaVisualSprite,spriteToldo);
    }
}