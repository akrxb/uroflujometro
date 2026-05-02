from flask import Flask, render_template
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
import threading, socket, json

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///uroflow.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

# --- MODELOS ---
class Sesion(db.Model):
    id_real = db.Column(db.Integer, primary_key=True, autoincrement=True)
    paciente_id = db.Column(db.String(50))
    fecha = db.Column(db.String(20), default=lambda: datetime.now().strftime("%d/%m/%Y %H:%M"))
    v_total = db.Column(db.Float, default=0.0)
    q_max = db.Column(db.Float, default=0.0)
    muestras = db.relationship('Muestra', backref='sesion', lazy=True)

class Muestra(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    sesion_id = db.Column(db.Integer, db.ForeignKey('sesion.id_real'))
    caudal = db.Column(db.Float)
    peso = db.Column(db.Float)

with app.app_context():
    db.create_all()

# --- LÓGICA UDP ---
id_act = None
ult_est = -1

def servicio_udp():
    global id_act, ult_est
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", 9000))
    
    while True:
        try:
            data, _ = sock.recvfrom(1024)
            d = json.loads(data.decode('utf-8'))
            est = d.get('estado', 0)

            with app.app_context():
                # 1. INICIO (Flanco de subida a RECORDING=2)
                if est == 2 and ult_est != 2:
                    s = Sesion(v_total=d.get('v_total'), q_max=d.get('q_max'))
                    db.session.add(s)
                    db.session.flush() # Para obtener s.id_real antes del commit
                    s.paciente_id = f"PAC-{s.id_real:03d}"
                    id_act = s.id_real
                    print(f"🔴 Grabando: {s.paciente_id}")

                # 2. GRABACIÓN
                if est == 2 and id_act:
                    s = db.session.get(Sesion, id_act)
                    if s:
                        s.v_total, s.q_max = d.get('v_total'), d.get('q_max')
                        db.session.add(Muestra(sesion_id=id_act, caudal=d.get('c'), peso=d.get('p')))
                
                # 3. CIERRE (FINISHED=3 o IDLE=0)
                if est in [0, 3] and id_act:
                    print(f"🏁 Sesión {id_act} cerrada.")
                    id_act = None

                db.session.commit()
                ult_est = est

        except Exception as e:
            print(f"❌ Error: {e}")
            db.session.rollback()

threading.Thread(target=servicio_udp, daemon=True).start()

# --- RUTAS ---
@app.route('/')
def home(): return render_template('index.html')

@app.route('/consultas')
def consultas():
    return render_template('consultas.html', pacientes=Sesion.query.order_by(Sesion.id_real.desc()).all())

@app.route('/informe/<int:id_real>')
def informe(id_real):
    s = db.session.get(Sesion, id_real)
    if not s: return "No encontrado", 404
    m = Muestra.query.filter_by(sesion_id=id_real).order_by(Muestra.id.asc()).all()
    return render_template('informe.html', datos=s, labels=list(range(len(m))), 
                           caudal=[i.caudal for i in m], volumen=[i.peso for i in m])

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)