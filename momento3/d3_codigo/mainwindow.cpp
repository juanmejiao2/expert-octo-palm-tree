#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    moviendoDerecha = false;
    moviendoIzquierda = false;
    camaraX = 0.0f;
    camaraY = 0.0f;
    enSeleccionDificultad = true;
    dificultadJuego = 0;
    contadorFramesGO = 0;
    nivelActual = 1;

    this->setFixedSize(1280,720);

    juegoTerminado = false;
    spriteGameOver.load("game_over.jpg");
    spriteSheetPunisher.load("Punisher_sprites.png");
    spriteToldo.load("toldo_rojo.png");
    pixmapGo.load("GO.png");
    spriteBalaRecoger.load("municion_box.png");
    spritePantallaInicio.load("pantalla_inicio.jpg");
    spriteExplosion.load("explosion.png");
    spriteCura.load("HEAL.png");
    spriteConservador.load("Conservadores_sprites.png");
    spriteLiberal.load("Liberales_sprites.png");
    spritePistolero.load("esmad_pistola.png");
    spriteGranadero.load("esmad_granadero.png");
    spriteCarro.load("carro_en_llamas.png");
    spriteGranada.load("granada.png");
    QPixmap fuego("fuego.png");
    if (!fuego.isNull()) {
        spriteFuego = fuego;
    }
    QPixmap Lanzallamas("lanzallamero.png");
    if (!Lanzallamas.isNull()){
        spriteLanzallamas= Lanzallamas;
    }
    jugador = nullptr;

    QPixmap fondolvl1("BG_lvl1.jpg");
    if (!fondolvl1.isNull()) {
        fondoLvl1Escalado = fondolvl1.scaledToHeight(height(), Qt::SmoothTransformation);
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    timer->start(16);
    reiniciarNivel();
}

void MainWindow::reiniciarNivel() {
    if(jugador != nullptr){
        delete jugador;
        jugador=nullptr;
    }
    qDeleteAll(proyectiles);
    proyectiles.clear();
    qDeleteAll(Plataforma);
    Plataforma.clear();
    qDeleteAll(enemigos);
    enemigos.clear();

    camaraX = 0.0f;
    camaraY = 0.0f;
    juegoTerminado = false;
    sectorLimpio = false;
    nivelActual = 1;
    jugador = new ::jugador(100.0f, 660.0f);
    if (dificultadJuego == 0) {
        jugador->setMaxMunicion(12);
        jugador->setMunicion(12);
    } else {
        jugador->setMaxMunicion(10);
        jugador->setMunicion(10);
    }

    Plataforma.clear();
    //Primera parte del nivel
    Plataforma.append(new plataformas(280.0f, 420.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(500.0f, 200.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(740.0f, 360.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(960.0f, 170.0f, 130.0f, 20.0f));
    Plataforma.append(new plataformas(1200.0f, 350.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(1440.0f, 190.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(1670.0f, 420.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(1900.0f, 160.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(2140.0f, 320.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(2360.0f, 180.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(170.0f, 510.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(620.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(1080.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(1550.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(2020.0f, 480.0f, 90.0f, 20.0f));
    //Segunda parte del nivel
    Plataforma.append(new plataformas(2600.0f, 370.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(2820.0f, 180.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(3060.0f, 340.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(3280.0f, 160.0f, 130.0f, 20.0f));
    Plataforma.append(new plataformas(3520.0f, 400.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(3740.0f, 200.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(3980.0f, 350.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(4220.0f, 170.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(4460.0f, 380.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(4680.0f, 190.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(4920.0f, 310.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(5140.0f, 150.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(2710.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(3170.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(3630.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(4100.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(4570.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(5030.0f, 480.0f, 90.0f, 20.0f));
    //Tercera parte del nivel
    Plataforma.append(new plataformas(5360.0f, 360.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(5580.0f, 180.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(5820.0f, 340.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(6040.0f, 160.0f, 170.0f, 20.0f));
    Plataforma.append(new plataformas(6280.0f, 390.0f, 130.0f, 20.0f));
    Plataforma.append(new plataformas(6500.0f, 190.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(6740.0f, 320.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(6960.0f, 150.0f, 140.0f, 20.0f));
    Plataforma.append(new plataformas(7200.0f, 370.0f, 150.0f, 20.0f));
    Plataforma.append(new plataformas(7420.0f, 180.0f, 160.0f, 20.0f));
    Plataforma.append(new plataformas(7640.0f, 400.0f, 130.0f, 20.0f));
    Plataforma.append(new plataformas(5470.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(5930.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(6390.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(6850.0f, 480.0f, 90.0f, 20.0f));
    Plataforma.append(new plataformas(7310.0f, 480.0f, 90.0f, 20.0f));

    balasEnMapa.clear();
    if (dificultadJuego==0){
        // Parte 1, en modo facil (las 3 partes)
        balasEnMapa.append({320.0f, 630.0f, false});
        balasEnMapa.append({1050.0f, 630.0f, false});
        balasEnMapa.append({2250.0f, 630.0f, false});
        balasEnMapa.append({3450.0f, 630.0f, false});
        balasEnMapa.append({4600.0f, 630.0f, false});
        balasEnMapa.append({6050.0f, 630.0f, false});
        // Parte 2
        balasEnMapa.append({500.0f, 135.0f, false});
        balasEnMapa.append({960.0f, 105.0f, false});
        balasEnMapa.append({1440.0f, 125.0f, false});
        balasEnMapa.append({1900.0f, 95.0f, false});
        balasEnMapa.append({2820.0f, 115.0f, false});
        balasEnMapa.append({3280.0f, 95.0f, false});
        balasEnMapa.append({3740.0f, 135.0f, false});
        balasEnMapa.append({4680.0f, 125.0f, false});
        balasEnMapa.append({5140.0f, 85.0f, false});
        balasEnMapa.append({5580.0f, 115.0f, false});
        balasEnMapa.append({6040.0f, 95.0f, false});
        balasEnMapa.append({6960.0f, 85.0f, false});
        // Parte 3, antes de la bossfight
        balasEnMapa.append({5140.0f, 100.0f, false});
        balasEnMapa.append({5580.0f, 130.0f, false});
        balasEnMapa.append({6040.0f, 110.0f, false});
        balasEnMapa.append({6960.0f, 100.0f, false});
    } else {
        //balas del modo dif
        balasEnMapa.append({320.0f, 630.0f, false});
        balasEnMapa.append({2250.0f, 630.0f, false});
        balasEnMapa.append({4600.0f, 630.0f, false});
        balasEnMapa.append({6050.0f, 630.0f, false});
        balasEnMapa.append({740.0f, 295.0f, false});
        balasEnMapa.append({2360.0f, 115.0f, false});
        balasEnMapa.append({5140.0f, 85.0f, false});
        balasEnMapa.append({6960.0f, 85.0f, false});
    }
    curasEnMapa.clear();
    if (dificultadJuego == 0) {
        curasEnMapa.append({900.0f, 630.0f, false});
        curasEnMapa.append({2650.0f, 630.0f, false});
        curasEnMapa.append({4400.0f, 630.0f, false});
        curasEnMapa.append({6400.0f, 630.0f, false});
    } else {
        curasEnMapa.append({1100.0f, 630.0f, false});
        curasEnMapa.append({3500.0f, 630.0f, false});
        curasEnMapa.append({5950.0f, 630.0f, false});
    }

    enemigos.clear();
    if(dificultadJuego==0){
        short vidaFacil=50;//para el modo fácil dejemos solo los de melee y los del esmad
        enemigos.append(new enemigo(600.0f, 660.0f, true, TipoEnemigoArma::conservador, vidaFacil));
        enemigos.append(new enemigo(1400.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(1900.0f, 160.0f, true, TipoEnemigoArma::conservador, vidaFacil));
        enemigos.append(new enemigo(3900.0f, 660.0f, true, TipoEnemigoArma::liberal, vidaFacil));
        enemigos.append(new enemigo(5500.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(6600.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(960.0f, 170.0f, true, TipoEnemigoArma::conservador, vidaFacil));
        enemigos.append(new enemigo(2140.0f, 320.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(3740.0f, 200.0f, true, TipoEnemigoArma::liberal, vidaFacil));
        enemigos.append(new enemigo(6280.0f, 390.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
    } else {
        short vidaDificilLvl1crook=140;
        short vidaDificilMafiaBoss=250;
        //Todos los enemigos del piso
        enemigos.append(new enemigo(600.0f, 660.0f, true, TipoEnemigoArma::liberal, vidaDificilLvl1crook));
        enemigos.append(new enemigo(1400.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        enemigos.append(new enemigo(3900.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        enemigos.append(new enemigo(5500.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        enemigos.append(new enemigo(6600.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        //Primeros enemigos del lvl en plataformitas
        enemigos.append(new enemigo(500.0f, 200.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
        enemigos.append(new enemigo(960.0f, 170.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
        enemigos.append(new enemigo(1440.0f, 190.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
        enemigos.append(new enemigo(1900.0f, 160.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
        //Segundos enemigos del lvl
        enemigos.append(new enemigo(2820.0f, 180.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
        enemigos.append(new enemigo(3280.0f, 160.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
        enemigos.append(new enemigo(3980.0f, 350.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        enemigos.append(new enemigo(4220.0f, 170.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
        enemigos.append(new enemigo(4920.0f, 310.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
        //Ultimos enemigos antes de la boss fight :p
        enemigos.append(new enemigo(5360.0f, 360.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
        enemigos.append(new enemigo(5820.0f, 340.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        enemigos.append(new enemigo(6280.0f, 390.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        enemigos.append(new enemigo(6740.0f, 320.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
        enemigos.append(new enemigo(7200.0f, 370.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
    }
}
// Cambio de escena: borra lo del nivel 1 y arma el nivel 2 en el mismo MainWindow.
// Punisher conserva su vida y su munición; solo se ubica al inicio.
void MainWindow::cargarNivel2() {
    nivelActual = 2;

    qDeleteAll(proyectiles);
    proyectiles.clear();
    qDeleteAll(Plataforma);
    Plataforma.clear();
    qDeleteAll(enemigos);
    enemigos.clear();
    balasEnMapa.clear();
    curasEnMapa.clear();
    explosionesActivas.clear();

    camaraX = 0.0f;
    camaraY = 0.0f;
    sectorLimpio = false;
    juegoTerminado = false;

    jugador->setX(100.0f);
    jugador->setY(660.0f);
    jugador->setVy(0.0f);
    jugador->setVelocidadX(0.0f);
    jugador->setSueloY(660.0f);   // por ahora hay piso;

    // Nubes de prueba
    //                                   x       y     ancho  alto  amplitud omega  fase
    Plataforma.append(new plataformas(300.0f, 540.0f, 160.0f, 24.0f, 50.0f, 1.2f, 0.0f));
    Plataforma.append(new plataformas(600.0f, 430.0f, 160.0f, 24.0f, 70.0f, 0.9f, 1.5f));
    Plataforma.append(new plataformas(900.0f, 320.0f, 160.0f, 24.0f, 40.0f, 1.6f, 3.0f));
    Plataforma.append(new plataformas(600.0f, 200.0f, 160.0f, 24.0f, 60.0f, 1.1f, 0.7f));
    Plataforma.append(new plataformas(300.0f, 80.0f, 160.0f, 24.0f, 50.0f, 1.4f, 2.2f));
    Plataforma.append(new plataformas(650.0f, -40.0f, 160.0f, 24.0f, 80.0f, 0.8f, 4.0f));
    Plataforma.append(new plataformas(950.0f, -160.0f, 160.0f, 24.0f, 40.0f, 1.7f, 1.0f));
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
    if (enSeleccionDificultad) {
        update();
        return;
    }
    if (juegoTerminado) {
        update();
        return;
    }

    if (jugador == nullptr || jugador->GETVida() <= 0) {
        juegoTerminado = true;
        update();
        return;
    }
    for (plataformas* plat : Plataforma) {
        plat->mover(0.016f);
    }

    jugador->mover(0.016f);

    bool enAlgunaPlataforma = false;

    for (plataformas* plat : Plataforma) {
        QRectF hitboxplataforma = plat->GETHitbox();
        QRectF hitboxjugador = jugador->GETHitbox();
        float yPlataforma = plat->GETY();

        bool cayendo = (jugador->GETVy() >= 0.0f);

        float pieAnterior = hitboxjugador.bottom() - jugador->GETVy() * 0.016f;
        float pieActual = hitboxjugador.bottom();

        bool cruzoBordeArriba = (pieAnterior <= yPlataforma + 5.0f && pieActual >= yPlataforma - 8.0f);
        float margenBorde = 20.0f;
        bool colisionX = (hitboxjugador.right() - margenBorde > hitboxplataforma.left() && hitboxjugador.left() + margenBorde < hitboxplataforma.right());

        if (colisionX && cayendo && cruzoBordeArriba) {
            jugador->setY(yPlataforma - 22.0f);
            jugador->setX(jugador->GETX() + plat->GETDeltaX());
            jugador->setVy(0.0f);
            jugador->setEnAire(false);
            enAlgunaPlataforma = true;
            break;
        }
    }
    if (!enAlgunaPlataforma && jugador->GETY() < 660.0f) {
        jugador->setEnAire(true);
    }

    if (jugador->GETAnimacionGolpes()) {
        int frameActualGolpe = jugador->GETFrameGolpes();
        static bool yaPegoEstePunio = false;

        if (frameActualGolpe == 0) {
            yaPegoEstePunio = false;
        }

        if (frameActualGolpe >= 1 && frameActualGolpe <= 3 && !yaPegoEstePunio) {
            yaPegoEstePunio = true;

            float anchoGolpe = 50.0f;
            float altoGolpe = 30.0f;
            float posXGolpe = 0.0f;
            float posYTorso = jugador->GETY() - 70.0f;

            if (jugador->GETmirandoDerecha()) {
                posXGolpe = jugador->GETX() + 85.0f;
            } else {
                posXGolpe = jugador->GETX() - 20.0f;
            }

            QRectF hitboxGolpe(posXGolpe, posYTorso, anchoGolpe, altoGolpe);

            for (enemigo* en : enemigos) {
                if (en->GETEstado() != EstadoEnemigo::destruido) {
                    if (hitboxGolpe.intersects(en->GETHitbox())) {
                        en->recibirdano(25.0f);
                        break;
                    }
                }
            }
        }
    }

    static float tiempoFlote = 0.0f;
    tiempoFlote += 0.1f;

    for (int i = 0; i < balasEnMapa.size(); ++i) {
        if (!balasEnMapa[i].recolectada) {
            float offsetY = std::sin(tiempoFlote + i) * 6.0f;
            float posYActual = balasEnMapa[i].yBase + offsetY;

            QRectF hitboxBalaSuelo(balasEnMapa[i].x, posYActual, 20.0f, 25.0f);

            if (jugador->GETHitbox().intersects(hitboxBalaSuelo)) {
                int munActual = jugador->GETMunicion();
                int munMax = jugador->GETMaxMunicion();
                if (munActual < munMax) {
                    jugador->setMunicion(munActual + 3);
                    balasEnMapa[i].recolectada = true;
                }
            }
        }
    }
    for (int i = 0; i < curasEnMapa.size(); ++i) {
        if (!curasEnMapa[i].recolectada) {
            float offsetY = std::sin(tiempoFlote + i + 1.5f) * 6.0f;
            float posYActual = curasEnMapa[i].ybase + offsetY;

            QRectF hitboxCura(curasEnMapa[i].x, posYActual, 25.0f, 25.0f);

            if (jugador->GETHitbox().intersects(hitboxCura)) {
                short vidaActual = jugador->GETVida();
                if (vidaActual < 100) {
                    short nuevaVida = qMin<short>(100, vidaActual + 25);
                    jugador->setVida(nuevaVida);
                    curasEnMapa[i].recolectada = true;
                }
            }
        }
    }
if (nivelActual == 1) {
    float posEnPantallaX = jugador->GETX() - camaraX;
    if (posEnPantallaX > 640.0f) {
        camaraX = jugador->GETX() - 640.0f;
    } else if (posEnPantallaX < 400.0f && camaraX > 0.0f) {
        camaraX = jugador->GETX() - 400.0f;
    }

    float limiteDerechoSectorActual = 2500.0f;
    if (jugador->GETX() > 2500.0f && jugador->GETX() <= 5000.0f) {
        limiteDerechoSectorActual = 5000.0f;
    } else if (jugador->GETX() > 5000.0f && jugador->GETX() <= 7500.0f) {
        limiteDerechoSectorActual = 7500.0f;
    } else if (jugador->GETX() > 7500.0f) {
        limiteDerechoSectorActual = 7800.0f;
    }

    enemigosEnSectorActual = 0;
    for (enemigo* en : enemigos) {
        if (en->GETEstado() != EstadoEnemigo::destruido) {
            if (en->GETX() <= limiteDerechoSectorActual) {
                enemigosEnSectorActual++;
            }
        }
    }
    sectorLimpio = (enemigosEnSectorActual == 0);

    // Fin del nivel 1: último sector limpio -> cambio de escena al nivel 2
    // (cuando esté el camión del ESMAD, será un enemigo más de este sector)
    if (sectorLimpio && jugador->GETX() > 7500.0f) {
        cargarNivel2();
        update();
        return;
    }

    float camaraMaxPermitida = limiteDerechoSectorActual - width();
    if (camaraMaxPermitida < 0.0f) camaraMaxPermitida = 0.0f;

    if (!sectorLimpio && camaraX > camaraMaxPermitida) {
        camaraX = camaraMaxPermitida;
        if (jugador->GETX() > camaraX + width() - 80.0f) {
            jugador->setX(camaraX + width() - 80.0f);
        }
    }
} else {
    camaraX = 0.0f;           // nivel 2: la pantalla no se desplaza de lado

    float posEnPantallaY = jugador->GETY() - camaraY;
    if (posEnPantallaY < 250.0f) {
        camaraY = jugador->GETY() - 250.0f;          // sube con él
    } else if (posEnPantallaY > 500.0f) {
        camaraY = jugador->GETY() - 500.0f;          // baja si se cae
    }
    if (camaraY > 0.0f) camaraY = 0.0f;             // no mostrar debajo de la plaza
    if (camaraY < -2880.0f) camaraY = -2880.0f;      // tope: la arena de Gaitán
}

    if (camaraX < 0.0f) {
        camaraX = 0.0f;
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
        enemigo->mover(0.016f);
        bool enemigoEnPantalla = (enemigo->GETX() >= camaraX - 150 && enemigo->GETX() <= camaraX + width() + 150);

        if (enemigoEnPantalla && enemigo->puededisparar()) {
            float origenX = enemigo->GETX();
            float origenY = enemigo->GETY() - 50.0f;
            TipoEnemigoArma tipo = enemigo->GETTipoEnemigo();

            if (tipo == TipoEnemigoArma::conservador || tipo == TipoEnemigoArma::liberal) {
                float distanciaX = std::abs(jugador->GETX() - origenX);
                float distanciaY = std::abs(jugador->GETY() - enemigo->GETY());
                if (distanciaX < 90.0f && distanciaY < 60.0f) {
                    jugador->recibirDanoJugador(10);
                }
            }
                else if (tipo == TipoEnemigoArma::esmadpistolon) {
                float dirX = jugador->GETX() - origenX;
                float dirY = jugador->GETY() - origenY;
                float distancia = std::sqrt(dirX * dirX + dirY * dirY);
                float velocidadBala = 450.0f;
                float velocidadBalaX = 450.0f;
                float velocidadBalaY = 0.0f;

                if (distancia > 0.0f) {
                    velocidadBalaX = (dirX / distancia) * velocidadBala;
                    velocidadBalaY = (dirY / distancia) * velocidadBala;
                }
                proyectiles.append(new Proyectil(origenX, origenY, velocidadBalaX, velocidadBalaY, false, TipoMovimientoProyectil::Rectilineo));
            }
            else if (tipo == TipoEnemigoArma::lanzallamas) {
                float anchoLlama = 130.0f;
                float altoLlama = 70.0f;
                bool jugadorDerecha = (jugador->GETX() > origenX);
                float posXLlama = jugadorDerecha ? origenX : (origenX - anchoLlama);
                float posYLlama = enemigo->GETY() - 80.0f;
                QRectF areaLlamarada(posXLlama, posYLlama, anchoLlama, altoLlama);

                if (areaLlamarada.intersects(jugador->GETHitbox()) && std::abs(jugador->GETY() - enemigo->GETY()) < 60.0f) {
                    jugador->recibirDanoJugador(10);
                }
            }
            else if (tipo == TipoEnemigoArma::lanzagranadas) {
                float dirX = jugador->GETX() - origenX;
                float velocidadBalaX = dirX * 0.8f;
                float velocidadBalaY = -390.0f;
                if (velocidadBalaX > 450.0f) velocidadBalaX = 450.0f;
                if (velocidadBalaX < -450.0f) velocidadBalaX = -450.0f;
                if (std::abs(velocidadBalaX) < 150.0f) {
                    velocidadBalaX = (dirX > 0) ? 150.0f : -150.0f;
                }

                float manoX = enemigo->GETX() + 45.0f + ((dirX > 0) ? 34.0f : -34.0f);
                float manoY = enemigo->GETY() - 100.0f;
                proyectiles.append(new Proyectil(manoX, manoY, velocidadBalaX, velocidadBalaY, false, TipoMovimientoProyectil::Parabolico));
            }
        }
    }

    update();

    for (int i = proyectiles.size() - 1; i >= 0; --i) {
        proyectiles[i]->mover(0.016f);
        bool proyectilEliminado = false;
        QRectF hitboxBala(proyectiles[i]->GETX(), proyectiles[i]->GETY(), 8.0f, 4.0f);

        if (proyectiles[i]->GETDeJugador()) {
            for (enemigo* enemigo : enemigos) {
                if (enemigo->GETEstado() != EstadoEnemigo::destruido) {
                    if (hitboxBala.intersects(enemigo->GETHitbox())) {
                        enemigo->recibirdano(25);
                        delete proyectiles[i];
                        proyectiles.removeAt(i);
                        proyectilEliminado = true;
                        break;
                    }
                }
            }
            if (proyectilEliminado) continue;

            for (plataformas* plat : Plataforma) {
                if (hitboxBala.intersects(plat->GETHitbox())) {
                    delete proyectiles[i];
                    proyectiles.removeAt(i);
                    proyectilEliminado = true;
                    break;
                }
            }
            if (proyectilEliminado) continue;
        }
        else {
            QRectF hitboxProyectilEnemigo(proyectiles[i]->GETX(), proyectiles[i]->GETY(), 12.0f, 12.0f);
            bool colisionConJugador = hitboxProyectilEnemigo.intersects(jugador->GETHitbox());
            bool tocaSuelo = (proyectiles[i]->GETY() >= 660.0f);

            bool tocaPlataforma = false;
            for (plataformas* plat : Plataforma) {
                if (hitboxProyectilEnemigo.intersects(plat->GETHitbox())) {
                    tocaPlataforma = true;
                    break;
                }
            }

            if (proyectiles[i]->GETTipoMovimiento() == TipoMovimientoProyectil::Parabolico && (tocaSuelo || tocaPlataforma || colisionConJugador)) {
                float radioExplosion = 60.0f;
                float centroGranadaX = proyectiles[i]->GETX();
                float centroGranadaY = proyectiles[i]->GETY();


                if (centroGranadaY > 580.0f) {
                    centroGranadaY = 660.0f;
                }
                QRectF areaExplosion(centroGranadaX - radioExplosion, centroGranadaY - radioExplosion, radioExplosion * 2.0f, radioExplosion * 2.0f);

                if (areaExplosion.intersects(jugador->GETHitbox())) {
                    jugador->recibirDanoJugador(33);
                }

                explosionesActivas.append({centroGranadaX, centroGranadaY, 0, 0.0f});

                delete proyectiles[i];
                proyectiles.removeAt(i);
                proyectilEliminado = true;
                break;
            }

            if (tocaSuelo || tocaPlataforma) {
                delete proyectiles[i];
                proyectiles.removeAt(i);
                proyectilEliminado = true;
                break;
            }

            if (colisionConJugador) {
                if (proyectiles[i]->GETTipoMovimiento() == TipoMovimientoProyectil::Parabolico) {
                    float radioExplosion = 110.0f;
                    float centroGranadaX = proyectiles[i]->GETX();
                    float centroGranadaY = proyectiles[i]->GETY();
                    QRectF areaExplosion(centroGranadaX - radioExplosion, centroGranadaY - radioExplosion, radioExplosion * 2.0f, radioExplosion * 2.0f);

                    if (areaExplosion.intersects(jugador->GETHitbox())) {
                        jugador->recibirDanoJugador(40);
                    }
                } else {
                    jugador->recibirDanoJugador(20);
                }

                delete proyectiles[i];
                proyectiles.removeAt(i);
                proyectilEliminado = true;
                break;
            }
        }

        if (proyectilEliminado) continue;

        if (proyectiles[i]->GETY() >= 700.0f || proyectiles[i]->GETX() < camaraX - 300 || proyectiles[i]->GETX() > camaraX + width() + 300) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }
    for (int e = explosionesActivas.size() - 1; e >= 0; --e) {
        explosionesActivas[e].tiempoFrame += 0.016f;
        if (explosionesActivas[e].tiempoFrame >= 0.08f) {
            explosionesActivas[e].tiempoFrame = 0.0f;
            explosionesActivas[e].frameActual++;

            if (explosionesActivas[e].frameActual >= 4) {
                explosionesActivas.removeAt(e);
            }
        }
    }
}




void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->isAutoRepeat()) return;

    if (enSeleccionDificultad) {
        if (event->key() == Qt::Key_1) {
            dificultadJuego = 0; // Modo facil
            enSeleccionDificultad = false;
            reiniciarNivel();
        } else if (event->key() == Qt::Key_2) {
            dificultadJuego = 1; // Dificil
            enSeleccionDificultad = false;
            reiniciarNivel();
        }
        return;
    }
    if (juegoTerminado) {
        if (event->key() == Qt::Key_R || event->key() == Qt::Key_Return) {
            if (nivelActual == 2) {          // si murió en el nivel 2, reinicia el nivel 2
                jugador->setVida(100);
                jugador->setMunicion(jugador->GETMaxMunicion());
                cargarNivel2();
            } else {
                reiniciarNivel();
            }
        }
        return;
    }

    // ================== TEMPORAL - BORRAR ANTES DE ENTREGAR ==================
    // F2 salta directo al nivel 2 para probarlo sin jugar todo el nivel 1.
    if (event->key() == Qt::Key_F2) {
        cargarNivel2();
        return;
    }
    // ================ FIN TEMPORAL - BORRAR ANTES DE ENTREGAR ================

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
    else if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up) {
        jugador->setApuntandoArriba(true);
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
    else if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up) {
        jugador->setApuntandoArriba(false);
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (enSeleccionDificultad) {
        if (!spritePantallaInicio.isNull()) {
            painter.drawPixmap(0, 0, width(), height(), spritePantallaInicio);
        } else {
                painter.setBrush(Qt::black);
                painter.setPen(Qt::NoPen);
                painter.drawRect(0, 0, width(), height());

                QFont fuenteTitulo("Courier New", 28, QFont::Bold);
                painter.setFont(fuenteTitulo);
                painter.setPen(Qt::white);
                QString titulo = "SELECCIONA LA DIFICULTAD";
                int anchoT = QFontMetrics(fuenteTitulo).horizontalAdvance(titulo);
                painter.drawText((width() - anchoT) / 2, 220, titulo);

                QFont fuenteOpciones("Courier New", 18, QFont::Bold);
                painter.setFont(fuenteOpciones);
                painter.setPen(Qt::green);
                painter.drawText((width() - QFontMetrics(fuenteOpciones).horizontalAdvance("[1] MODO FÁCIL (Más munición, una mejor experiencia para principiantes :) )")) / 2, 340, "[1] MODO FÁCIL (Más munición, una mejor experiencia para principiantes :) )");

                painter.setPen(Qt::red);
                painter.drawText((width() - QFontMetrics(fuenteOpciones).horizontalAdvance("[2] MODO DIFÍCIL ( Experiencia clásica y extrema >:) )")) / 2, 420, "[2] MODO DIFÍCIL ( Experiencia clásica y extrema >:) )");
                return;
            }
        return;
    }


    if (juegoTerminado) {
        if (!spriteGameOver.isNull()) {
            painter.drawPixmap(0, 0, width(), height(), spriteGameOver);
        } else {
            painter.setBrush(Qt::black);
            painter.setPen(Qt::NoPen);
            painter.drawRect(0, 0, width(), height());

            QFont fuenteGameOver("Courier New", 32, QFont::Bold);
            painter.setFont(fuenteGameOver);
            painter.setPen(Qt::white);
            QString textoGO = "GAME OVER";
            int anchoTexto = QFontMetrics(fuenteGameOver).horizontalAdvance(textoGO);
            painter.drawText((width() - anchoTexto) / 2, height() / 2, textoGO);
        }
        return;
    }


    if (nivelActual == 2) {
        // cielo provisional: se aclara a medida que se sube (luego va la imagen de fondo)
        float altura = qBound(0.0f, -camaraY / 2880.0f, 1.0f);
        QColor cielo(111 + qRound(101 * altura), 164 + qRound(90 * altura), 198 + qRound(52 * altura));
        painter.fillRect(0, 0, width(), height(), cielo);
    } else if (!fondoLvl1Escalado.isNull() && fondoLvl1Escalado.width() > 0) {
        int anchoFondo = fondoLvl1Escalado.width();
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
            painter.drawPixmap(xPos, 0, fondoLvl1Escalado);
        }
    }
    painter.save();
    painter.translate(0, -camaraY);

    int indiceFrame = 0;
    int spriteY = 1669;
    int totalFramesFila = 12;
    float anchoTiraFila = 932.0f;
    int offsetXInicial = 4;
    int spriteAlto = 97;
    int spriteX = 0;
    int spriteAncho = 0;
    bool usarcoordenadasmanuales=false;

    if (jugador->GETAnimacionDano()) {
        spriteY = 3053;
        spriteX = 23;
        spriteAncho = 71;
        spriteAlto = 86;
        usarcoordenadasmanuales = true;
    }
    else if (jugador->GETAnimacionGolpes()) {
        spriteY = 251;
        spriteAlto = 88;
        totalFramesFila = 5;
        indiceFrame = jugador->GETFrameGolpes() % totalFramesFila;

        static const int coordenadasX[5] = {2, 180, 380, 587, 773};
        static const int anchos[5]  = {77, 106, 89, 81, 76};

        spriteX = coordenadasX[indiceFrame];
        spriteAncho = anchos[indiceFrame];
        usarcoordenadasmanuales=true;

    }else {
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
        }

        else if (jugador->GETApuntandoArriba()&& jugador->GETDisparandoAnim()) {
            spriteY = 2641;
            spriteAlto = 156;
            totalFramesFila = 11;

            float anchoTiraArriba = 826.0f;
            int offsetInicioArriba = 4;

            int framePoseArriba = jugador->GETFrameDisparo() % totalFramesFila;
            spriteX = offsetInicioArriba + qRound(framePoseArriba * (anchoTiraArriba / totalFramesFila));
            int siguienteXArriba = offsetInicioArriba + qRound((framePoseArriba + 1) * (anchoTiraArriba / totalFramesFila));
            spriteAncho = (siguienteXArriba - spriteX);
            usarcoordenadasmanuales = true;

        } else if (jugador->GETApuntandoArriba()) {
            if (jugador->GETEnMovimiento() && !jugador->GETEnElAire()) {
                indiceFrame = jugador->GETFrameActual() % 12;
                spriteY = 1669;
                spriteAlto = 97;
                totalFramesFila = 12;
                anchoTiraFila = 932.0f;
                offsetXInicial = 4;
            } else {
                spriteY = 2641;
                spriteAlto = 156;
                totalFramesFila = 11;

                float anchoTiraArriba = 826.0f;
                int offsetInicioArriba = 4;
                int framePoseArriba = 0;

                spriteX = offsetInicioArriba + qRound(framePoseArriba * (anchoTiraArriba / totalFramesFila));
                int siguienteXArriba = offsetInicioArriba + qRound((framePoseArriba + 1) * (anchoTiraArriba / totalFramesFila));
                spriteAncho = (siguienteXArriba - spriteX);
                usarcoordenadasmanuales = true;
            }

        } else if (jugador->GETDisparandoAnim()) {
            spriteY=1779;
            spriteAlto = 97;
            totalFramesFila = 6;
            static const int coordsXDisparo[6] = {5, 135, 265, 395, 525, 655};
            static const int anchosDisparo[6]  = {115, 115, 115, 115, 115, 115};

            int frameDispIdx = jugador->GETFrameDisparo() % 6;
            spriteX = coordsXDisparo[frameDispIdx];
            spriteAncho = anchosDisparo[frameDispIdx];
            usarcoordenadasmanuales = true;

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

    static int relojAnim = 0;
    relojAnim++;

    for (enemigo* en : enemigos) {
        if (en->GETEstado() != EstadoEnemigo::destruido) {
            int xPantalla = static_cast<int>(en->GETX() - camaraX);
            int yPantalla = static_cast<int>(en->GETY() - 100);

            TipoEnemigoArma tipo = en->GETTipoEnemigo();
            EstadoEnemigo estado = en->GETEstado();
            bool mirarIzquierda = (jugador->GETX() < en->GETX());

            if ((tipo == TipoEnemigoArma::conservador || tipo == TipoEnemigoArma::liberal) &&
                (!spriteConservador.isNull() || !spriteLiberal.isNull())) {
                QPixmap hoja = (tipo == TipoEnemigoArma::conservador) ? spriteConservador : spriteLiberal;

                if (hoja.isNull()) {
                    hoja = !spriteConservador.isNull() ? spriteConservador : spriteLiberal;
                }
                int xFrame = 4;
                int yFrame = 6;
                int wFrame = 72;
                int hFrame = 104;
                int centroFrame = 36;

                if (estado == EstadoEnemigo::idle) {
                    int xsIdle[4] = {2, 77, 132, 194};
                    int wsIdle[4] = {46, 51, 60, 60};
                    int centrosIdle[4] = {22, 27, 35, 38};
                    int idx = (relojAnim / 16) % 4;
                    xFrame = xsIdle[idx];
                    wFrame = wsIdle[idx];
                    centroFrame = centrosIdle[idx];
                    yFrame = 24;
                    hFrame = 104;

                } else if (estado == EstadoEnemigo::alerta) {
                    int xsAlerta[2] = {258, 365};
                    int wsAlerta[2] = {74, 64};
                    int centrosAlerta[2] = {40, 39};
                    int idx = (relojAnim / 10) % 2;
                    xFrame = xsAlerta[idx];
                    wFrame = wsAlerta[idx];
                    centroFrame = centrosAlerta[idx];
                    yFrame = 24;
                    hFrame = 104;

                } else if (estado == EstadoEnemigo::disparando) {
                    int xsAtaque[3] = {10, 96, 162};
                    int wsAtaque[3] = {69, 64, 94};
                    int centrosAtaque[3] = {43, 35, 61};
                    int idx = (relojAnim / 9) % 3;
                    xFrame = xsAtaque[idx];
                    wFrame = wsAtaque[idx];
                    centroFrame = centrosAtaque[idx];
                    yFrame = 152;
                    hFrame = 104;
                }

                if (xFrame + wFrame <= hoja.width() && yFrame + hFrame <= hoja.height()) {
                    QPixmap recorte = hoja.copy(xFrame, yFrame, wFrame, hFrame);
                    int altoDibujo = 104;
                    int anchoDibujo = qRound((static_cast<float>(wFrame) / hFrame) * altoDibujo);
                    QPixmap escalado = recorte.scaled(anchoDibujo, altoDibujo, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                    int centroDibujo = qRound(centroFrame * (static_cast<float>(escalado.height()) / hFrame));
                    if (!mirarIzquierda) centroDibujo = escalado.width() - centroDibujo;
                    int drawX = xPantalla - centroDibujo + 20;
                    int drawY = static_cast<int>(en->GETY()) - escalado.height() + 4;

                    painter.save();
                    if (!mirarIzquierda) {
                        painter.translate(drawX + escalado.width(), drawY);
                        painter.scale(-1, 1);
                        painter.drawPixmap(0, 0, escalado);
                    } else {
                        painter.drawPixmap(drawX, drawY, escalado);
                    }
                    painter.restore();
                } else {
                    painter.fillRect(xPantalla, yPantalla, 40, 100, (tipo == TipoEnemigoArma::conservador) ? Qt::blue : Qt::red);
                }

            } else if (tipo == TipoEnemigoArma::lanzallamas && !spriteLanzallamas.isNull()) {
                bool estaDisparando = (estado == EstadoEnemigo::disparando);
                bool mostrandoFuego = estaDisparando && en->estaLanzandoLlama();

                // medidas reales de cada cuadro (fila 0 = quieto, fila 1 = disparando)
                static const int xsFila0[4] = {26, 181, 335, 485};
                static const int wsFila0[4] = {153, 153, 149, 131};
                static const int centrosFila0[4] = {57, 57, 58, 59};
                static const int xsFila1[7] = {21, 182, 356, 535, 709, 871, 1042};
                static const int wsFila1[7] = {155, 159, 158, 156, 157, 149, 157};
                static const int centrosFila1[7] = {58, 60, 60, 60, 60, 60, 60};
                static const int piesFila1[7] = {411, 408, 407, 405, 400, 400, 399};

                int fila = estaDisparando ? 1 : 0;
                int col = 0;

                if (estaDisparando) {
                    int cuadrosDisparo = 6;
                    int velDisparo = 11;
                    col = 1 + ((relojAnim / velDisparo) % cuadrosDisparo);
                } else {
                    int cuadrosIdle = 4;
                    int velIdle = (estado == EstadoEnemigo::alerta) ? 15 : 22;
                    col = (relojAnim / velIdle) % cuadrosIdle;
                }

                int hRec = 188;
                int xRec = (fila == 1) ? xsFila1[col] : xsFila0[col];
                int wRec = (fila == 1) ? wsFila1[col] : wsFila0[col];
                int yRec = ((fila == 1) ? piesFila1[col] : 206) - hRec;
                int centroRec = (fila == 1) ? centrosFila1[col] : centrosFila0[col];

                if (xRec < 0) xRec = 0;
                if (yRec < 0) yRec = 0;
                if (xRec + wRec > spriteLanzallamas.width()) wRec = spriteLanzallamas.width() - xRec;
                if (yRec + hRec > spriteLanzallamas.height()) hRec = spriteLanzallamas.height() - yRec;

                QPixmap recorteSoldado = spriteLanzallamas.copy(xRec, yRec, wRec, hRec);

                int altoSoldado = 104;
                QPixmap pixSoldado = recorteSoldado.scaledToHeight(altoSoldado, Qt::SmoothTransformation);
                int centroSoldado = qRound(centroRec * (static_cast<float>(pixSoldado.height()) / hRec));
                if (mirarIzquierda) centroSoldado = pixSoldado.width() - centroSoldado;
                int cuerpoX = xPantalla + 22 - centroSoldado;
                int cuerpoY = static_cast<int>(en->GETY()) - pixSoldado.height() + 4;

                painter.save();
                if (mirarIzquierda) {
                    painter.translate(cuerpoX + pixSoldado.width(), cuerpoY);
                    painter.scale(-1, 1);
                    painter.drawPixmap(0, 0, pixSoldado);
                } else {
                    painter.drawPixmap(cuerpoX, cuerpoY, pixSoldado);
                }
                painter.restore();
                if (mostrandoFuego && !spriteFuego.isNull()) {
                    static const float inicioFuego[4] = {0.017f, 0.180f, 0.393f, 0.647f};
                    static const float finFuego[4]    = {0.141f, 0.353f, 0.607f, 0.950f};

                    int velFuego = 8;
                    int idxLlama = (relojAnim / velFuego) % 4;
                    int anchoTotalFuego = spriteFuego.width();
                    int altoTotalFuego = spriteFuego.height();

                    int xLlama = qRound(inicioFuego[idxLlama] * anchoTotalFuego);
                    int xFinLlama = qRound(finFuego[idxLlama] * anchoTotalFuego);
                    int wLlama = qMax(1, xFinLlama - xLlama);

                    QPixmap recorteLlama = spriteFuego.copy(xLlama, 0, wLlama, altoTotalFuego);
                    int altoLlama = 68;
                    QPixmap llamaEscalada = recorteLlama.scaledToHeight(altoLlama, Qt::SmoothTransformation);

                    int bocaY = cuerpoY + qRound(pixSoldado.height() * 0.56f) - (llamaEscalada.height() / 2);

                    painter.save();
                    if (mirarIzquierda) {
                        int bocaX = cuerpoX + 6;
                        painter.translate(bocaX, bocaY);
                        painter.scale(-1, 1);
                        painter.drawPixmap(0, 0, llamaEscalada);
                    } else {
                        int bocaX = cuerpoX + pixSoldado.width() - 6;
                        painter.drawPixmap(bocaX, bocaY, llamaEscalada);
                    }
                    painter.restore();
                }

            } else if (tipo == TipoEnemigoArma::conservador) {
                painter.fillRect(xPantalla, yPantalla, 40, 100, Qt::blue);
            } else if (tipo == TipoEnemigoArma::liberal) {
                painter.fillRect(xPantalla, yPantalla, 40, 100, Qt::red);
            } else if (tipo == TipoEnemigoArma::esmadpistolon && !spritePistolero.isNull()) {
                int fila = 0;
                int col = (relojAnim / 8) % 6;
                if (estado == EstadoEnemigo::disparando) {
                    fila = 1;
                    long long t = en->tiempoDesdeDisparo();
                    if (t < 90) col = 1;
                    else if (t < 180) col = 2;
                    else col = 0;
                }
                QPixmap cuadroPistolero = spritePistolero.copy(col * 144, fila * 128, 144, 128);
                int piesX = xPantalla + 20;
                int dibX = mirarIzquierda ? piesX - (144 - 61) : piesX - 61;
                int dibY = static_cast<int>(en->GETY()) + 4 - 124;

                painter.save();
                if (mirarIzquierda) {
                    painter.translate(dibX + 144, dibY);
                    painter.scale(-1, 1);
                    painter.drawPixmap(0, 0, cuadroPistolero);
                } else {
                    painter.drawPixmap(dibX, dibY, cuadroPistolero);
                }
                painter.restore();

            } else if (tipo == TipoEnemigoArma::lanzagranadas && !spriteGranadero.isNull()) {
                int fila = 0;
                int col = (relojAnim / 14) % 4;
                if (estado == EstadoEnemigo::disparando) {
                    long long t = en->tiempoDesdeDisparo();
                    if (t >= 2100) { fila = 1; col = static_cast<int>((t - 2100) / 100); }
                    else if (t < 400) { fila = 1; col = 4 + static_cast<int>(t / 100); }
                    if (col > 7) col = 7;
                }
                QPixmap cuadroGranadero = spriteGranadero.copy(col * 144, fila * 128, 144, 128);
                int piesX = xPantalla + 45;
                int dibX = mirarIzquierda ? piesX - (144 - 58) : piesX - 58;
                int dibY = static_cast<int>(en->GETY()) + 4 - 124;

                painter.save();
                if (mirarIzquierda) {
                    painter.translate(dibX + 144, dibY);
                    painter.scale(-1, 1);
                    painter.drawPixmap(0, 0, cuadroGranadero);
                } else {
                    painter.drawPixmap(dibX, dibY, cuadroGranadero);
                }
                painter.restore();
            } else if (tipo == TipoEnemigoArma::esmadpistolon) {
                painter.fillRect(xPantalla, yPantalla, 40, 100, QColor(70, 130, 180));
            } else if (tipo == TipoEnemigoArma::lanzallamas) {
                painter.fillRect(xPantalla, yPantalla, 45, 100, QColor(210, 105, 30));
            } else if (tipo == TipoEnemigoArma::lanzagranadas) {
                painter.fillRect(xPantalla, yPantalla, 45, 100, QColor(85, 107, 47));
            }


            if (en->MostrarLaAlerta()) {
                QFont fuenteAlerta("Arial", 50, QFont::Bold);
                painter.setFont(fuenteAlerta);
                painter.setPen(Qt::red);
                int anchoRectangulo = 40;
                int centroEnemigoX = xPantalla + (anchoRectangulo / 2);
                int alturaAlertaY = yPantalla - 30;
                painter.drawText(centroEnemigoX - 10, alturaAlertaY, "!");
            }
        }

        if (en->GETTipoEnemigo() == TipoEnemigoArma::lanzagranadas && !spriteCarro.isNull()) {
            int piesX = static_cast<int>(en->GETX() - camaraX) + 45;
            int colCarro = (relojAnim / 6) % 6;
            QPixmap cuadroCarro = spriteCarro.copy(colCarro * 176, 0, 176, 128);
            painter.drawPixmap(piesX - 82, static_cast<int>(en->GETY()) + 4 - 124, cuadroCarro);
        }
    }
        painter.restore();

    painter.setPen(Qt::white);
    QFont fuenteHud("Arial", 14, QFont::Bold);
    painter.setFont(fuenteHud);
    painter.drawText(30, 40, "MUNICIÓN: " + QString::number(jugador->GETMunicion()));

    painter.drawText(30, 75, "SALUD");
    painter.setBrush(Qt::darkGray);
    painter.setPen(Qt::white);
    painter.drawRect(100, 55, 200, 22);

    float porcentajeSalud = qMax(0.0f, static_cast<float>(jugador->GETVida()) / 100.0f);
    painter.setBrush(Qt::red);
    painter.drawRect(100, 55, static_cast<int>(200 * porcentajeSalud), 22);

    painter.save();
    painter.translate(0, -camaraY);

    for (Proyectil* p : proyectiles) {
        int projPantallaX = static_cast<int>(p->GETX() - camaraX);
        int projPantallaY = static_cast<int>(p->GETY());

        if (p->GETDeJugador()) {
            painter.setBrush(Qt::yellow);
            painter.setPen(QPen(QColor(255, 140, 0), 2));
            painter.drawEllipse(projPantallaX, projPantallaY, 10, 5);
        } else if (p->GETTipoMovimiento() == TipoMovimientoProyectil::Parabolico && !spriteGranada.isNull()) {
            int giro = (relojAnim / 4) % 4;                     // la granada da vueltas en el aire
            painter.drawPixmap(projPantallaX - 8, projPantallaY - 8, spriteGranada.copy(giro * 16, 0, 16, 16));
        } else {
            painter.setBrush(Qt::yellow);
            painter.setPen(QPen(QColor(255, 140, 0), 2));
            painter.drawEllipse(projPantallaX - 6, projPantallaY - 6, 14, 14);
        }
    }

    if (spriteToldo.isNull()) {
        painter.setPen(Qt::red);
        painter.drawText(width()/2, height()/2, "ERROR: No se encuentra toldo_rojo.png");
    }

    for (plataformas* plat : Plataforma) {
        QRectF hbplat = plat->GETHitbox();

        int platPantallaX = static_cast<int>(hbplat.x() - camaraX);
        int platPantallaY = static_cast<int>(hbplat.y());

        if (platPantallaY < camaraY - 100 || platPantallaY > camaraY + height() + 100) {
            continue;             // fuera de la pantalla: no se dibuja
        }

        int offsetVisualY = -5;
        int alturaVisualSprite = 60;

        if (nivelActual == 2) {          // nube provisional
            int w = static_cast<int>(hbplat.width());
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(245, 247, 252));
            painter.drawEllipse(platPantallaX, platPantallaY - 6, w, 30);
            painter.drawEllipse(platPantallaX + w / 5, platPantallaY - 22, w / 2, 34);
            painter.drawEllipse(platPantallaX + w / 2, platPantallaY - 16, w / 3, 28);
        } else {
            painter.drawPixmap(platPantallaX, platPantallaY + offsetVisualY,static_cast<int>(hbplat.width()), alturaVisualSprite,spriteToldo);
        }
    }
    if (!spriteExplosion.isNull()) {
        int totalFramesExplosion = 4;
        int anchoTotal = spriteExplosion.width();
        int anchoFrame = anchoTotal / totalFramesExplosion;
        int altoFrame = spriteExplosion.height();

        for (const auto& exp : explosionesActivas) {
            int expPantallaX = static_cast<int>(exp.x - camaraX);
            float escalaExplosion = 1.0f;
            int anchoDibujo = qRound(anchoFrame * escalaExplosion);
            int altoDibujo = qRound(altoFrame * escalaExplosion);
            int expPantallaXPos = expPantallaX - (anchoDibujo / 2);
            int expPantallaYPos = static_cast<int>(exp.y) - (altoDibujo/2)-18;

            QRect rectRecorte(exp.frameActual * anchoFrame, 0, anchoFrame, altoFrame);
            QPixmap frameExplosion = spriteExplosion.copy(rectRecorte);
            painter.drawPixmap(expPantallaXPos, expPantallaYPos, anchoDibujo, altoDibujo, frameExplosion);
        }
    }

    static float tiempoFlotePintar = 0.0f;
    tiempoFlotePintar += 0.1f;

    for (int i = 0; i < balasEnMapa.size(); ++i) {
        if (!balasEnMapa[i].recolectada) {
            float offsetY = std::sin(tiempoFlotePintar + i) * 6.0f;
            int balaPantallaX = static_cast<int>(balasEnMapa[i].x - camaraX);
            int balaPantallaY = static_cast<int>(balasEnMapa[i].yBase + offsetY);

            if (balaPantallaX >= -30 && balaPantallaX <= width() + 30) {
                painter.drawPixmap(balaPantallaX, balaPantallaY, 60, 70, spriteBalaRecoger);
            }
        }
    }

    for (int i = 0; i < curasEnMapa.size(); ++i) {
        if (!curasEnMapa[i].recolectada) {
            float offsetY = std::sin(tiempoFlotePintar + i + 1.5f) * 6.0f;
            int curaPantallaX = static_cast<int>(curasEnMapa[i].x - camaraX);
            int curaPantallaY = static_cast<int>(curasEnMapa[i].ybase + offsetY);

            if (curaPantallaX >= -40 && curaPantallaX <= width() + 40) {
                if (!spriteCura.isNull()) {
                    painter.drawPixmap(curaPantallaX, curaPantallaY, 45, 45, spriteCura);
                } else {
                    painter.setBrush(Qt::green);
                    painter.drawRect(curaPantallaX, curaPantallaY, 30, 30);
                }
            }
        }
    }
    painter.restore();            // fin del mundo

    if (sectorLimpio) {
        contadorFramesGO++;
        if (contadorFramesGO < 180) {
            if ((contadorFramesGO / 20) % 2 == 0) {
                int anchoGo = 120;
                int altoGo = 80;
                int posX = width() - anchoGo - 20;
                int posY = (height() - altoGo) / 2;

                painter.drawPixmap(posX, posY, anchoGo, altoGo, pixmapGo);
            }
        }
    } else {
        contadorFramesGO = 0;
    }



    if (juegoTerminado) {
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width(), height());

        QFont fuenteGameOver("Courier New", 40, QFont::Bold);
        painter.setFont(fuenteGameOver);
        painter.setPen(Qt::white);

        QString textoGO = "GAME OVER";
        QFontMetrics fm(fuenteGameOver);
        int anchoTextoGO = fm.horizontalAdvance(textoGO);
        painter.drawText((width() - anchoTextoGO) / 2, 180, textoGO);

        int calaveraAncho = 220;
        int calaveraAlto = 240;
        int calaveraX = (width() - calaveraAncho) / 2;
        int calaveraY = 230;

        if (!spriteGameOver.isNull()) {
            painter.drawPixmap(calaveraX, calaveraY, calaveraAncho, calaveraAlto, spriteGameOver);
        } else {
            painter.setBrush(Qt::white);
            painter.drawEllipse(calaveraX, calaveraY, calaveraAncho, 180);
            painter.setBrush(Qt::black);
            painter.drawRect(calaveraX + 60, calaveraY + 140, 20, 70);
            painter.drawRect(calaveraX + 100, calaveraY + 140, 20, 70);
            painter.drawRect(calaveraX + 140, calaveraY + 140, 20, 70);
        }
        QFont fuenteContinuar("Courier New", 16, QFont::Bold);
        painter.setFont(fuenteContinuar);
        painter.setPen(Qt::yellow);

        QString textoContinuar = "PRESS [R] TO RESTART";
        int anchoTextoCont = QFontMetrics(fuenteContinuar).horizontalAdvance(textoContinuar);
        painter.drawText((width() - anchoTextoCont) / 2, 520, textoContinuar);
    }
}