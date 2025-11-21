from flask import Flask, Response
import requests
import plotly.graph_objs as go
import plotly.io as pio
from datetime import datetime, timedelta, timezone

app = Flask(__name__)

# --------------------- CONFIG FIWARE ---------------------
FIWARE_URL = "http://localhost:8666"

# CONFIGURAÇÕES ADAPTADAS PARA SEU CENÁRIO REAL
ENTITY_ID = "urn:ngsi-ld:Workstation:001"
ENTITY_TYPE = "Workstation"
ATTRIBUTE = "noise"   # <-- seu atributo correto

HEADERS = {
    "fiware-service": "smart",
    "fiware-servicepath": "/"
}

# --------------------- BUSCAR HISTÓRICO ---------------------
def get_history(last_n=30):
    url = f"{FIWARE_URL}/STH/v1/contextEntities/type/{ENTITY_TYPE}/id/{ENTITY_ID}/attributes/{ATTRIBUTE}?lastN={last_n}"

    try:
        r = requests.get(url, headers=HEADERS)
        r.raise_for_status()
        data = r.json()

        values = data["contextResponses"][0]["contextElement"]["attributes"][0]["values"]

        timestamps = []
        readings = []

        for v in values:
            utc_time = datetime.strptime(v["recvTime"], "%Y-%m-%dT%H:%M:%S.%fZ")
            local_time = utc_time - timedelta(hours=3)

            timestamps.append(local_time.strftime("%H:%M:%S"))
            readings.append(float(v["attrValue"]))

        return timestamps, readings

    except Exception as e:
        print(f"[ERRO] Falha ao obter histórico de {ATTRIBUTE}: {e}")
        return [], []


# --------------------- CRIAR DASHBOARD ---------------------
def build_dashboard():
    timestamps, noise_values = get_history()

    fig = go.Figure()

    fig.add_trace(go.Scatter(
        x=timestamps,
        y=noise_values,
        mode="lines+markers",
        name="Ruído (dB)"
    ))

    fig.update_layout(
        title="📢 Monitoramento do Ruído no Ambiente de Trabalho",
        template="plotly_dark",
        height=500,
        xaxis_title="Horário",
        yaxis_title="Nível de Ruído (dB)",
        margin=dict(l=40, r=40, t=60, b=40)
    )

    return pio.to_html(fig, full_html=False, include_plotlyjs="cdn")


# --------------------- ROTA PRINCIPAL ---------------------
@app.route("/")
def index():
    local_time = datetime.now(timezone.utc) - timedelta(hours=3)

    html = f"""
    <html>
      <head>
        <meta http-equiv="refresh" content="10">
        <title>Dashboard - Nível de Ruído</title>
        <style>
          body {{
            background-color: #111;
            color: white;
            font-family: Arial;
            padding: 20px;
            text-align: center;
          }}
          h1 {{
            color: #4FC3F7;
          }}
        </style>
      </head>
      <body>
        <h1>📢 Dashboard da Workstation Inteligente</h1>
        {build_dashboard()}
        <p>Atualizado em {local_time.strftime("%d/%m/%Y %H:%M:%S")} (UTC-3)</p>
      </body>
    </html>
    """
    return Response(html, mimetype="text/html")


# --------------------- MAIN ---------------------
if __name__ == "__main__":
    print("🚀 Dashboard disponível em http://0.0.0.0:5000")
    app.run(host="0.0.0.0", port=5000, debug=False)
