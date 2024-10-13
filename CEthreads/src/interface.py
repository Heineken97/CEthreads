import tkinter as tk
import tkinter.ttk as ttk
from collections import deque
import random
import struct
import os
import time
import threading

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

        # Variable para almacenar el descriptor de archivo del pipe
        self.pipe_fd = None

        # Iniciar la verificación del pipe en un hilo separado
        self.start_pipe_check()

    def draw_boat(self, label, boat_type):
        # Dibujar un barco en la etiqueta
        if boat_type == "Normal":
            label.config(text="⛴️")
        elif boat_type == "Patrulla":
            label.config(text="🚢")
        elif boat_type == "Pesquero":
            label.config(text="🎣")

    def actualizar_puerto_A(self):
        # Actualizar la interfaz gráfica para mostrar los barcos en el puerto A
        for widget in self.puerto_A_frame.winfo_children():
            widget.destroy()
        for boat in self.puerto_A_salida:
            label = tk.Label(self.puerto_A_frame, text="⛴️")
            label.pack(side=tk.LEFT, padx=10)
    
    def actualizar_puerto_A_entrada(self):
        # Actualizar la interfaz gráfica para mostrar los barcos en la cola de entrada del puerto A
        for widget in self.puerto_A_entrada_frame.winfo_children():
            widget.destroy()
        for boat in self.puerto_A_entrada:
            label = tk.Label(self.puerto_A_entrada_frame, text="⛴️")
            label.pack(side=tk.LEFT, padx=10)
    
    def actualizar_puerto_B(self):
        # Actualizar la interfaz gráfica para mostrar los barcos en el puerto B
        for widget in self.puerto_B_frame.winfo_children():
            widget.destroy()
        for boat in self.puerto_B_salida:
            label = tk.Label(self.puerto_B_frame, text="🚢")
            label.pack(side=tk.LEFT, padx=10)
    
    def actualizar_puerto_B_entrada(self):
        # Actualizar la interfaz gráfica para mostrar los barcos en la cola de entrada del puerto B
        for widget in self.puerto_B_entrada_frame.winfo_children():
            widget.destroy()
        for boat in self.puerto_B_entrada:
            label = tk.Label(self.puerto_B_entrada_frame, text="🚢")
            label.pack(side=tk.LEFT, padx=10)

    def start_pipe_check(self):
        # Crear el pipe si no existe
        try:
            os.mkfifo('mock_serial_port')
        except OSError as e:
            if e.errno != 17:  # Error 17 es "File exists"
                raise

        # Abrir el pipe en modo bloqueante
        self.pipe_fd = os.open('mock_serial_port', os.O_RDONLY)

        # Iniciar el chequeo del pipe en un hilo separado
        threading.Thread(target=self.check_pipe, daemon=True).start()

    def check_pipe(self):
        while True:
            self.read_from_pipe()
            time.sleep(0.1)

    def read_from_pipe(self):
        try:
            # Leer hasta 64 bytes (tamaño de la estructura FlowManager)
            data = os.read(self.pipe_fd, 64)
            if len(data) == 0:
                # No hay datos disponibles, no es un error crítico
                return
            elif len(data) != 64:
                print(f"Error: received data size {len(data)} is incorrect")
                return

            # Desempaquetar los datos recibidos
            flow_manager = self.unpack_flow_manager(data)
            # Actualizar la interfaz gráfica con los nuevos datos
            self.update_gui(flow_manager)

        except OSError as e:
            if e.errno != 11:  # Errno 11 es "Resource temporarily unavailable", lo ignoramos
                print(f"Error reading from pipe: {e}")
        except struct.error as e:
            print(f"Error unpacking data: {e}")

    def unpack_flow_manager(self, data):
        # Desempaquetar la estructura FlowManager (15 enteros y 1 flotante)
        flow_manager = struct.unpack('iiiiiiiiiiiiiiif', data)
        return {
            'method': flow_manager[0],
            'canal_length': flow_manager[1],
            'current_direction': flow_manager[2],
            'ships_passed': flow_manager[3],
            'total_ships_passed': flow_manager[4],
            'normal_ship_speed': flow_manager[5],
            'fishing_ship_speed': flow_manager[6],
            'patrol_ship_speed': flow_manager[7],
            'ships_in_queue_LR': flow_manager[8],
            'ships_in_queue_RL': flow_manager[9],
            'ships_in_midcanal_LR': flow_manager[10],
            'ships_in_midcanal_RL': flow_manager[11],
            'ships_in_done_LR': flow_manager[12],
            'ships_in_done_RL': flow_manager[13],
            'w_param': flow_manager[14],
            't_param': flow_manager[15],
        }

    def update_gui(self, flow_manager):
        # Actualizar la interfaz gráfica según los datos del FlowManager
        # Actualizar la cola de entrada del puerto A
        self.puerto_A_entrada = deque([f"Barco {i+1}" for i in range(flow_manager['ships_in_queue_LR'])])
        self.actualizar_puerto_A_entrada()

        # Actualizar la cola de entrada del puerto B
        self.puerto_B_entrada = deque([f"Barco {i+1}" for i in range(flow_manager['ships_in_queue_RL'])])
        self.actualizar_puerto_B_entrada()

        # Actualizar el canal
        for i in range(canal_size):
            if i < flow_manager['ships_in_midcanal_LR']:
                label = tk.Label(self.canal_labels[i])
                self.draw_boat(label, "Normal")
                label.pack()
            elif i < flow_manager['ships_in_midcanal_LR'] + flow_manager['ships_in_midcanal_RL']:
                label = tk.Label(self.canal_labels[i])
                self.draw_boat(label, "Patrulla")
                label.pack()
            else:
                self.canal_labels[i].config(text="Espacio " + str(i + 1))

        # Actualizar la cola de salida del puerto A
        self.puerto_A_salida = [f"Barco {i+1}" for i in range(flow_manager['ships_in_done_LR'])]
        self.actualizar_puerto_A()

        # Actualizar la cola de salida del puerto B
        self.puerto_B_salida = [f"Barco {i+1}" for i in range(flow_manager['ships_in_done_RL'])]
        self.actualizar_puerto_B()

if __name__ == "__main__":
    root = tk.Tk()
    app = CanalApp(root)
    root.mainloop()
