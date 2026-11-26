#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QPainter>
#include <stdexcept>
#include <random>

// Recorrido del ascenso del nivel 2 (de abajo hacia arriba). y = línea donde se para Punisher.
struct NubeNivel2 {
    float x, y, ancho;
    float amplitud, omega, fase;
};
static const NubeNivel2 RECORRIDO_NIVEL2[] = {
    {260,   520, 240,   0, 0.0f, 0.0f},     //  0
    {560,   380, 160,  60, 0.9f, 0.0f},     //  1
    {860,   240, 344,   0, 0.0f, 0.0f},     //  2
    {560,   100, 160,  80, 1.0f, 1.5f},     //  3
    {240,   -40, 240,   0, 0.0f, 0.0f},     //  4
    {520,  -180, 160, 100, 0.8f, 3.0f},     //  5
    {860,  -320, 240,  40, 0.6f, 0.5f},     //  6
    {560,  -460, 160,  90, 1.1f, 2.0f},     //  7
    {180,  -600, 344,   0, 0.0f, 0.0f},     //  8
    {560,  -740, 160, 110, 0.9f, 0.0f},     //  9
    {880,  -880, 240,   0, 0.0f, 0.0f},     // 10
    {600, -1020, 160, 120, 1.0f, 1.0f},     // 11
    {280, -1160, 240,  60, 0.7f, 2.5f},     // 12
    {560, -1300, 160, 120, 1.2f, 4.0f},     // 13
    {820, -1440, 344,   0, 0.0f, 0.0f},     // 14
    {540, -1580, 160, 120, 1.1f, 0.5f},     // 15
    {240, -1720, 240,  80, 0.8f, 3.5f},     // 16
    {520, -1860, 160, 130, 1.3f, 1.2f},     // 17
    {840, -2000, 240,  50, 0.9f, 2.8f},     // 18
    {560, -2140, 160, 100, 1.2f, 0.0f},     // 19
    {240, -2280, 800,   0, 0.0f, 0.0f},     // 20: arena de Gaitán
};

// Revisa que cada nube se pueda alcanzar con el salto de Punisher y que no se salga de la pantalla.
//nube imposible, lanza una excepción que dice cuál es.
static void validarRecorrido(float anchoPantalla) {
    const float alturaSalto = 800.0f * 800.0f / (2.0f * 1500.0f);
    float yAnterior = 660.0f;
    int indice = 0;
    for (const NubeNivel2 &n : RECORRIDO_NIVEL2) {
        if (yAnterior - n.y > alturaSalto) {
            throw std::logic_error("la nube " + std::to_string(indice) + " queda más alta que el salto de Punisher");
        }
        if (n.x - n.amplitud < 0.0f || n.x + n.ancho + n.amplitud > anchoPantalla) {
            throw std::logic_error("la nube " + std::to_string(indice) + " se sale de la pantalla al moverse");
        }
        yAnterior = n.y;
        indice++;
    }
}
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
    jefe = nullptr;
    peleaActiva = false;
    victoria = false;
    camionEsmad = nullptr;
    peleaCamionActiva = false;
    camionDerrotadoPendiente = false;
    tiempoTransicionNivel2 = 0.0f;
    timerSiguienteExplosionCamion = 0.0f;
    explosionesCamionEmitidas = 0;
    rngDrops.seed(std::random_device{}());

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
    spritePistolero.load("esmad_pistolon.png");
    spriteGranadero.load("lanzagranada.png");
    spriteGranada.load("granada.png");
    spriteCamionEsmad.load("camionesmad.png");
    spriteSierra.load("sierras.png");
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
    enemigosEnNube.clear();
    qDeleteAll(enemigos);
    enemigos.clear();
    enemigosDropsGenerados.clear();
    borrarJefe();
    delete camionEsmad;
    camionEsmad = nullptr;
    peleaCamionActiva = false;
    camionDerrotadoPendiente = false;
    tiempoTransicionNivel2 = 0.0f;
    timerSiguienteExplosionCamion = 0.0f;
    explosionesCamionEmitidas = 0;

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
    enemigosEnNube.clear();
    qDeleteAll(enemigos);
    enemigos.clear();
    enemigosDropsGenerados.clear();
    balasEnMapa.clear();
    curasEnMapa.clear();
    explosionesActivas.clear();
    borrarJefe();
    delete camionEsmad;
    camionEsmad = nullptr;
    peleaCamionActiva = false;
    camionDerrotadoPendiente = false;
    tiempoTransicionNivel2 = 0.0f;

    camaraX = 0.0f;
    camaraY = 0.0f;
    sectorLimpio = false;
    juegoTerminado = false;

    jugador->setX(100.0f);
    jugador->setY(660.0f);
    jugador->setVy(0.0f);
    jugador->setVelocidadX(0.0f);
    jugador->setSueloY(660.0f);   // por ahora hay piso;

    // Recorrido del ascenso (primero se valida la tabla)
    try {
        validarRecorrido(static_cast<float>(width()));
    } catch (const std::logic_error &e) {
        qWarning("Recorrido del nivel 2 inválido: %s", e.what());
    }
    for (const NubeNivel2 &n : RECORRIDO_NIVEL2) {
        Plataforma.append(new plataformas(n.x, n.y, n.ancho, 24.0f, n.amplitud, n.omega, n.fase));
    }

    // Se ubican respecto a su nube
    auto ponerEnemigo = [this](int indiceNube, float desplazamientoX, TipoEnemigoArma tipo, short vida) {
        plataformas *nube = Plataforma[indiceNube];
        enemigo *nuevo = new enemigo(nube->GETX() + desplazamientoX, nube->GETY(), true, tipo, vida);
        nuevo->setRangoVisionY(320.0f);
        enemigos.append(nuevo);
        enemigosEnNube.append(qMakePair(nuevo, nube));
    };
    if (dificultadJuego == 0) {
        short vidaFacil = 50;
        ponerEnemigo(2, 152.0f, TipoEnemigoArma::conservador, vidaFacil);
        ponerEnemigo(4, 100.0f, TipoEnemigoArma::liberal, vidaFacil);
        ponerEnemigo(8, 152.0f, TipoEnemigoArma::esmadpistolon, vidaFacil);
        ponerEnemigo(12, 100.0f, TipoEnemigoArma::liberal, vidaFacil);
        ponerEnemigo(14, 152.0f, TipoEnemigoArma::esmadpistolon, vidaFacil);
    } else {
        short vidaCrook = 140;
        short vidaPesado = 250;
        ponerEnemigo(2, 152.0f, TipoEnemigoArma::esmadpistolon, vidaCrook);
        ponerEnemigo(4, 100.0f, TipoEnemigoArma::liberal, vidaCrook);
        ponerEnemigo(6, 100.0f, TipoEnemigoArma::conservador, vidaCrook);
        ponerEnemigo(8, 60.0f, TipoEnemigoArma::esmadpistolon, vidaCrook);
        ponerEnemigo(8, 210.0f, TipoEnemigoArma::lanzallamas, vidaPesado);
        ponerEnemigo(10, 100.0f, TipoEnemigoArma::conservador, vidaCrook);
        ponerEnemigo(12, 100.0f, TipoEnemigoArma::esmadpistolon, vidaCrook);
        ponerEnemigo(14, 60.0f, TipoEnemigoArma::esmadpistolon, vidaCrook);
        ponerEnemigo(14, 210.0f, TipoEnemigoArma::lanzallamas, vidaPesado);
        ponerEnemigo(16, 100.0f, TipoEnemigoArma::liberal, vidaCrook);
        ponerEnemigo(18, 100.0f, TipoEnemigoArma::esmadpistolon, vidaCrook);
    }
}

void MainWindow::borrarJefe() {
    delete jefe;
    jefe = nullptr;
    peleaActiva = false;
    victoria = false;
}

MainWindow::~MainWindow() {
    delete camionEsmad;
    camionEsmad = nullptr;
    delete jefe;
    qDeleteAll(enemigos);
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
    if (juegoTerminado || victoria) {
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

    for (const auto &par : enemigosEnNube) {
        float dx = par.second->GETDeltaX();
        if (dx != 0.0f) {
            par.first->setX(par.first->GETX() + dx);
        }
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
                        if (en->GETEstado() == EstadoEnemigo::destruido && !enemigosDropsGenerados.contains(en)) {
                            enemigosDropsGenerados.insert(en);
                            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                            if (dist(rngDrops) < 0.50f) {
                                float dropX = en->GETX();
                                float dropY = (en->GETY() < 650.0f) ? (en->GETY() - 25.0f) : 630.0f;
                                if (dist(rngDrops) < 0.50f) {
                                    balasEnMapa.append({dropX, dropY, false});
                                } else {
                                    curasEnMapa.append({dropX, dropY, false});
                                }
                            }
                        }
                        break;
                    }
                }
            }
            if (camionEsmad != nullptr && camionEsmad->GETEstado() != EstadoEnemigo::destruido) {
                if (hitboxGolpe.intersects(camionEsmad->GETHitbox())) {
                    std::uniform_int_distribution<int> distCamionGolpe(0, 6);
                    if (distCamionGolpe(rngDrops) == 0) {
                        float spawnX = qBound(8000.0f, jugador->GETX() + 100.0f, 9050.0f);
                        balasEnMapa.append({spawnX, 630.0f, false});
                    }
                    if (camionEsmad->recibirDisparo(25)) {
                        camionDerrotadoPendiente = true;
                        tiempoTransicionNivel2 = 0.0f;
                        explosionesActivas.append({camionEsmad->GETX(), camionEsmad->GETY() - 65.0f, 0, 0.0f});
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
        if (dificultadJuego == 0) {
            if (camionEsmad != nullptr) {
                delete camionEsmad;
                camionEsmad = nullptr;
            }
            peleaCamionActiva = false;
            camionDerrotadoPendiente = false;
        }
        if (camionDerrotadoPendiente) {
            tiempoTransicionNivel2 += 0.016f;
            timerSiguienteExplosionCamion += 0.016f;
            if (camionEsmad != nullptr && explosionesCamionEmitidas < 8 && timerSiguienteExplosionCamion >= 0.085f) {
                timerSiguienteExplosionCamion = 0.0f;
                QRectF hbC = camionEsmad->GETHitbox();
                static const float offsetsX[7] = {-75.0f, 65.0f, -25.0f, -90.0f, 45.0f, -45.0f, 0.0f};
                static const float offsetsY[7] = {-95.0f, -45.0f, -115.0f, -35.0f, -90.0f, -60.0f, -70.0f};
                static const float escalas[7]  = {1.15f,  1.05f,  1.25f,   1.00f,   1.15f,  1.30f,  1.95f};

                int idx = (explosionesCamionEmitidas - 1) % 7;
                float exX = hbC.center().x() + offsetsX[idx];
                float exY = hbC.bottom() + offsetsY[idx];
                explosionesActivas.append({exX, exY, 0, 0.0f, escalas[idx]});
                explosionesCamionEmitidas++;
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
            if (tiempoTransicionNivel2 >= 1.25f) {
                cargarNivel2();
                update();
                return;
            }
            update();
            return;
        }

        const float inicioBossX = 7900.0f;
        const float finBossX = 9180.0f;

        if (dificultadJuego == 1 && !peleaCamionActiva && jugador->GETX() >= inicioBossX) {
            peleaCamionActiva = true;
            if (camionEsmad == nullptr) {
                camionEsmad = new CamionEsmad(dificultadJuego);
            }
        }

        if (dificultadJuego == 1 && peleaCamionActiva && camionEsmad != nullptr) {
            camionEsmad->actualizar(0.016f, *jugador, proyectiles);
            if (camionEsmad->consumirCambioDeFase()) {
                explosionesActivas.append({camionEsmad->GETX(), camionEsmad->GETY() - 60.0f, 0, 0.0f});
            }
        }


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
            limiteDerechoSectorActual = finBossX;
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

        if (sectorLimpio && jugador->GETX() >= inicioBossX && !peleaCamionActiva && camionEsmad == nullptr && !camionDerrotadoPendiente) {
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
        if (dificultadJuego == 1 && peleaCamionActiva) {
            if (camaraX < inicioBossX) {
                camaraX = inicioBossX;
            }
            if (jugador->GETX() < inicioBossX + 25.0f) {
                jugador->setX(inicioBossX + 25.0f);
                if (jugador->GETVx() < 0.0f) {
                    jugador->setVx(0.0f);
                }
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
    if (camaraY < -2880.0f) camaraY = -2880.0f;
    // ---------- pelea final contra Gaitán ----------
    if (!peleaActiva && jugador->GETY() < -2200.0f) {
        jefe = new JefeGaitan(dificultadJuego);
        peleaActiva = true;
        balasEnMapa.append({420.0f, -2310.0f, false});
        balasEnMapa.append({860.0f, -2310.0f, false});
        recargaMunicion = 0.0f;
    }
    if (peleaActiva) {
        camaraY = -2880.0f;
        if (jugador->GETY() > -2100.0f) {
            jugador->setX(600.0f);
            jugador->setY(-2302.0f);
            jugador->setVy(0.0f);
            jugador->recibirDanoJugador(10);
        }
        recargaMunicion += 0.016f;
        if (recargaMunicion >= 8.0f) {
            recargaMunicion = 0.0f;
            for (BalaSuelo &caja : balasEnMapa) caja.recolectada = false;
        }
        jefe->actualizar(0.016f, *jugador, proyectiles);
        int danioLaser = jefe->aplicarLaser(jugador->GETHitbox(), 0.016f);
        if (danioLaser > 0) {
            jugador->recibirDanoJugador(static_cast<short>(danioLaser));
        }
        if (jefe->consumirCambioDeFase()) {
            explosionesActivas.append({jefe->GETX(), jefe->GETY(), 0, 0.0f});
        }
    }
}


    if (camaraX < 0.0f) {
        camaraX = 0.0f;
    }

    float margenVisual = 25.0f;
    if (jugador->GETX() < camaraX + margenVisual) {
        jugador->setX(camaraX + margenVisual);
        if (jugador->GETVx() < 0.0f) {
            jugador->setVx(0.0f);
        }
    }
    float margenDerecho = 80.0f;

    if (jugador->GETX() > camaraX + width() - margenDerecho) {
        jugador->setX(camaraX + width() - margenDerecho);
        if (jugador->GETVx() > 0.0f) {
            jugador->setVx(0.0f);
        }
    }

    for (enemigo* enemigo : enemigos) {
        enemigo->actualizar(jugador->GETX(), jugador->GETY());
        enemigo->mover(0.016f);
        bool enemigoEnPantalla = (enemigo->GETX() >= camaraX - 150 && enemigo->GETX() <= camaraX + width() + 150);

        if (nivelActual == 2 && (enemigo->GETY() < camaraY - 100 || enemigo->GETY() > camaraY + height() + 100)) {
            enemigoEnPantalla = false;
        }

        if (enemigoEnPantalla && enemigo->puededisparar()) {
            float origenX = enemigo->GETX();
            float origenY = enemigo->GETY() - 50.0f;
            TipoEnemigoArma tipo = enemigo->GETTipoEnemigo();

            if (tipo == TipoEnemigoArma::conservador || tipo == TipoEnemigoArma::liberal) {
                float deltaX = jugador->GETX() - origenX;
                bool jugadorAdelante = (jugador->GETX() < origenX) ? (deltaX < 0.0f) : (deltaX > 0.0f);
                float distanciaX = std::abs(deltaX);
                float distanciaY = std::abs(jugador->GETY() - enemigo->GETY());

                if (jugadorAdelante && distanciaX < 145.0f && distanciaY < 90.0f) {
                    jugador->recibirDanoJugador(10);
                }
            }


            else if (tipo == TipoEnemigoArma::esmadpistolon) {
                bool haciaIzquierda = (jugador->GETX() < enemigo->GETX());
                origenX = enemigo->GETX() + (haciaIzquierda ? -40.0f : 40.0f);   // boca de la pistola en el cuadro del fogonazo
                origenY = enemigo->GETY() - 72.0f;
                QPointF pechoJugador = jugador->GETHitbox().center();
                float dirX = pechoJugador.x() - origenX;
                float dirY = pechoJugador.y() - origenY;
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

                float manoX = enemigo->GETX() + ((dirX > 0) ? 58.0f : -18.0f);
                float manoY = enemigo->GETY() - 112.0f;
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
            if (camionEsmad != nullptr && camionEsmad->GETEstado() != EstadoEnemigo::destruido && hitboxBala.intersects(camionEsmad->GETHitbox())) {
                std::uniform_int_distribution<int> distCamion(0, 6);
                if (distCamion(rngDrops) == 0) {
                    float spawnX = qBound(8000.0f, jugador->GETX() + 100.0f, 9050.0f);
                    balasEnMapa.append({spawnX, 630.0f, false});
                }
                if (camionEsmad->recibirDisparo(25)) {
                    camionDerrotadoPendiente = true;
                    tiempoTransicionNivel2 = 0.0f;
                    timerSiguienteExplosionCamion = 0.0f;
                    explosionesCamionEmitidas = 1;
                    explosionesActivas.append({camionEsmad->GETX(), camionEsmad->GETY() - 65.0f, 0, 0.0f});
                }
                delete proyectiles[i];
                proyectiles.removeAt(i);
                continue;
            }
            if (jefe != nullptr && jefe->GETEstado() != EstadoEnemigo::destruido && hitboxBala.intersects(jefe->GETHitbox())) {
                std::uniform_int_distribution<int> distGaitan(0, 5);
                if (distGaitan(rngDrops) == 0) {
                    float spawnX = qBound(260.0f, jugador->GETX() + 80.0f, 980.0f);
                    balasEnMapa.append({spawnX, -2245.0f, false});
                }
                if (jefe->recibirDisparo(25)) {
                    victoria = true;
                }
                delete proyectiles[i];
                proyectiles.removeAt(i);
                continue;
            }
            for (enemigo* enemigo : enemigos) {
                if (enemigo->GETEstado() != EstadoEnemigo::destruido) {
                    if (hitboxBala.intersects(enemigo->GETHitbox())) {
                        enemigo->recibirdano(25);
                        if (enemigo->GETEstado() == EstadoEnemigo::destruido && !enemigosDropsGenerados.contains(enemigo)) {
                            enemigosDropsGenerados.insert(enemigo);
                            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                            if (dist(rngDrops) < 0.50f) {
                                float dropX = enemigo->GETX();
                                float dropY = (enemigo->GETY() < 650.0f) ? (enemigo->GETY() - 25.0f) : 630.0f;
                                if (dist(rngDrops) < 0.50f) {
                                    balasEnMapa.append({dropX, dropY, false});
                                } else {
                                    curasEnMapa.append({dropX, dropY, false});
                                }
                            }
                        }
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
            int origen = proyectiles[i]->GETAtaqueOrigen();
            bool esSierra = (nivelActual == 1 && origen == AtaqueSierras);

            if (proyectiles[i]->GETTipoMovimiento() == TipoMovimientoProyectil::Pendulo) {
                hitboxProyectilEnemigo = QRectF(proyectiles[i]->GETX() - 14.0f, proyectiles[i]->GETY() - 14.0f, 28.0f, 28.0f);
            } else if (esSierra) {
                hitboxProyectilEnemigo = QRectF(proyectiles[i]->GETX() - 30.0f, proyectiles[i]->GETY() - 34.0f, 68.0f, 68.0f);
            }

            bool colisionConJugador = hitboxProyectilEnemigo.intersects(jugador->GETHitbox());
            bool tocaSuelo = (!esSierra && proyectiles[i]->GETY() >= 660.0f);

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

                bool explosionToca = areaExplosion.intersects(jugador->GETHitbox());
                if (explosionToca) {
                    int danioGranada = (origen >= 0 && nivelActual == 1 && camionEsmad != nullptr) ? camionEsmad->escalarDanio(26) : (origen >= 0 && jefe != nullptr ? jefe->escalarDanio(28) : 33);
                    jugador->recibirDanoJugador(static_cast<short>(danioGranada));
                }
                if (origen >= 0 && jefe != nullptr) jefe->aprender(origen, explosionToca);

                explosionesActivas.append({centroGranadaX, centroGranadaY, 0, 0.0f});

                delete proyectiles[i];
                proyectiles.removeAt(i);
                proyectilEliminado = true;
                break;
            }

            if (tocaSuelo || tocaPlataforma) {
                if (origen >= 0 && jefe != nullptr) jefe->aprender(origen, false);
                else if (origen >= 0 && nivelActual == 1 && camionEsmad != nullptr) camionEsmad->aprender(origen, false);

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
                } else if (esSierra) {
                    jugador->recibirDanoJugador(22);
                } else {
                    jugador->recibirDanoJugador(origen >= 0 && jefe != nullptr ? jefe->escalarDanio(20) : 20);
                }
                if (origen >= 0 && jefe != nullptr) jefe->aprender(origen, true);
                else if (origen >= 0 && nivelActual == 1 && camionEsmad != nullptr) camionEsmad->aprender(origen, true);

                delete proyectiles[i];
                proyectiles.removeAt(i);
                proyectilEliminado = true;
                break;
            }
        }

        if (proyectilEliminado) continue;

        bool debajoDePantalla = (nivelActual == 2 && proyectiles[i]->GETY() > camaraY + height() + 100);
        if (debajoDePantalla || proyectiles[i]->GETY() >= 700.0f || proyectiles[i]->GETY() < camaraY - 300 || proyectiles[i]->GETX() < camaraX - 300 || proyectiles[i]->GETX() > camaraX + width() + 300) {
            if (!proyectiles[i]->GETDeJugador() && proyectiles[i]->GETAtaqueOrigen() >= 0 && jefe != nullptr) {
                jefe->aprender(proyectiles[i]->GETAtaqueOrigen(), false);   // se fue sin tocarlo: falló
            }
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
    if (victoria) {
        if (event->key() == Qt::Key_R || event->key() == Qt::Key_Return) {
            borrarJefe();
            enSeleccionDificultad = true;
        }
        return;
    }
    if (juegoTerminado) {
        if (event->key() == Qt::Key_R || event->key() == Qt::Key_Return) {
            if (nivelActual == 2) {
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
    if (event->key() == Qt::Key_F3) {
        if (nivelActual != 1) {
            reiniciarNivel();
        }
        jugador->setMaxMunicion(999);
        jugador->setMunicion(999);
        jugador->setVida(100);
        jugador->setX(7950.0f);
        jugador->setY(660.0f);
        jugador->setVy(0.0f);
        jugador->setVelocidadX(0.0f);

        peleaCamionActiva = true;
        camionDerrotadoPendiente = false;
        tiempoTransicionNivel2 = 0.0f;
        if (camionEsmad == nullptr) {
            camionEsmad = new CamionEsmad(dificultadJuego);
        }
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

static QPixmap aplicarParpadeoRojo(const QPixmap &base, int relojAnim) {
    if (base.isNull()) return base;
    if (((relojAnim / 4) % 2) != 0) {
        return base;
    }
    QPixmap resultado = base;
    QPainter p(&resultado);
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    p.fillRect(resultado.rect(), QColor(255, 45, 45, 155));
    p.end();
    return resultado;
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
                int yFrame = 24;
                int wFrame = 72;
                int hFrame = 104;
                int centroFrame = 36;

                if (en->estaEnAnimacionDano()) {
                    int xsDano[2] = {565, 635};
                    int wsDano[2] = {62, 62};
                    int centrosDano[2] = {32, 32};
                    int idx = (relojAnim / 5) % 2;
                    xFrame = xsDano[idx];
                    wFrame = wsDano[idx];
                    centroFrame = centrosDano[idx];
                    yFrame = 152;
                    hFrame = 104;
                } else if (estado == EstadoEnemigo::idle) {
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
                    long long tAtk = en->tiempoEnAtaque();
                    int idx = 0;
                    if (tAtk >= 0) {
                        if (tAtk < 120) idx = 0;
                        else if (tAtk < 240) idx = 1;          // sube la rodilla
                        else if (tAtk < 440) idx = 2;          // patada:
                        else if (tAtk < 560) idx = 1;          // recoge la pierna
                    }
                    xFrame = xsAtaque[idx];
                    wFrame = wsAtaque[idx];
                    centroFrame = centrosAtaque[idx];
                    yFrame = 152;
                    hFrame = 104;
                }
                if (xFrame >= 0 && yFrame >= 0 && xFrame + wFrame <= hoja.width() && yFrame + hFrame <= hoja.height()) {
                    QPixmap recorte = hoja.copy(xFrame, yFrame, wFrame, hFrame);
                    float escalaEnemigo = 1.25f;
                    int altoDibujo = qRound(104 * escalaEnemigo);
                    int anchoDibujo = qRound((static_cast<float>(wFrame) / hFrame) * altoDibujo);
                    QPixmap escalado = recorte.scaled(anchoDibujo, altoDibujo, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    if (en->estaEnAnimacionDano()) {
                        escalado = aplicarParpadeoRojo(escalado, relojAnim);
                    }

                    int centroDibujo = qRound(centroFrame * (static_cast<float>(escalado.height()) / hFrame));
                    if (!mirarIzquierda) centroDibujo = escalado.width() - centroDibujo;
                    int drawX = xPantalla - centroDibujo + qRound(20 * escalaEnemigo);
                    int drawY = static_cast<int>(en->GETY()) - escalado.height() + 28;

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

                int wHoja = spriteLanzallamas.width();
                int hHoja = spriteLanzallamas.height();

                static const int xsFila0[4] = {26, 181, 335, 485};
                static const int wsFila0[4] = {153, 153, 149, 131};
                static const int centrosFila0[4] = {57, 57, 58, 59};
                static const int xsFila1[7] = {21, 182, 356, 535, 709, 871, 1042};
                static const int wsFila1[7] = {155, 159, 158, 156, 157, 149, 157};
                static const int centrosFila1[7] = {58, 60, 60, 60, 60, 60, 60};
                static const int piesFila1[7] = {411, 408, 407, 405, 400, 400, 399};
                static const int xsFila2[2] = {20, 185};
                static const int wsFila2[2] = {155, 160};
                static const int centrosFila2[2] = {60, 62};

                int hRec = 188;
                int xRec = 26;
                int wRec = 153;
                int yRec = 206 - hRec;
                int centroRec = 57;
                int extraOffsetY = 14;

                if (en->estaEnAnimacionDano()) {
                    int idxDano = (relojAnim / 5) % 2;
                    xRec = xsFila2[idxDano];
                    wRec = wsFila2[idxDano];
                    centroRec = centrosFila2[idxDano];
                    hRec = qMin(165, hHoja / 4);
                    yRec = qMax(0, hHoja - hRec);
                    extraOffsetY = 24;
                    mostrandoFuego = false;
                } else if (estaDisparando) {
                    long long ciclo = en->cicloLlamas();
                    int col = 1;
                    if (ciclo < 750) col = 1 + ((relojAnim / 5) % 5);   // ráfaga:
                    else if (ciclo < 1050) col = 6;                     // chispas al cortar la ráfaga
                    else if (ciclo >= 2000) col = 0;                    // levanta el arma:
                    xRec = xsFila1[col];
                    wRec = wsFila1[col];
                    yRec = piesFila1[col] - hRec;
                    centroRec = centrosFila1[col];
                } else {
                    int cuadrosIdle = 4;
                    int velIdle = (estado == EstadoEnemigo::alerta) ? 15 : 22;
                    int col = (relojAnim / velIdle) % cuadrosIdle;
                    xRec = xsFila0[col];
                    wRec = wsFila0[col];
                    yRec = 206 - hRec;
                    centroRec = centrosFila0[col];
                }

                if (xRec < 0) xRec = 0;
                if (yRec < 0) yRec = 0;
                if (xRec + wRec > wHoja) wRec = qMax(1, wHoja - xRec);
                if (yRec + hRec > hHoja) hRec = qMax(1, hHoja - yRec);

                QPixmap recorteSoldado = spriteLanzallamas.copy(xRec, yRec, wRec, hRec);

                float escalaLanzallamas = 1.25f;
                int altoSoldado = qRound(104 * escalaLanzallamas);
                QPixmap pixSoldado = recorteSoldado.scaledToHeight(altoSoldado, Qt::SmoothTransformation);
                if (en->estaEnAnimacionDano()) {
                    pixSoldado = aplicarParpadeoRojo(pixSoldado, relojAnim);
                }

                int centroSoldado = qRound(centroRec * (static_cast<float>(pixSoldado.height()) / qMax(1, hRec)));
                if (mirarIzquierda) centroSoldado = pixSoldado.width() - centroSoldado;
                int cuerpoX = xPantalla + qRound(22 * escalaLanzallamas) - centroSoldado;
                int cuerpoY = static_cast<int>(en->GETY()) - pixSoldado.height() + extraOffsetY;

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
                    int altoLlama = qRound(68 * escalaLanzallamas);
                    QPixmap llamaEscalada = recorteLlama.scaledToHeight(altoLlama, Qt::SmoothTransformation);

                    int bocaY = cuerpoY + qRound(pixSoldado.height() * 0.56f) - (llamaEscalada.height() / 2) + 7;

                    painter.save();
                    if (mirarIzquierda) {
                        int bocaX = cuerpoX + qRound(6 * escalaLanzallamas);
                        painter.translate(bocaX, bocaY);
                        painter.scale(-1, 1);
                        painter.drawPixmap(0, 0, llamaEscalada);
                    } else {
                        int bocaX = cuerpoX + pixSoldado.width() - qRound(6 * escalaLanzallamas);
                        painter.drawPixmap(bocaX, bocaY, llamaEscalada);
                    }
                    painter.restore();
                }

            } else if (tipo == TipoEnemigoArma::conservador) {
                painter.fillRect(xPantalla, yPantalla, 40, 100, Qt::blue);
            } else if (tipo == TipoEnemigoArma::liberal) {
                painter.fillRect(xPantalla, yPantalla, 40, 100, Qt::red);
            } else if (tipo == TipoEnemigoArma::esmadpistolon && !spritePistolero.isNull()) {
                const int colsTotal = 6;
                const int filasTotal = 3;
                int wCelda = spritePistolero.width() / colsTotal;
                int hCelda = spritePistolero.height() / filasTotal;

                int fila = 0;
                int col = 0;
                int yRec = 0;
                int hRec = hCelda;
                int extraOffsetY = 16;

                if (en->estaEnAnimacionDano()) {
                    fila = 2;
                    col = (relojAnim / 5) % 2;
                    hRec = qMax(1, hCelda - 18);
                    yRec = fila * hCelda + 12;
                    extraOffsetY = 24;
                } else {
                    yRec = fila * hCelda;
                    if (estado == EstadoEnemigo::disparando) {
                        long long tAtk = en->tiempoEnAtaque();
                        fila = 1;
                        col = 0;                                        // apuntando mientras espera
                        if (tAtk >= 250 && tAtk < 370) col = 1;         // fogonazo
                        else if (tAtk >= 370 && tAtk < 490) col = 2;    // retroceso
                        yRec = fila * hCelda;
                    } else if (estado == EstadoEnemigo::alerta) {
                        fila = 0;
                        yRec = fila * hCelda;
                        col = (relojAnim / 8) % colsTotal;
                    } else {
                        fila = 0;
                        yRec = fila * hCelda;
                        col = ((relojAnim / 45) % 2 == 0) ? 0 : 2;
                    }
                }

                static const int xsDisparo[3]    = {37, 265, 472};
                static const int wsDisparo[3]    = {165, 206, 188};
                static const int torsoDisparo[3] = {76, 79, 99};
                const float escalaPistolero = 124.0f / hCelda;

                QPixmap cuadroPistolero;
                int anclaX;
                if (fila == 1 && col <= 2 && !en->estaEnAnimacionDano()) {
                    cuadroPistolero = spritePistolero.copy(xsDisparo[col], hCelda + 12, wsDisparo[col], hCelda - 12)
                    .scaledToHeight(qRound((hCelda - 12) * escalaPistolero), Qt::SmoothTransformation);
                    anclaX = qRound(torsoDisparo[col] * escalaPistolero);
                } else {
                    int xRec = col * wCelda;
                    if (xRec + wCelda > spritePistolero.width()) xRec = qMax(0, spritePistolero.width() - wCelda);
                    if (yRec + hRec > spritePistolero.height()) hRec = qMax(1, spritePistolero.height() - yRec);
                    cuadroPistolero = spritePistolero.copy(xRec, yRec, wCelda, hRec).scaledToHeight(124, Qt::SmoothTransformation);
                    anclaX = cuadroPistolero.width() / 2;
                }
                if (en->estaEnAnimacionDano()) {
                    cuadroPistolero = aplicarParpadeoRojo(cuadroPistolero, relojAnim);
                }

                int wDib = cuadroPistolero.width();
                int hDib = cuadroPistolero.height();
                if (mirarIzquierda) anclaX = wDib - anclaX;
                int dibX = xPantalla - anclaX;
                int dibY = static_cast<int>(en->GETY()) - hDib + extraOffsetY;

                painter.save();
                if (mirarIzquierda) {
                    painter.translate(dibX + wDib, dibY);
                    painter.scale(-1, 1);
                    painter.drawPixmap(0, 0, cuadroPistolero);
                } else {
                    painter.drawPixmap(dibX, dibY, cuadroPistolero);
                }
                painter.restore();

            } else if (tipo == TipoEnemigoArma::lanzagranadas && !spriteGranadero.isNull()) {
                static const int xsReposo[2]    = {40, 209};
                static const int wsReposo[2]    = {146, 147};
                static const int torsoReposo[2] = {62, 69};
                static const int xsLanzar[8]    = {18, 185, 353, 521, 683, 857, 1042, 1212};
                static const int wsLanzar[8]    = {151, 148, 149, 152, 157, 167, 145, 140};
                static const int torsoLanzar[8] = {83, 79, 79, 67, 66, 81, 64, 63};
                static const int xsDano[2]      = {16, 245};
                static const int wsDano[2]      = {215, 273};
                static const int torsoDano[2]   = {105, 139};
                const float escalaGranadero = 0.4748f;

                int xFrame, wFrame, torso, yFrame, hFrame, pies;
                if (en->estaEnAnimacionDano()) {
                    int idxDano = (relojAnim / 6) % 2;
                    xFrame = xsDano[idxDano];
                    wFrame = wsDano[idxDano];
                    torso = torsoDano[idxDano];
                    yFrame = 520;  hFrame = 247;  pies = 760;
                } else if (estado == EstadoEnemigo::disparando || estado == EstadoEnemigo::alerta) {
                    int cuadroLanzar = 0;
                    long long tAtk = en->tiempoEnAtaque();
                    if (tAtk >= 0) {
                        if (tAtk < 150) cuadroLanzar = 2;          // lleva la mano atrás
                        else if (tAtk < 330) cuadroLanzar = 3;     // sube el brazo
                        else if (tAtk < 480) cuadroLanzar = 4;     // LANZA:
                        else cuadroLanzar = 5;                     // sigue el impulso
                    }
                    xFrame = xsLanzar[cuadroLanzar];
                    wFrame = wsLanzar[cuadroLanzar];
                    torso = torsoLanzar[cuadroLanzar];
                    yFrame = 228;  hFrame = 278;  pies = 503;
                } else {
                    int idxIdle = (relojAnim / 30) % 2;
                    xFrame = xsReposo[idxIdle];
                    wFrame = wsReposo[idxIdle];
                    torso = torsoReposo[idxIdle];
                    yFrame = 30;  hFrame = 192;  pies = 218;
                }

                QPixmap cuadroGranadero = spriteGranadero.copy(xFrame, yFrame, wFrame, hFrame)
                                              .scaledToHeight(qRound(hFrame * escalaGranadero), Qt::SmoothTransformation);
                if (en->estaEnAnimacionDano()) {
                    cuadroGranadero = aplicarParpadeoRojo(cuadroGranadero, relojAnim);
                }

                int wDib = cuadroGranadero.width();
                int hDib = cuadroGranadero.height();
                int anclaX = qRound(torso * escalaGranadero);
                if (mirarIzquierda) anclaX = wDib - anclaX;
                int dibX = xPantalla + 45 - anclaX;
                int dibY = static_cast<int>(en->GETY()) + 10 - qRound((pies - yFrame) * escalaGranadero);

                painter.save();
                if (mirarIzquierda) {
                    painter.translate(dibX + wDib, dibY);
                    painter.scale(-1, 1);
                    painter.drawPixmap(0, 0, cuadroGranadero);
                } else {
                    painter.drawPixmap(dibX, dibY, cuadroGranadero);
                }
                painter.restore();
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

    if (peleaActiva && jefe != nullptr) {
        int anchoBarra = 500;
        int barraX = (width() - anchoBarra) / 2;
        painter.setPen(Qt::white);
        painter.drawText(barraX, 30, jefe->enFase2() ? "GAITÁN - FURIA" : "GAITÁN");
        painter.setBrush(Qt::darkGray);
        painter.drawRect(barraX, 38, anchoBarra, 16);
        float porcentajeJefe = qMax(0.0f, static_cast<float>(jefe->GETVida()) / jefe->GETVidaMaxima());
        painter.setBrush(jefe->enFase2() ? QColor(200, 30, 30) : QColor(220, 180, 40));
        painter.drawRect(barraX, 38, static_cast<int>(anchoBarra * porcentajeJefe), 16);
    }

    if (nivelActual == 1 && peleaCamionActiva && camionEsmad != nullptr) {
        int anchoBarra = 500;
        int barraX = (width() - anchoBarra) / 2;
        painter.setPen(Qt::white);
        painter.drawText(barraX, 30, camionEsmad->enFase2() ? "CAMIÓN ESMAD - FURIA" : "CAMIÓN ESMAD");
        painter.setBrush(Qt::darkGray);
        painter.drawRect(barraX, 38, anchoBarra, 16);
        float porcentajeCamion = qMax(0.0f, static_cast<float>(camionEsmad->GETVida()) / camionEsmad->GETVidaMaxima());
        painter.setBrush(camionEsmad->enFase2() ? QColor(210, 45, 35) : QColor(65, 115, 185));
        painter.drawRect(barraX, 38, static_cast<int>(anchoBarra * porcentajeCamion), 16);
    }
    painter.save();
    painter.translate(0, -camaraY);

    for (Proyectil* p : proyectiles) {
        int projPantallaX = static_cast<int>(p->GETX() - camaraX);
        int projPantallaY = static_cast<int>(p->GETY());

        if (p->GETDeJugador()) {
            painter.setBrush(Qt::yellow);
            painter.setPen(QPen(QColor(255, 140, 0), 2));
            painter.drawEllipse(projPantallaX, projPantallaY, 10, 5);
        } else if (p->GETTipoMovimiento() == TipoMovimientoProyectil::Pendulo) {   // provisional: cuerda + bola
        painter.setPen(QPen(QColor(230, 210, 150), 2));
        painter.drawLine(QPointF(p->GETPivoteX() - camaraX, p->GETPivoteY()), QPointF(projPantallaX, projPantallaY));
        painter.setPen(QPen(QColor(255, 200, 60), 3));
        painter.setBrush(QColor(255, 120, 20));
        painter.drawEllipse(QPointF(projPantallaX, projPantallaY), 14, 14);
        } else if (p->GETTipoMovimiento() == TipoMovimientoProyectil::Parabolico && !spriteGranada.isNull()) {
            int giro = (relojAnim / 4) % 4;
            int wCuadro = spriteGranada.width() / 4;
            int hCuadro = spriteGranada.height();
            QPixmap cuadroGranada = spriteGranada.copy(giro * wCuadro, 0, wCuadro, hCuadro).scaled(34, 34, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(projPantallaX - cuadroGranada.width() / 2, projPantallaY - cuadroGranada.height() / 2, cuadroGranada);
            } else if (nivelActual == 1 && !p->GETDeJugador() && p->GETAtaqueOrigen() == AtaqueSierras) {
            painter.save();
            painter.translate(projPantallaX, projPantallaY);
            painter.rotate((relojAnim * 24) % 360);
            if (!spriteSierra.isNull()) {
                int ladoBase = qMin(spriteSierra.width(), spriteSierra.height());
                int xCrop = (spriteSierra.width() - ladoBase) / 2;
                int yCrop = (spriteSierra.height() - ladoBase) / 2;
                QPixmap sierraCuadrada = spriteSierra.copy(xCrop, yCrop, ladoBase, ladoBase);
                QPixmap sierraEscalada = sierraCuadrada.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(-sierraEscalada.width() / 2, -sierraEscalada.height() / 2, sierraEscalada);
            } else {
                painter.setPen(QPen(QColor(235, 242, 250), 3));
                painter.setBrush(QColor(150, 165, 180));
                painter.drawEllipse(-65, -65, 130, 130);
                painter.drawLine(-72, 0, 72, 0);
                painter.drawLine(0, -72, 0, 72);
                painter.drawLine(-50, -50, 50, 50);
                painter.drawLine(-50, 50, 50, -50);
            }
            painter.restore();

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
            float escalaExplosion = exp.escala;
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
    if (jefe != nullptr && jefe->GETEstado() != EstadoEnemigo::destruido) {
        QRectF cuerpo = jefe->GETHitbox();
        QColor colorCuerpo = jefe->enFase2() ? QColor(200, 60, 40, 230) : QColor(230, 190, 80, 230);
        if (jefe->GETDestello() > 0.0f) colorCuerpo = Qt::white;       // destello al teletransportarse
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 220, 120, 60));
        painter.drawEllipse(cuerpo.center(), 70, 90);
        painter.setBrush(colorCuerpo);
        painter.drawRect(cuerpo.adjusted(0, 30, 0, 0));
        painter.drawEllipse(QPointF(cuerpo.center().x(), cuerpo.top() + 20), 22, 22);
        QPointF ojos = jefe->GETOjos();
        painter.setBrush(QColor(255, 40, 40));
        painter.drawEllipse(QPointF(ojos.x() - 8, ojos.y()), 4, 3);
        painter.drawEllipse(QPointF(ojos.x() + 8, ojos.y()), 4, 3);

        if (jefe->GETFaseLaser() == 1) {             // aviso
            painter.setPen(QPen(QColor(255, 60, 60, 110), 2));
            painter.drawLine(ojos, jefe->GETFinLaser());
        } else if (jefe->GETFaseLaser() == 2) {      // disparando
            painter.setPen(QPen(QColor(255, 30, 30, 200), 14));
            painter.drawLine(ojos, jefe->GETFinLaser());
            painter.setPen(QPen(QColor(255, 240, 240), 4));
            painter.drawLine(ojos, jefe->GETFinLaser());
        }    // fin del mundo
    }
    if (nivelActual == 1 && peleaCamionActiva && camionEsmad != nullptr && camionEsmad->GETEstado() != EstadoEnemigo::destruido) {
        QRectF hbCamion = camionEsmad->GETHitbox();
        int xCamion = static_cast<int>(hbCamion.x() - camaraX);
        int yCamion = static_cast<int>(hbCamion.y());
        int wCamion = static_cast<int>(hbCamion.width());
        int hCamion = static_cast<int>(hbCamion.height());

        bool enFase2 = camionEsmad->enFase2();
        float vibY = std::sin(relojAnim * (enFase2 ? 0.52f : 0.20f)) * (enFase2 ? 2.8f : 0.8f);
        bool disparandoAhora = (camionEsmad->GETTiempoFogonazo() > 0.0f);
        float retrocesoCanon = disparandoAhora ? 5.0f : 0.0f;

        int altoSpriteCamion = hCamion + 24;
        int anchoSpriteCamion = wCamion + 20;
        int offsetPisoCamion = 14;
        int xDib = xCamion - 10 + qRound(retrocesoCanon);
        int yDib = yCamion - 10 + offsetPisoCamion + qRound(vibY);
        float tSeg = relojAnim * 0.016f;

        float ySueloCamion = yCamion + hCamion + offsetPisoCamion;
        painter.save();
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 125));
        painter.drawEllipse(QRectF(xCamion + 4.0f, ySueloCamion - 8.0f, wCamion - 8.0f, 16.0f));
        painter.restore();
        painter.save();
        painter.setPen(Qt::NoPen);
        static const float centrosLlantasX[2] = {0.36f, 0.78f};
        float baseLlantasY = ySueloCamion - 9.0f;

        for (int r = 0; r < 2; ++r) {
            float lxBase = xCamion + wCamion * centrosLlantasX[r];
            for (int k = 0; k < 4; ++k) {
                float fase = std::fmod(tSeg * 1.35f + r * 0.37f + k * 0.25f, 1.0f);
                float hx = lxBase - fase * (24.0f + k * 6.0f) + std::sin(fase * 6.28318f + r + k) * 4.0f;
                float hy = baseLlantasY - fase * (20.0f + k * 4.0f);
                float radio = (9.0f + k * 3.0f) + fase * 8.5f;
                int alfa = qRound((1.0f - fase) * (enFase2 ? 175.0f : 145.0f));
                if (alfa > 0) {
                    painter.setBrush(QColor(enFase2 ? 95 : 155, enFase2 ? 85 : 160, enFase2 ? 85 : 168, alfa));
                    painter.drawEllipse(QPointF(hx, hy), radio * 1.4f, radio * 0.9f);
                }
            }
        }
        painter.restore();

        if (!spriteCamionEsmad.isNull()) {
            QPixmap pixCamion = spriteCamionEsmad.scaled(anchoSpriteCamion, altoSpriteCamion, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            if (camionEsmad->GETDestello() > 0.0f) {
                QPainter pImpacto(&pixCamion);
                pImpacto.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                pImpacto.fillRect(pixCamion.rect(), QColor(255, 255, 255, 165));
                pImpacto.end();
            }

            if (enFase2 && ((relojAnim / 5) % 2 == 0)) {
                QPainter pFase2(&pixCamion);
                pFase2.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                pFase2.fillRect(pixCamion.rect(), QColor(255, 45, 45, 145));
                pFase2.end();
            }

            painter.drawPixmap(xDib, yDib, pixCamion);
        } else {
            QColor colorBlindaje = enFase2 ? QColor(135, 35, 35) : QColor(45, 65, 95);
            if (camionEsmad->GETDestello() > 0.0f) colorBlindaje = QColor(245, 245, 245);

            painter.setPen(QPen(Qt::black, 2));
            painter.setBrush(colorBlindaje);
            painter.drawRect(xCamion, yCamion + 25 + offsetPisoCamion, wCamion, hCamion - 25);

            painter.setBrush(enFase2 ? QColor(170, 50, 40) : QColor(65, 90, 125));
            painter.drawRect(xCamion + 12, yCamion + offsetPisoCamion, 75, 45);

            painter.setBrush(QColor(30, 30, 30));
            painter.drawEllipse(xCamion + 32, yCamion + hCamion - 28 + offsetPisoCamion, 34, 34);
            painter.drawEllipse(xCamion + wCamion - 64, yCamion + hCamion - 28 + offsetPisoCamion, 34, 34);
        }

        float periodoBerrinche = 3.2f;
        float duracionBerrinche = 1.25f;
        float posCiclo = std::fmod(tSeg, periodoBerrinche);
        if (enFase2 && posCiclo <= duracionBerrinche) {
            float prog = posCiclo / duracionBerrinche;
            float envolvente = std::sin(prog * 3.14159265f);

            struct ChorrosIra {
                float ox, oy;
                float dx, dy;
                float faseOff;
                float radio;
            };
            static const ChorrosIra chorros[4] = {
                {0.22f, 0.72f, -22.0f, -26.0f, 0.00f, 12.0f},
                {0.48f, 0.45f,  10.0f, -34.0f, 0.28f, 14.0f},
                {0.72f, 0.52f, -18.0f, -30.0f, 0.57f, 15.0f},
                {0.88f, 0.25f,  20.0f, -36.0f, 0.81f, 13.0f}
            };

            painter.save();
            painter.setPen(Qt::NoPen);
            for (int c = 0; c < 4; ++c) {
                const ChorrosIra &ch = chorros[c];
                float irregular = 0.6f + 0.4f * std::sin(tSeg * (6.5f + c * 2.1f) + ch.faseOff * 6.28318f);
                for (int n = 0; n < 3; ++n) {
                    float f = std::fmod(prog * 1.9f + ch.faseOff + n * 0.33f, 1.0f);
                    float px = xCamion + wCamion * ch.ox + f * ch.dx + std::sin(f * 8.0f + c + n) * 5.0f;
                    float py = yCamion + hCamion * ch.oy + offsetPisoCamion + f * ch.dy + qRound(vibY);
                    float rad = (ch.radio + n * 4.0f + f * 12.0f) * irregular;
                    int alfa = qRound((1.0f - f) * envolvente * 165.0f);
                    if (alfa > 0) {
                        painter.setBrush(QColor(55, 42, 42, alfa));
                        painter.drawEllipse(QPointF(px, py), rad * 1.35f, rad * 0.85f);
                    }
                }
            }
            painter.restore();
        }

        if (disparandoAhora) {
            QPointF canonMundo = camionEsmad->GETCanonPos();
            int bocaX = static_cast<int>(canonMundo.x() - camaraX) + qRound(retrocesoCanon);
            int bocaY = static_cast<int>(canonMundo.y()) + qRound(vibY);
            painter.save();
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 195, 50, 200));
            painter.drawEllipse(QPointF(bocaX - 8, bocaY), 12, 7);
            painter.setBrush(QColor(255, 250, 200, 235));
            painter.drawEllipse(QPointF(bocaX - 4, bocaY), 6, 4);
            painter.restore();
        }
    }

    painter.restore();
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
    if (victoria) {
        painter.setBrush(QColor(0, 0, 0, 170));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width(), height());
        QFont fuenteVictoria("Courier New", 40, QFont::Bold);
        painter.setFont(fuenteVictoria);
        painter.setPen(QColor(255, 215, 0));
        QString textoV = "¡VICTORIA!";
        painter.drawText((width() - QFontMetrics(fuenteVictoria).horizontalAdvance(textoV)) / 2, 300, textoV);
        QFont fuenteSub("Courier New", 16, QFont::Bold);
        painter.setFont(fuenteSub);
        painter.setPen(Qt::white);
        QString textoSub = "PRESS [R] TO PLAY AGAIN";
        painter.drawText((width() - QFontMetrics(fuenteSub).horizontalAdvance(textoSub)) / 2, 380, textoSub);
    }
}