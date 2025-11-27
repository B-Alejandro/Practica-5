#include "juego.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "=== APLICACIÓN INICIADA ===";

    try
    {
        qDebug() << "Creando ventana de juego...";
        Juego juego;

        qDebug() << "Mostrando ventana...";
        juego.show();

        qDebug() << "Entrando al event loop...";
        return a.exec();
    }
    catch (const std::exception& e)
    {
        qDebug() << "EXCEPCIÓN CAPTURADA:" << e.what();
        QMessageBox::critical(nullptr, "Error Fatal",
                              QString("Error: %1").arg(e.what()));
        return -1;
    }
    catch (...)
    {
        qDebug() << "EXCEPCIÓN DESCONOCIDA CAPTURADA";
        QMessageBox::critical(nullptr, "Error Fatal",
                              "Error desconocido al iniciar el juego");
        return -1;
    }
}
