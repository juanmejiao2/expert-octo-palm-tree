#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    moviendoDerecha = false;
    moviendoIzquierda = false;
    camaraX = 0.0f;
    enSeleccionDificultad = true;
    dificultadJuego = 0;

    this->setFixedSize(1280,720);

    juegoTerminado = false;
    spriteGameOver.load("game_over.jpg");
    spriteSheetPunisher.load("Punisher_sprites.png");
    spriteToldo.load("toldo_rojo.png");
    pixmapGo.load("GO.png");
    spriteBalaRecoger.load("municion.png");
    spritePantallaInicio.load("pantalla_inicio.jpg");

    jugador = nullptr;

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
    juegoTerminado = false;
    sectorLimpio = false;

    jugador = new ::jugador(100.0f, 660.0f);
    if (dificultadJuego == 0) {
        jugador->setMaxMunicion(8);
        jugador->setMunicion(8);
    } else {
        jugador->setMaxMunicion(6);
        jugador->setMunicion(6);
    }

    Plataforma.clear();
    //Primera parte del nivel
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
    //Segunda parte del nivel
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
    //Tercera parte del nivel
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

    balasEnMapa.clear();
    if (dificultadJuego==0){
    // Parte 1, en modo facil (las 3 partes)
    balasEnMapa.append({400.0f, 630.0f, false});
    balasEnMapa.append({1600.0f, 630.0f, false});
    balasEnMapa.append({2800.0f, 630.0f, false});
    balasEnMapa.append({4200.0f, 630.0f, false});
    balasEnMapa.append({5800.0f, 630.0f, false});
    balasEnMapa.append({6900.0f, 630.0f, false});
    // Parte 2
    balasEnMapa.append({520.0f, 375.0f, false});
    balasEnMapa.append({900.0f, 435.0f, false});
    balasEnMapa.append({1300.0f, 455.0f, false});
    balasEnMapa.append({1920.0f, 405.0f, false});
    balasEnMapa.append({2720.0f, 215.0f, false});
    balasEnMapa.append({3300.0f, 415.0f, false});
    balasEnMapa.append({3920.0f, 345.0f, false});
    balasEnMapa.append({4720.0f, 425.0f, false});
    balasEnMapa.append({5300.0f, 245.0f, false});
    balasEnMapa.append({5920.0f, 435.0f, false});
    balasEnMapa.append({6600.0f, 235.0f, false});
    balasEnMapa.append({7280.0f, 445.0f, false});
    // Parte 3, antes de bossfight
    balasEnMapa.append({5300.0f, 260.0f, false});
    balasEnMapa.append({5920.0f, 450.0f, false});
    balasEnMapa.append({6600.0f, 250.0f, false});
    balasEnMapa.append({7280.0f, 460.0f, false});
    } else {
        balasEnMapa.append({1600.0f, 630.0f, false});
        balasEnMapa.append({4200.0f, 630.0f, false});
        balasEnMapa.append({6900.0f, 630.0f, false});
        balasEnMapa.append({1920.0f, 405.0f, false});
        balasEnMapa.append({4720.0f, 425.0f, false});
        balasEnMapa.append({5300.0f, 245.0f, false});
        balasEnMapa.append({6600.0f, 250.0f, false});
        balasEnMapa.append({7280.0f, 445.0f, false});
    }

    enemigos.clear();
    if(dificultadJuego==0){
    short vidaFacil=50;        //para el modo fácil dejemos solo los de melee y los del esmad
        enemigos.append(new enemigo(600.0f, 660.0f, true, TipoEnemigoArma::conservador, vidaFacil));
        enemigos.append(new enemigo(1500.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(2600.0f, 660.0f, true, TipoEnemigoArma::conservador, vidaFacil));
        enemigos.append(new enemigo(4000.0f, 660.0f, true, TipoEnemigoArma::liberal, vidaFacil));
        enemigos.append(new enemigo(5600.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(6700.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(1120.0f, 360.0f, true, TipoEnemigoArma::conservador, vidaFacil));
        enemigos.append(new enemigo(2100.0f, 330.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
        enemigos.append(new enemigo(3920.0f, 390.0f, true, TipoEnemigoArma::liberal, vidaFacil));
        enemigos.append(new enemigo(6380.0f, 430.0f, true, TipoEnemigoArma::esmadpistolon, vidaFacil));
    } else {
        short vidaDificilLvl1crook=140;
        short vidaDificilMafiaBoss=250;
    //Todos los enemigos del piso
    enemigos.append(new enemigo(600.0f, 660.0f, true, TipoEnemigoArma::liberal, vidaDificilLvl1crook));
    enemigos.append(new enemigo(1500.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    enemigos.append(new enemigo(2600.0f, 660.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(4000.0f, 660.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(5600.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    enemigos.append(new enemigo(6700.0f, 660.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    //Primeros enemigos del lvl en plataformitas
    enemigos.append(new enemigo(700.0f, 340.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(1120.0f, 360.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    enemigos.append(new enemigo(1700.0f, 280.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(2100.0f, 330.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
    //Segundos enemigos del lvl
    enemigos.append(new enemigo(2720.0f, 260.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(3120.0f, 300.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(3920.0f, 390.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    enemigos.append(new enemigo(4500.0f, 320.0f, true, TipoEnemigoArma::lanzallamas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(4900.0f, 350.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
    //Ultimos enemigos antes de la boss fight :p
    enemigos.append(new enemigo(5300.0f, 290.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
    enemigos.append(new enemigo(5920.0f, 480.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    enemigos.append(new enemigo(6380.0f, 430.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    enemigos.append(new enemigo(6830.0f, 410.0f, true, TipoEnemigoArma::esmadpistolon, vidaDificilLvl1crook));
    enemigos.append(new enemigo(7050.0f, 320.0f, true, TipoEnemigoArma::lanzagranadas, vidaDificilMafiaBoss));
    }
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

    jugador->mover(0.016f);

    bool enAlgunaPlataforma = false;

    for (plataformas* plat : Plataforma) {
        QRectF hitboxplataforma = plat->GETHitbox();
        QRectF hitboxjugador = jugador->GETHitbox();
        float yPlataforma = plat->GETY();

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

    if (jugador->GETAnimacionGolpes()) {
        int frameActualGolpe = jugador->GETFrameGolpes();

        if (frameActualGolpe >= 1 && frameActualGolpe <= 3) {
            float anchoGolpe = 50.0f;
            float altoGolpe = 30.0f;
            float posXGolpe = 0.0f;
            float posYTorso = jugador->GETY() - 70.0f;
            QRectF hbJugador = jugador->GETHitbox();

            if (jugador->GETmirandoDerecha()) {
                posXGolpe = jugador->GETX() +85.0f;
            } else {
                posXGolpe = jugador->GETX() - 20.0f;
            }

            QRectF hitboxGolpe(posXGolpe, posYTorso, anchoGolpe, altoGolpe);

            for (enemigo* en : enemigos) {
                if (en->GETEstado() != EstadoEnemigo::destruido) {
                    if (hitboxGolpe.intersects(en->GETHitbox())) {
                        en->recibirdano(10.0f);
                        if (en->GETVida() <= 0) {
                        }
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
                    jugador->setMunicion(munActual + 1);
                    balasEnMapa[i].recolectada = true;
                }
            }
        }
    }

    if (jugador->GETX() - camaraX > 640.0f) {
        camaraX = jugador->GETX() - 640.0f;
    }
    else if (jugador->GETX() - camaraX < 400.0f && camaraX > 0.0f) {
        camaraX = jugador->GETX() - 400.0f;
    }

    float limiteDerechoSectorActual = 2500.0f;
    if (jugador->GETX() > 2500.0f && jugador->GETX() <= 5000.0f) {
        limiteDerechoSectorActual = 5000.0f;
    } else if (jugador->GETX() > 5000.0f && jugador->GETX() <= 7500.0f) {
        limiteDerechoSectorActual = 7500.0f;
    } else if (jugador->GETX() > 7500.0f) {
        limiteDerechoSectorActual = 10000.0f;
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

    if (jugador->GETX() - camaraX > 640.0f) {
        camaraX = jugador->GETX() - 640.0f;
    }
    else if (jugador->GETX() - camaraX < 400.0f && camaraX > 0.0f) {
        camaraX = jugador->GETX() - 400.0f;
    }
    float camaraMaxPermitida = limiteDerechoSectorActual - width();
    if (!sectorLimpio && camaraX > camaraMaxPermitida) {
        camaraX = camaraMaxPermitida;
        if (jugador->GETX() > camaraX + width() - 80.0f) {
            jugador->setX(camaraX + width() - 80.0f);
        }
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
        bool enemigoEnPantalla = (enemigo->GETX() >= camaraX - 150 && enemigo->GETX() <= camaraX + width() + 150);

        if (enemigoEnPantalla && enemigo->puededisparar()) {
            float origenX = enemigo->GETX();
            float origenY = enemigo->GETY() - 50.0f;
            TipoEnemigoArma tipo = enemigo->GETTipoEnemigo();

            if (tipo == TipoEnemigoArma::conservador || tipo == TipoEnemigoArma::liberal) {
                float distanciaX = std::abs(jugador->GETX() - origenX);
                float distanciaY = std::abs(jugador->GETY() - enemigo->GETY());
                if (distanciaX < 55.0f && distanciaY < 50.0f) {
                    jugador->recibirDanoJugador(12);
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
                    jugador->recibirDanoJugador(15);
                }
            }
            else if (tipo == TipoEnemigoArma::lanzagranadas) {
                float dirX = jugador->GETX() - origenX;
                float velocidadBalaX = (dirX > 0) ? 280.0f : -280.0f;
                float velocidadBalaY = -350.0f;
                proyectiles.append(new Proyectil(origenX, origenY, velocidadBalaX, velocidadBalaY, false, TipoMovimientoProyectil::Parabolico));
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

            if (colisionConJugador || tocaSuelo) {
                float radioExplosion = 110.0f;
                float centroGranadaX = proyectiles[i]->GETX();
                float centroGranadaY = proyectiles[i]->GETY();
                QRectF areaExplosion(centroGranadaX - radioExplosion, centroGranadaY - radioExplosion, radioExplosion * 2.0f, radioExplosion * 2.0f);

                if (areaExplosion.intersects(jugador->GETHitbox())) {
                    jugador->recibirDanoJugador(40);
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
            reiniciarNivel();
        }
        return;
    }

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
            int enemigoPantallaY = static_cast<int>(enemigo->GETY() - 100);

            if (enemigo->GETTipoEnemigo() == TipoEnemigoArma::conservador) {
                painter.fillRect(enemigoPantallaX, enemigoPantallaY, 40, 100, Qt::blue);
            } else if (enemigo->GETTipoEnemigo() == TipoEnemigoArma::liberal) {
                painter.fillRect(enemigoPantallaX, enemigoPantallaY, 40, 100, Qt::red);
            } else if (enemigo->GETTipoEnemigo() == TipoEnemigoArma::lanzallamas) {
                painter.fillRect(enemigoPantallaX, enemigoPantallaY, 45, 100, Qt::darkYellow);
            }
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

    for (Proyectil* p : proyectiles) {
        int projPantallaX = static_cast<int>(p->GETX() - camaraX);
        int projPantallaY = static_cast<int>(p->GETY());

        if (p->GETDeJugador()) {
            painter.setBrush(Qt::yellow);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(projPantallaX, projPantallaY, 8, 4);
        } else {
            painter.setBrush(QColor(255, 69, 0));
            painter.setPen(QPen(Qt::black, 1));
            painter.drawEllipse(projPantallaX - 6, projPantallaY - 6, 14, 14);
            painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
            painter.setBrush(QColor(255, 0, 0, 15));
            painter.drawEllipse(projPantallaX - 35, projPantallaY - 35, 70, 70);
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

        int offsetVisualY = -5;
        int alturaVisualSprite = 60;

        painter.drawPixmap(platPantallaX, platPantallaY + offsetVisualY,static_cast<int>(hbplat.width()), alturaVisualSprite,spriteToldo);
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

    if (sectorLimpio) {
        static int contadorFramesGO = 0;
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
        static int contadorFramesGO = 0;
        contadorFramesGO = 0;
    }


//hitboxes y ya, esto lo quitamos después (no se muestran cuando hacen el daño, se muestran siempre ;_;
    painter.save();

    painter.setPen(QPen(Qt::green, 2, Qt::DashLine));
    painter.setBrush(QColor(0, 255, 0, 30));
    QRectF hbJugador = jugador->GETHitbox();
    painter.drawRect(QRectF(hbJugador.x() - camaraX, hbJugador.y(), hbJugador.width(), hbJugador.height()));
    for (enemigo* en : enemigos) {
        if (en->GETEstado() != EstadoEnemigo::destruido) {
            QRectF hbEnemigo = en->GETHitbox();
            float enemigoPantallaX = hbEnemigo.x() - camaraX;
            TipoEnemigoArma tipoEnemigo = en->GETTipoEnemigo();
            painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
            painter.setBrush(QColor(255, 0, 0, 40));
            painter.drawRect(QRectF(enemigoPantallaX, hbEnemigo.y(), hbEnemigo.width(), hbEnemigo.height()));
            if (tipoEnemigo == TipoEnemigoArma::conservador || tipoEnemigo == TipoEnemigoArma::liberal) {
                QRectF rangoMelee(en->GETX() - 25.0f, en->GETY() - 80.0f, 55.0f, 80.0f);
                painter.setPen(QPen(Qt::cyan, 1, Qt::DotLine));
                painter.setBrush(QColor(0, 255, 255, 30));
                painter.drawRect(QRectF(rangoMelee.x() - camaraX, rangoMelee.y(), rangoMelee.width(), rangoMelee.height()));
            }
            else if (tipoEnemigo == TipoEnemigoArma::lanzallamas) {
                if (en->GETEstado() == EstadoEnemigo::disparando) {
                    float anchoLlama = 130.0f;
                    float altoLlama = 70.0f;
                    bool jugadorDerecha = (jugador->GETX() > en->GETX());
                    float posXLlama = jugadorDerecha ? en->GETX() : (en->GETX() - anchoLlama);
                    QRectF areaLlamarada(posXLlama, en->GETY() - 80.0f, anchoLlama, altoLlama);

                    painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
                    painter.setBrush(QColor(255, 140, 0, 90));
                    painter.drawRect(QRectF(areaLlamarada.x() - camaraX, areaLlamarada.y(), areaLlamarada.width(), areaLlamarada.height()));
                }
            }
        }
    }
    for (Proyectil* p : proyectiles) {
        int projPantallaX = static_cast<int>(p->GETX() - camaraX);
        int projPantallaY = static_cast<int>(p->GETY());

        if (p->GETDeJugador()) {
            painter.setBrush(Qt::yellow);
            painter.drawEllipse(projPantallaX, projPantallaY, 8, 4);
        } else {
            painter.setBrush(QColor(255, 69, 0));
            painter.setPen(QPen(Qt::black, 1));
            painter.drawEllipse(projPantallaX - 6, projPantallaY - 6, 14, 14);
            painter.setPen(QPen(Qt::magenta, 1, Qt::DashLine));
            painter.setBrush(QColor(255, 0, 255, 30));
            float radioVisualAOE = 90.0f;
            painter.drawEllipse(projPantallaX - radioVisualAOE, projPantallaY - radioVisualAOE, radioVisualAOE * 2.0f, radioVisualAOE * 2.0f);
        }
    }

    painter.restore();







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