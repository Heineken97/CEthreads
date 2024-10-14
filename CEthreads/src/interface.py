import tkinter as tk
import struct
import os
import time
import threading
from collections import deque

# Define constants for the maximum number of ships and the canal size
MAX_SHIPS = 5  # Maximum number of ships
CANAL_SIZE = 5  # Size of the canal

# Define the data format for the pipe
INTERFACE_DATA_FORMAT = 'ii' * (MAX_SHIPS * 5) + 'iiii'  # 50 integers for the ships and 4 additional integers

class CanalApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Simulación de Canal Marítimo")

        # Create a main frame to hold the UI elements
        main_frame = tk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Initialize the port and canal UI elements
        self.init_ports(main_frame)
        self.init_canal(main_frame)

        # Create labels for additional information
        self.init_labels(main_frame)

        # Initialize the pipe for inter-process communication
        self.pipe_fd = None
        self.start_pipe_check()

    def init_ports(self, main_frame):
        # Create frames for Port A and Port B
        self.puerto_A_salida = deque()
        self.puerto_A_entrada = deque()
        self.puerto_B_salida = deque()
        self.puerto_B_entrada = deque()

        # Create frames for Port A Out, Port A In, Port B Out, and Port B In
        puerto_A_salida_frame = tk.Frame(main_frame)
        puerto_A_salida_frame.grid(row=0, column=0, padx=10)
        puerto_A_entrada_frame = tk.Frame(main_frame)
        puerto_A_entrada_frame.grid(row=1, column=0, padx=10)
        puerto_B_salida_frame = tk.Frame(main_frame)
        puerto_B_salida_frame.grid(row=0, column=2, padx=10)
        puerto_B_entrada_frame = tk.Frame(main_frame)
        puerto_B_entrada_frame.grid(row=1, column=2, padx=10)

        # Create labels for Port A Out, Port A In, Port B Out, and Port B In
        tk.Label(puerto_A_salida_frame, text="Puerto A Salida").pack()
        tk.Label(puerto_A_entrada_frame, text="Puerto A Entrada").pack()
        tk.Label(puerto_B_salida_frame, text="Puerto B Salida").pack()
        tk.Label(puerto_B_entrada_frame, text="Puerto B Entrada").pack()

        # Create frames to hold the boat labels for each port
        self.puerto_A_salida_frame = tk.Frame(puerto_A_salida_frame)
        self.puerto_A_salida_frame.pack()
        self.puerto_A_entrada_frame = tk.Frame(puerto_A_entrada_frame)
        self.puerto_A_entrada_frame.pack()
        self.puerto_B_salida_frame = tk.Frame(puerto_B_salida_frame)
        self.puerto_B_salida_frame.pack()
        self.puerto_B_entrada_frame = tk.Frame(puerto_B_entrada_frame)
        self.puerto_B_entrada_frame.pack()

        # Initialize the boat labels for each port
        self.actualizar_puerto_A()
        self.actualizar_puerto_A_entrada()
        self.actualizar_puerto_B()
        self.actualizar_puerto_B_entrada()

    def init_canal(self, main_frame):
        # Create a frame for the canal
        canal_frame = tk.Frame(main_frame)
        canal_frame.grid(row=0, column=1, rowspan=2, padx=20)
        tk.Label(canal_frame, text="Canal").pack()

        # Create labels for the canal positions
        self.canal_labels = []
        for i in range(CANAL_SIZE):
            label = tk.Label(canal_frame, text="Espacio " + str(i + 1), width=10, relief="ridge")
            label.pack(side=tk.LEFT)
            self.canal_labels.append(label)

    def init_labels(self, main_frame):
        # Create labels for additional information
        self.canal_length_label = tk.Label(main_frame, text="Longitud del canal:")
        self.canal_length_label.grid(row=2, column=1, padx=10)
        self.actual_direction_label = tk.Label(main_frame, text="Dirección actual:")
        self.actual_direction_label.grid(row=3, column=1, padx=10)
        self.method_label = tk.Label(main_frame, text="Método de flujo:")
        self.method_label.grid(row=4, column=1, padx=10)
        self.scheduler_label = tk.Label(main_frame, text="Tipo de programador:")
        self.scheduler_label.grid(row=5, column=1, padx=10)
        

    def draw_boat(self, label, boat_type, position):
        # Draw a boat in the label
        if boat_type == 0:
            label.config(text=f" ⛴️ ")
        elif boat_type == 1:
            label.config(text=f" 🚢 ")
        elif boat_type == 2:
            label.config(text=f" 🎣 ")
        else:
            label.config(text="")


    def actualizar_puerto_A(self):
        # Update the UI for Port A Out
        for widget in self.puerto_A_salida_frame.winfo_children():
            widget.destroy()
    
        # Display the boats that are currently leaving Port A
        for i in range(MAX_SHIPS):
            label = tk.Label(self.puerto_A_salida_frame, text="")
            label.pack(side=tk.LEFT, padx=10)
            if i < len(self.puerto_A_salida):
                self.draw_boat(label, self.puerto_A_salida[i][0], self.puerto_A_salida[i][1])
            else:
                label.config(text="")  # Leave an empty space for departed boats
    
    def actualizar_puerto_A_entrada(self):
        # Update the UI for Port A In
        for widget in self.puerto_A_entrada_frame.winfo_children():
            widget.destroy()
    
        # Display the boats that are currently arriving at Port A
        for i in range(MAX_SHIPS):
            label = tk.Label(self.puerto_A_entrada_frame, text="")
            label.pack(side=tk.LEFT, padx=10)
            if i < len(self.puerto_A_entrada):
                self.draw_boat(label, self.puerto_A_entrada[i][0], self.puerto_A_entrada[i][1])
            else:
                label.config(text="")  # Leave an empty space for unoccupied slots
    
    def actualizar_puerto_B(self):
        # Update the UI for Port B Out
        for widget in self.puerto_B_salida_frame.winfo_children():
            widget.destroy()
    
        # Display the boats that are currently leaving Port B
        for i in range(MAX_SHIPS):
            label = tk.Label(self.puerto_B_salida_frame, text="")
            label.pack(side=tk.LEFT, padx=10)
            if i < len(self.puerto_B_salida):
                self.draw_boat(label, self.puerto_B_salida[i][0], self.puerto_B_salida[i][1])
            else:
                label.config(text="")  # Leave an empty space for departed boats
    
    def actualizar_puerto_B_entrada(self):
        # Update the UI for Port B In
        for widget in self.puerto_B_entrada_frame.winfo_children():
            widget.destroy()
    
        # Display the boats that are currently arriving at Port B
        for i in range(MAX_SHIPS):
            label = tk.Label(self.puerto_B_entrada_frame, text="")
            label.pack(side=tk.LEFT, padx=10)
            if i < len(self.puerto_B_entrada):
                self.draw_boat(label, self.puerto_B_entrada[i][0], self.puerto_B_entrada[i][1])
            else:
                label.config(text="")  # Leave an empty space for unoccupied slots
    def start_pipe_check(self):
        # Create the pipe if it doesn't exist
        try:
            os.mkfifo('mock_serial_port')
        except OSError as e:
            if e.errno != 17:  # Error 17 is "File exists"
                raise

        # Open the pipe in blocking mode
        self.pipe_fd = os.open('mock_serial_port', os.O_RDONLY)

        # Start checking the pipe in a separate thread
        threading.Thread(target=self.check_pipe, daemon=True).start()

    def check_pipe(self):
        while True:
            self.read_from_pipe()
            time.sleep(0.1)

    def read_from_pipe(self):
        try:
            # Read the exact size of the InterfaceData structure
            data_size = struct.calcsize(INTERFACE_DATA_FORMAT)
            data = os.read(self.pipe_fd, data_size)

            if len(data) == 0:
                # No data available, not a critical error
                return

            elif len(data) != data_size:
                print(f"Error: received data size {len(data)} is incorrect")
                return

            # Unpack the received data
            interface_data = struct.unpack(INTERFACE_DATA_FORMAT, data)

            # Thread-safe UI update
            self.root.after(0, self.update_gui, interface_data)

        except OSError as e:
            if e.errno != 11:  # Errno 11 is "Resource temporarily unavailable", ignore it
                print(f"Error reading from pipe: {e}")

        except struct.error as e:
            print(f"Error unpacking data: {e}")

    def update_gui(self, interface_data):
        # Extract the different data components from interface_data tuple
        queue_LR_data = interface_data[:MAX_SHIPS * 2]
        queue_RL_data = interface_data[MAX_SHIPS * 2:MAX_SHIPS * 4]
        midcanal_data = interface_data[MAX_SHIPS * 4:MAX_SHIPS * 6]
        done_LR_ships = interface_data[MAX_SHIPS * 6:MAX_SHIPS * 8]
        done_RL_ships = interface_data[MAX_SHIPS * 8:MAX_SHIPS * 10]
        canal_length = interface_data[MAX_SHIPS * 10]
        actual_direction = interface_data[MAX_SHIPS * 10 + 1]
        method = interface_data[MAX_SHIPS * 10 + 2]
        scheduler = interface_data[MAX_SHIPS * 10 + 3]
        
        # Update UI components based on the received data
        self.canal_length_label.config(text=f"Longitud del canal: {canal_length}")
        self.actual_direction_label.config(text=f"Dirección actual: {'L->R' if actual_direction == 0 else 'R->L'}")
        
        # Map method and scheduler to their corresponding names
        method_names = {0: "EQUITY", 1: "SIGN", 2: "SIGN"}
        scheduler_names = {0: "ROUND_ROBIN", 1: "PRIORITY", 2: "SJF", 3: "FCFS", 4: "REAL_TIME"}
        
        self.method_label.config(text=f"Método de flujo: {method_names.get(method, 'Unknown')}")
        self.scheduler_label.config(text=f"Tipo de programador: {scheduler_names.get(scheduler, 'Unknown')}")
        
        # Update ports
        self.puerto_A_salida = deque([(queue_LR_data[i], queue_LR_data[i+1]) for i in range(0, MAX_SHIPS*2, 2)])
        self.puerto_A_entrada = deque([(done_LR_ships[i], done_LR_ships[i+1]) for i in range(0, MAX_SHIPS*2, 2)])
        self.puerto_B_salida = deque([(queue_RL_data[i], queue_RL_data[i+1]) for i in range(0, MAX_SHIPS*2, 2)])
        self.puerto_B_entrada = deque([(done_RL_ships[i], done_RL_ships[i+1]) for i in range(0, MAX_SHIPS*2, 2)])
        
        self.actualizar_puerto_A()
        self.actualizar_puerto_A_entrada()
        self.actualizar_puerto_B()
        self.actualizar_puerto_B_entrada()
        
        # Update canal
        midcanal_ships = []
        for i in range(0, len(midcanal_data), 2):
            if 0 < midcanal_data[i+1] < canal_length:  # Only consider ships that are actually in the canal
                midcanal_ships.append((midcanal_data[i], midcanal_data[i+1]))
        midcanal_ships.sort(key=lambda x: x[1] if actual_direction == 0 else -x[1])  # Sort ships based on direction
        
        for i in range(CANAL_SIZE):
            if i < len(midcanal_ships):
                self.draw_boat(self.canal_labels[i], midcanal_ships[i][0], midcanal_ships[i][1])
            else:
                self.canal_labels[i].config(text="Espacio " + str(i + 1))
    

if __name__ == "__main__":
    root = tk.Tk()
    app = CanalApp(root)
    root.mainloop()
