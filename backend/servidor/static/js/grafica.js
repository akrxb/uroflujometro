document.addEventListener('DOMContentLoaded', function () {
    const canvas = document.getElementById('graficaUroflow');
    if (!canvas) return;

    const ctx = canvas.getContext('2d');

    // Verificación en consola (Presiona F12 en el navegador para ver si llegan datos)
    console.log("Labels:", window.labelsGrafica);
    console.log("Caudal:", window.datosCaudal);
    console.log("Peso:", window.datosPeso);

    new Chart(ctx, {
        type: 'line',
        data: {
            labels: window.labelsGrafica,
            datasets: [
                {
                    label: 'Caudal (ml/s)',
                    data: window.datosCaudal,
                    borderColor: '#0d6efd',
                    backgroundColor: 'rgba(13, 110, 253, 0.1)',
                    borderWidth: 2,
                    fill: true,
                    tension: 0.4,
                    pointRadius: 0, // Línea continua sin puntos
                    yAxisID: 'yCaudal'
                },
                {
                    label: 'Peso Acumulado (g)',
                    data: window.datosPeso,
                    borderColor: '#6c757d',
                    borderWidth: 2,
                    borderDash: [5, 5],
                    fill: false,
                    tension: 0.1,
                    pointRadius: 0, // Línea continua sin puntos
                    yAxisID: 'yPeso'
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    display: true,
                    title: { display: true, text: 'Tiempo (muestras)' }
                },
                yCaudal: {
                    type: 'linear',
                    display: true,
                    position: 'left',
                    suggestedMin: 0,
                    title: { display: true, text: 'Caudal (ml/s)' }
                },
                yPeso: {
                    type: 'linear',
                    display: true,
                    position: 'right',
                    suggestedMin: 0,
                    title: { display: true, text: 'Peso (g)' },
                    grid: { drawOnChartArea: false }
                }
            }
        }
    });
});