import matplotlib.pyplot as plt
import glob

def graficar_trayectorias():
    archivos = sorted(glob.glob("trayectoria_*.txt"))

    if not archivos:
        print("No se encontraron archivos de trayectoria.")
        return

    plt.figure(figsize=(8, 6))
    for archivo in archivos:
        x, y = [], []
        with open(archivo, 'r') as f:
            for linea in f:
                partes = linea.strip().split()
                if len(partes) == 2:
                    x.append(float(partes[0]))
                    y.append(float(partes[1]))
        plt.plot(x, y, label=archivo.replace(".txt", ""))

    plt.title("Trayectorias de las partículas")
    plt.xlabel("Posición X")
    plt.ylabel("Posición Y")
    plt.legend()
    plt.grid(True)
    plt.gca().invert_yaxis()  # opcional
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    graficar_trayectorias()
