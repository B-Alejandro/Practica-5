#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Crear el juego
    juego = new Juego(this);

    // Establecer el juego como widget central
    setCentralWidget(juego);

    // Configurar ventana
    setWindowTitle("Juego de Artillería - 2 Jugadores");
    resize(1000, 650);
}

MainWindow::~MainWindow()
{
    // El juego se eliminará automáticamente por ser hijo de MainWindow
}
