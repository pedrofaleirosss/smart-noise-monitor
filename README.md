# 🔊 Smart Noise Monitor — IoT + FIWARE + ESP32 para Ambientes de Trabalho Inteligentes
### 🧠 Edge Computing And Computer Systems — FIAP | Engenharia de Software

Monitoramento inteligente de ruído em ambientes de trabalho, promovendo bem-estar, produtividade e saúde ocupacional através de IoT + FIWARE + Dashboard.

[👉 Acesse a simulação completa no Wokwi](https://wokwi.com/projects/448264576571216897)

[ 📺 Acesse o vídeo do projeto no YouTube](https://youtu.be/MDtb2ONWMeM)

---

## 🧠 Visão Geral

O Smart Noise Monitor é uma solução IoT criada para melhorar o bem-estar no ambiente de trabalho, um dos pilares essenciais no tema O Futuro do Trabalho.

Ambientes barulhentos prejudicam:

✔ Produtividade

✔ Concentração

✔ Saúde mental

✔ Relacionamentos profissionais

A solução monitora continuamente o nível de ruído do ambiente usando um sensor analógico simulado por um potenciômetro, integra com a plataforma FIWARE, e apresenta o histórico de som em um dashboard dinâmico em Python + Plotly, hospedado em uma VM na Azure.

---

## 🏗️ Arquitetura da Solução

```
+-----------------------------------------------------------+
|                      DASHBOARD PYTHON                     |
|  Flask + Plotly (porta 5000) — Gráfico do nível de ruído  |
|  Consome histórico do STH-Comet via API 8666              |
+-----------------------------------------------------------+
                 ↑
                 │ (HTTP/REST)
                 ↓
+-----------------------------------------------------------+
|                        FIWARE CLOUD                       |
|  Orion Context Broker + IoT Agent MQTT + STH-Comet        |
|  - Recebe nível de ruído                                  |
|  - Armazena histórico                                     |
|  - Envia comandos on/off (alertas remotos)                |
+-----------------------------------------------------------+
                 ↑
                 │ (MQTT)
                 ↓
+-----------------------------------------------------------+
|                       NÓ IoT (ESP32)                      |
|    Sensor de Ruído (potenciômetro) + LCD + LED + Buzzer  |
|  - Envia medições do noise (%)                            |
|  - Exibe no LCD o ruído e status                          |
|  - Alerta em caso de ruído alto                           |
+-----------------------------------------------------------+

```

---

## ⚙️ Hardware Utilizado

| Componente | Função |
|------------|-------------|
| **ESP32** |	Microcontrolador e cliente MQTT |
| **Potenciômetro (Simulação Wokwi)** |	Sensor de ruído (0–100%) |
| **LCD I2C 16x2** | Exibição local dos dados |
| **LED Onboard (GPIO 2)** | Alerta visual |
| **Buzzer (GPIO 27)** | Alerta sonoro |

---

## 💻 Software e Tecnologias
| Camada | Tecnologia |	Função |
|-----------|-----------|--------|
| IoT Device | **Arduino/ESP32** | Leitura e envio de noise + alertas |
| Backend	| **FIWARE** | Registro, armazenamento histórico e comandos |
| Dashboard | **Python Flask + Plotly** |	Visualização do ruído em gráfico |
| Cloud	| **Microsoft Azure VM Ubuntu 24.04** |	Hospedagem FIWARE + Dashboard |
| Protocolo | **MQTT + HTTP REST** | Comunicação do dispositivo com a nuvem |

--- 

## 🔄 Fluxo de Comunicação

1. O ESP32 lê o nível de ruído através do potenciômetro.
2. A informação é enviada via MQTT para o IoT Agent.
3. O IoT Agent atualiza o Orion Context Broker.
4. O STH-Comet registra o histórico no MongoDB.
5. O dashboard Flask exibe o gráfico com os últimos minutos.
6. Se o ruído for alto:
    - LED pisca
    - Buzzer emite alerta
    - LCD mostra ALTO

---

## ⚙️ Manual de Instalação

### 1️⃣ Configuração do Hardware

Simulação no Wokwi (sem hardware físico):

| Componente | Pino ESP32 |
|-------------|------------|
| Potenciômetro (ruído) |	GPIO 34 |
| LED |	GPIO 2 |
| Buzzer | GPIO 27 |
| LCD SDA/SCL	| GPIO 21 / 22 |

---

## 2️⃣ Configuração da VM no Azure

### 🖥️ Acessar a máquina virtual

```bash
cd Downloads
ssh -i <private-key-file-path> <username>@<ip-da-vm>

sudo apt update
sudo apt install docker.io
sudo apt install docker-compose
```

### ☁️ Instalação do FIWARE

```bash
git clone https://github.com/fabiocabrini/fiware
cd fiware
sudo docker compose up -d
```

### 🌐 Portas públicas necessárias (TCP)

As portas públicas devem estar liberadas no Azure (protocolo **TCP**):

| Porta | Serviço |
|--------|----------|
| **1883** | MQTT |
| **4041** | IoT Agent MQTT |
| **1026** | Orion Context Broker |
| **8666** | STH-Comet |
| **27017** | MongoDB |
| **5000** | Dashboard Flask |

---

### 3️⃣ Configuração do FIWARE

- Registrar dispositivo noisemonitor001.
- Registrar entidade urn:ngsi-ld:Workstation:001.
  - Atributo: noise (Integer).
  - Comandos disponíveis: on, off.
  
A API usada no dashboard:
```http
GET http://<IP_VM>:8666/STH/v1/contextEntities/type/Workstation/id/urn:ngsi-ld:Workstation:001/attributes/noise?lastN=30
```
---

### 4️⃣ Dashboard Python (Flask + Plotly)

#### 📁 Criar pasta e arquivo

```bash
mkdir ~/dash
cd ~/dash
nano app.py
```

#### 📦 Instalar dependências

```bash
sudo apt install python3-pip -y
pip install flask plotly requests --break-system-packages
```

#### ▶️ Executar o servidor

```bash
python3 app.py
```

Acesse em:
```
http://<IP_VM>:5000
```

---

## 🧭 Operação do Sistema

1. O ESP32 conecta ao Wi-Fi e ao FIWARE.
2. LCD exibe o ruído (%) e status (IDEAL, MODERADO, ALTO).
3. Dados são enviados via MQTT a cada 5s.
4. FIWARE grava o histórico.
5. Dashboard exibe gráfico em tempo real.
6. Ruído alto → alerta no LED/buzzer.

---

## 📂 Estrutura do Repositório

```
smart-noise-monitor/
│
├── arquivo.ino                               # Código do ESP32
├── app.py                                   # Dashboard Flask
├── requirements.txt                          # Dependências Python
├── FIWARE SmartNoise.postman_collection.json # Testes Postman
└── README.md                                 # Documentação completa
```

---

## 📷 Fotos do Projeto

<img width="1550" height="846" alt="image" src="https://github.com/user-attachments/assets/c3e65a4a-726d-4606-851b-07656845c0d3" />

<img width="1900" height="934" alt="image" src="https://github.com/user-attachments/assets/746b9ca2-d81f-44f4-85eb-3ea7b95e35da" />


---

## 👨‍💻 Integrantes

| Nome | RM |
|------|----|
| **Pedro Alves Faleiros** | 562523 |
| **Luan Felix** | 565541 |
| **João Lopes** | 565737 |

---

## 📜 Licença

Projeto acadêmico, livre para uso educacional.
