# 📶 Primer Parcial - Redes de Computación

## 📋 Descripción

Este repositorio contiene las soluciones y desarrollo del primer examen parcial de la materia **Redes de Computación**. El parcial abarca conceptos fundamentales de redes inalámbricas, simulación de redes y análisis de señales mediante transformadas.

## 🎯 Contenido del Parcial

### **Ejercicio 1: Análisis de Redes Inalámbricas** 🌐
**Objetivo:** Investigar y analizar los parámetros de conexión de Internet a través de un enlace inalámbrico.

**Tareas realizadas:**
- ✅ Captura de datos de conexión durante 10 minutos
- ✅ Análisis del comportamiento del enlace (5G y tasa de transferencia)
- ✅ Documentación de resultados obtenidos mediante simulación en NS-3
- ✅ Comparación entre resultados reales y resultados teóricos

**Archivos incluidos:**
- `PUNTO 1 / Punto uno .pcapng` - Medicion del trafico de red de wireshark
- `PUNTO 1/ simulacion-wifi.cc` - Codigo que permite simular la red de wireshark

### **Ejercicio 2: Simulación en Packet Tracer** 🔧
**Objetivo:** Implementar una topología de red con intercambio de mensajes de señalización usando Packet Tracer.

> **Nota:** Este ejercicio fue desarrollado completamente en Packet Tracer. La documentación incluye capturas de pantalla del proceso de configuración y los resultados obtenidos, pero no se incluyen archivos de código debido a la naturaleza gráfica de la herramienta.

**Características de la simulación:**
- Definición de topología de 3 nodos
- Configuración de intercambio de mensajes
- Análisis de conectividad entre nodos
- Implementación de enlaces guiados

### **Ejercicio 3: Transformada de Fourier** 📊
**Objetivo:** Implementar y analizar la transformada de Fourier para analizar pulsos de ondas triangulares, con representación gráfica en el dominio de la frecuencia.

**Características del análisis:**
- 🔺 Generación de tren de pulsos triangulares
- 📈 Implementación de la Transformada de Fourier
- 📉 Análisis en dominio de frecuencia
- 🎨 Visualización gráfica de resultados

**Archivos incluidos:**
- `ejercicio3/punto3.py` → Implementación principal del ejercicio.  
- `ejercicio3/plots/punto3_tiempo.png` → Gráfico del tren de pulsos triangulares en el tiempo.  
- `ejercicio3/plots/punto3_espectro_fft.png` → Espectro de magnitud de la señal (FFT).  

## 🛠️ Tecnologías Utilizadas

- **Python 3.8+** - Para análisis de Fourier y procesamiento de datos
- **NumPy** - Cálculos numéricos y transformadas
- **Matplotlib** - Visualización de gráficos y señales
- **NS-3** - Simulación de redes inalámbricas
- **Packet Tracer** - Simulación de topologías de red
- **Jupyter Notebook** - Análisis interactivo de datos

## 🔧 Herramientas de Desarrollo

| Herramienta | Versión | Propósito |
|-------------|---------|-----------|
| Python | 3.8+ | Desarrollo principal |
| NumPy | 1.21.0 | Cálculos numéricos |
| Matplotlib | 3.5.0 | Visualización |
| NS-3 | 3.44 | Simulación de redes |
| Packet Tracer | 8.2.0 | Simulación de topologías |

---

## 👥 Integrantes
- Nicolas Sánchez  
- Iker Acevedo  
- Santiago Rodríguez  

---
