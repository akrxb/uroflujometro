from flask import Flask, render_template, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime

app = Flask(__name__)

# Configuración de la base de datos local (Libro de registro)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///uroflow.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

# Modelo de la base de datos
class Informe(db.Model):
    id_db = db.Column(db.Integer, primary_key=True)
    id_paciente = db.Column(db.String(50))
    v_total = db.Column(db.Float)
    q_max = db.Column(db.Float)
    ts_inicio = db.Column(db.Integer) # Guardamos para el zoom de Grafana
    ts_fin = db.Column(db.Integer)
    fecha_formateada = db.Column(db.String(100))

# Crear la base de datos al arrancar
with app.app_context():
    db.create_all()

@app.route('/')
def home():
    return render_template('index.html')

# --- ESTA ES LA RUTA QUE LLAMA NODE-RED ---
@app.route('/api/finalizar_prueba', methods=['POST'])
def finalizar_prueba():
    data = request.json
    nuevo_informe = Informe(
        id_paciente=str(data['id']),
        v_total=data['v_total'],
        q_max=data['q_max'],
        ts_inicio=data['ts_inicio'],
        ts_fin=data['ts_fin'],
        fecha_formateada=datetime.now().strftime('%d/%m/%Y %H:%M')
    )
    db.session.add(nuevo_informe)
    db.session.commit()
    return jsonify({"mensaje": "Informe guardado"}), 200

@app.route('/consultas')
def consultas():
    # Consultamos todos los informes guardados
    informes_db = Informe.query.order_by(Informe.id_db.desc()).all()
    
    # Adaptamos los nombres a lo que espera tu consultas.html (p.id, p.fecha...)
    pacientes_para_web = []
    for i in informes_db:
        pacientes_para_web.append({
            "id": i.id_paciente,
            "fecha": i.fecha_formateada,
            "v_total": i.v_total,
            "q_max": i.q_max,
            "id_real": i.id_db # Necesario para el enlace al informe
        })
    
    return render_template('consultas.html', pacientes=pacientes_para_web)

@app.route('/informe/<int:id_real>')
def informe(id_real):
    inf = Informe.query.get_or_404(id_real)
    
    # Preparamos los datos para informe.html
    datos_web = {
        "id": inf.id_paciente,
        "fecha": inf.fecha_formateada,
        "v_total": inf.v_total,
        "q_max": inf.q_max,
        "ts_inicio": inf.ts_inicio,
        "ts_fin": inf.ts_fin
    }
    return render_template('informe.html', datos=datos_web)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)