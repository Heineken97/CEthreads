import tkinter as tk
import tkinter.ttk as ttk
from collections import deque
import random

barcos = []
canal_size = 5  # Definir la longitud del canal

class CanalApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Simulación de Canal Marítimo")

        # Frame principal
        main_frame = tk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Puerto A Salida (Cola)
        self.puerto_A_salida = list(barcos)
        puerto_A_salida_frame = tk.Frame(main_frame)
        puerto_A_salida_frame.grid(row=0, column=0, padx=10)  # Mantener grid para alinear bien
        tk.Label(puerto_A_salida_frame, text="Puerto A Salida").pack()
        self.puerto_A_frame = tk.Frame(puerto_A_salida_frame)
        self.puerto_A_frame.pack()
        self.actualizar_puerto_A()

        # Puerto A Entrada (Cola)
        self.puerto_A_entrada = deque()
        puerto_A_entrada_frame = tk.Frame(main_frame)
        puerto_A_entrada_frame.grid(row=1, column=0, padx=10)  # Mantener en la misma columna
        tk.Label(puerto_A_entrada_frame, text="Puerto A Entrada").pack()
        self.puerto_A_entrada_frame = tk.Frame(puerto_A_entrada_frame)
        self.puerto_A_entrada_frame.pack()
        self.actualizar_puerto_A_entrada()

        # Canal (Horizontal con longitud definida)
        canal_frame = tk.Frame(main_frame)
        canal_frame.grid(row=0, column=1, rowspan=2, padx=20)  # Ocupa dos filas para alinearse bien
        tk.Label(canal_frame, text="Canal").pack()
        self.canal = [None] * canal_size  # Crear una lista que representa el canal
        self.canal_labels = []
        for i in range(canal_size):
            label = tk.Label(canal_frame, text="Espacio " + str(i + 1), width=10, relief="ridge")
            label.pack(side=tk.LEFT)
            self.canal_labels.append(label)

        # Puerto B Salida (Cola)
        self.puerto_B_salida =  list(barcos)
        puerto_B_salida_frame = tk.Frame(main_frame)
        puerto_B_salida_frame.grid(row=0, column=2, padx=10)  # Alinear en la misma fila que Puerto A
        tk.Label(puerto_B_salida_frame, text="Puerto B Salida").pack()
        self.puerto_B_frame = tk.Frame(puerto_B_salida_frame)
        self.puerto_B_frame.pack()
        self.actualizar_puerto_B()

        # Puerto B Entrada (Cola)
        self.puerto_B_entrada = deque()
        puerto_B_entrada_frame = tk.Frame(main_frame)
        puerto_B_entrada_frame.grid(row=1, column=2, padx=10)  # Mantener en la misma columna
        tk.Label(puerto_B_entrada_frame, text="Puerto B Entrada").pack()
        self.puerto_B_entrada_frame = tk.Frame(puerto_B_entrada_frame)
        self.puerto_B_entrada_frame.pack()
        self.actualizar_puerto_B_entrada()

        # Botones de control
        control_frame = tk.Frame(main_frame)
        control_frame.grid(row=2, column=1, pady=20)  # Centrar en la fila inferior
        tk.Button(control_frame, text="Movimiento de Puerto A", command=self.mover_desde_A).pack(side=tk.LEFT, padx=5)
        tk.Button(control_frame, text="Movimiento de Puerto B", command=self.mover_desde_B).pack(side=tk.LEFT, padx=5)
        tk.Button(control_frame, text="Agregar barcos definidos", command=self.agregar_barcos_definidos).pack(side=tk.LEFT, padx=5)
        tk.Button(control_frame, text="Agregar barcos aleatorios", command=self.agregar_barcos_aleatorios).pack(side=tk.LEFT, padx=5)
        
        # Crear un frame para la tabla de referencias
        frame_referencias = tk.Frame(main_frame)
        frame_referencias.grid(row=3, column=1, padx=10, pady=10)
        # Agregar filas a la tabla
        for boat_type in ["Normal", "Patrulla", "Pesquero"]:
            row_frame = tk.Frame(frame_referencias)
            row_frame.pack(fill=tk.X, padx=10, pady=5)
            # Agregar el nombre del barco
            tk.Label(row_frame, text=boat_type).pack(side=tk.LEFT, padx=10)
            # Agregar la imagen del barco
            label = tk.Label(row_frame)
            self.draw_boat(label, boat_type)
            label.pack(side=tk.LEFT, padx=10)
        frame_algoritmos = tk.Frame(main_frame)
        frame_algoritmos.grid(row=4, column=1, padx=10, pady=10)

    def mover_desde_A(self):
        # Tomar el barco de Puerto A Salida
        if self.puerto_A_salida:
            barco = self.puerto_A_salida.pop(-1)  # Elimina el barco de la cola
            self.actualizar_puerto_A()
            # Mover el barco por cada posición del canal
            for i in range(canal_size):
                if i > 0:
                    self.canal[i - 1] = None  # Libera la posición anterior si existe un barco
                self.canal[i] = barco  # Mueve el barco a la posición actual
                self.actualizar_canal()
                self.root.update()  # Simula el movimiento sin delay
                self.root.after(500)  # Simula el movimiento con una pausa
            # Colocar el barco en Puerto B Entrada
            self.puerto_B_entrada.append(barco)
            self.actualizar_puerto_B_entrada()
            self.canal = [None] * canal_size  # Vaciar el canal
            self.actualizar_canal()
    def mover_desde_B(self):
        # Tomar el barco de Puerto B Salida
        if self.puerto_B_salida:
            barco = self.puerto_B_salida.pop(0)  # Eliminar el primer barco de la lista
            self.actualizar_puerto_B()
            # Mover el barco por cada posición del canal (en orden inverso)
            for i in range(canal_size - 1, -1, -1):
                if i < canal_size - 1:
                    self.canal[i + 1] = None  # Libera la posición anterior si existe un barco
                self.canal[i] = barco  # Mueve el barco a la posición actual
                self.actualizar_canal()
                self.root.update()  # Simula el movimiento sin delay
                self.root.after(500)  # Simula el movimiento con una pausa
            # Colocar el barco en Puerto A Entrada
            self.puerto_A_entrada.append(barco)
            self.actualizar_puerto_A_entrada()
            self.canal = [None] * canal_size  # Vaciar el canal
            self.actualizar_canal()
            
                
    def agregar_barcos_definidos(self):
        # Crear una ventana emergente para agregar barcos definidos
        ventana = tk.Toplevel(self.root)
        ventana.title("Agregar barcos definidos")
        # Crear un frame para agregar barcos
        frame = tk.Frame(ventana)
        frame.pack(padx=10, pady=10)
        # Crear botones para agregar cada tipo de barco
        def agregar_normal_A():
            self.puerto_A_salida.insert(0, "Normal")
            self.actualizar_puerto_A()
        def agregar_patrulla_A():
            self.puerto_A_salida.insert(0, "Patrulla")
            self.actualizar_puerto_A()
        def agregar_pesquero_A():
            self.puerto_A_salida.insert(0, "Pesquero")
            self.actualizar_puerto_A()
        def agregar_normal_B():
            self.puerto_B_salida.append("Normal")
            self.actualizar_puerto_B()
        def agregar_patrulla_B():
            self.puerto_B_salida.append("Patrulla")
            self.actualizar_puerto_B()
        def agregar_pesquero_B():
            self.puerto_B_salida.append("Pesquero")
            self.actualizar_puerto_B()
        # Crear botones para eliminar el último barco ingresado
        def eliminar_ultimo_A():
            if self.puerto_A_salida:
                self.puerto_A_salida.pop()
                self.actualizar_puerto_A()
        def eliminar_ultimo_B():
            if self.puerto_B_salida:
                self.puerto_B_salida.pop()
                self.actualizar_puerto_B()
        # Crear un frame para los botones de A
        frame_A = tk.Frame(frame)
        frame_A.pack(side=tk.LEFT, padx=10)
        tk.Button(frame_A, text="Normal A", command=agregar_normal_A).pack()
        tk.Button(frame_A, text="Patrulla A", command=agregar_patrulla_A).pack()
        tk.Button(frame_A, text="Pesquero A", command=agregar_pesquero_A).pack()
        tk.Button(frame_A, text="Eliminar último A", command=eliminar_ultimo_A).pack()
        # Crear un frame para los botones de B
        frame_B = tk.Frame(frame)
        frame_B.pack(side=tk.LEFT, padx=10)
        tk.Button(frame_B, text="Normal B", command=agregar_normal_B).pack()
        tk.Button(frame_B, text="Patrulla B", command=agregar_patrulla_B).pack()
        tk.Button(frame_B, text="Pesquero B", command=agregar_pesquero_B).pack()
        tk.Button(frame_B, text="Eliminar último B", command=eliminar_ultimo_B).pack()
        ## Crear un frame para el botón de salir
        frame_salir = tk.Frame(frame)
        frame_salir.pack(side=tk.LEFT, padx=10)
        def salir():
            ventana.destroy()
        tk.Button(frame_salir, text="Salir", command=salir).pack()
        
    def agregar_barcos_aleatorios(self):
        # Agregar algunos elementos a la lista barcos
        barcos = ["Normal", "Patrulla", "Pesquero"]
        # Agregar un barco aleatorio a uno de los puertos
        barco = random.choice(barcos)
        if random.choice([True, False]):
            self.puerto_A_salida.append(barco)
            self.actualizar_puerto_A()
        else:
            self.puerto_B_salida.append(barco)
            self.actualizar_puerto_B()
    def actualizar_canal(self):
        # Actualiza las etiquetas del canal para reflejar el estado actual
        for i, label in enumerate(self.canal_labels):
            if self.canal[i] is None:
                label.config(text="Espacio " + str(i + 1))
                for widget in label.winfo_children():
                    widget.destroy()
            else:
                label.config(text=self.canal[i])
                self.draw_boat(label, self.canal[i])
    def actualizar_puerto_A(self):
        self._actualizar_puerto(self.puerto_A_salida, self.puerto_A_frame)
    def actualizar_puerto_B(self):
        self._actualizar_puerto(self.puerto_B_salida, self.puerto_B_frame)
    def _actualizar_puerto(self, puerto, frame):
        # Limpiar el frame
        for widget in frame.winfo_children():
            widget.destroy()
        # Mostrar los barcos de forma horizontal
        for barco in puerto:
            barco_label = tk.Label(frame, text=barco, borderwidth=1, relief="solid", padx=5, pady=5)
            barco_label.pack(side=tk.LEFT, padx=2)
            self.draw_boat(barco_label, barco)
        # Línea divisoria
        line = tk.Label(frame, text="|", padx=2)
        line.pack(side=tk.LEFT)
    def actualizar_puerto_A_entrada(self):
        self._actualizar_puerto(self.puerto_A_entrada, self.puerto_A_entrada_frame)
    def actualizar_puerto_B_entrada(self):
        self._actualizar_puerto(self.puerto_B_entrada, self.puerto_B_entrada_frame)
        
    def draw_boat(self, label, boat_type):
        canvas = tk.Canvas(label, width=50, height=30)
        canvas.pack()
        # Trapezium for the boat body
        if boat_type == 'Normal':
            canvas.create_polygon(10, 20, 20, 30, 40, 30, 50, 20, fill='blue')
        elif boat_type == 'Patrulla':
            canvas.create_polygon(10, 20, 20, 30, 40, 30, 50, 20, fill='red')
        elif boat_type == 'Pesquero':
            canvas.create_polygon(10, 20, 20, 30, 40, 30, 50, 20, fill='green')
        # Stick for the mast
        canvas.create_line(25, 20, 25, 10, fill='black')
        # Triangle for the flag
        canvas.create_polygon(30, 10, 25, 15, 25, 5, fill='white')

if __name__ == "__main__":
    root = tk.Tk()
    app = CanalApp(root)
    root.mainloop()
