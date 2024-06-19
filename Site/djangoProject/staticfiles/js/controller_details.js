document.addEventListener("DOMContentLoaded", function() {
    var recordsDataElement = document.getElementById('records-data');
    if (recordsDataElement) {
        try {
            var recordsDataText = recordsDataElement.textContent;

            var records = JSON.parse(recordsDataText);
            //console.log('Records data text:', recordsDataText); // Debug output
            //console.log('Parsed records:', records); // Debug output

            var labels = records.map(function(record) {
                return new Date(record.timestamp);
            });

            var temperatureData = records.map(function(record) {
                return record.temperature;
            });
            var outsideTemperatureData = records.map(function(record) {
                return record.outside_temperature;
            });
            var humidityData = records.map(function(record) {
                return record.humidity;
            });
            var outsideHumidityData = records.map(function(record) {
                return record.outside_humidity;
            });
            var weightData = records.map(function(record) {
                return record.weight;
            });
            var voltageData = records.map(function(record) {
                return record.voltage;
            });
            var signalStrengthData = records.map(function(record) {
                return record.signal_strength;
            });

            // Данные для свечного графика изменения веса
            var weightChangeData = [];
            for (var i = 1; i < weightData.length; i++) {
                weightChangeData.push([
                    new Date(records[i].timestamp).getTime(), // timestamp
                    weightData[i - 1], // open
                    Math.max(weightData[i], weightData[i - 1]), // high
                    Math.min(weightData[i], weightData[i - 1]), // low
                    weightData[i] // close
                ]);
            }

            var commonOptions = {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    x: {
                        ticks: { color: '#333' },
                        grid: { color: '#ccc' },
                        type: 'time',
                        time: {
                            unit: 'day',
                            tooltipFormat: 'll'
                        }
                    },
                    y: {
                        ticks: { color: '#333' },
                        grid: { color: '#ccc' }
                    }
                },
                plugins: {
                    legend: {
                        display: true,
                        position: 'bottom',
                        labels: {
                            usePointStyle: true,
                            pointStyle: 'circle',
                            color: '#333'
                        }
                    },
                    tooltip: {
                        backgroundColor: '#fff',
                        titleColor: '#333',
                        bodyColor: '#333',
                        borderColor: '#ccc',
                        borderWidth: 1
                    },
                    title: {
                        display: true,
                        color: '#333',
                        font: {
                            size: 20
                        }
                    }
                },
                elements: {
                    point: {
                        radius: 0
                    }
                }
            };

            if (document.getElementById("temperature-chart")) {
                new Chart(document.getElementById("temperature-chart"), {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [
                            {
                                label: 'Внутренняя Темп.',
                                data: temperatureData,
                                borderColor: 'rgba(255, 99, 132, 1)',
                                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                                fill: true,
                                tension: 0.1
                            },
                            {
                                label: 'Внешняя Темп.',
                                data: outsideTemperatureData,
                                borderColor: 'rgba(54, 162, 235, 1)',
                                backgroundColor: 'rgba(54, 162, 235, 0.2)',
                                fill: true,
                                tension: 0.1
                            }
                        ]
                    },
                    options: Object.assign({}, commonOptions, {
                        plugins: {
                            title: {
                                text: 'Температура'
                            }
                        }
                    })
                });
            }

            if (document.getElementById("humidity-chart")) {
                new Chart(document.getElementById("humidity-chart"), {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [
                            {
                                label: 'Внутренняя Влажн.',
                                data: humidityData,
                                borderColor: 'rgba(75, 192, 192, 1)',
                                backgroundColor: 'rgba(75, 192, 192, 0.2)',
                                fill: true,
                                tension: 0.1
                            },
                            {
                                label: 'Внешняя Влажн.',
                                data: outsideHumidityData,
                                borderColor: 'rgba(153, 102, 255, 1)',
                                backgroundColor: 'rgba(153, 102, 255, 0.2)',
                                fill: true,
                                tension: 0.1
                            }
                        ]
                    },
                    options: Object.assign({}, commonOptions, {
                        plugins: {
                            title: {
                                text: 'Влажность'
                            }
                        }
                    })
                });
            }

            if (document.getElementById("weight-chart")) {
                new Chart(document.getElementById("weight-chart"), {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [{
                            label: 'Вес',
                            data: weightData,
                            borderColor: 'rgba(255, 159, 64, 1)',
                            backgroundColor: 'rgba(255, 159, 64, 0.2)',
                            fill: true,
                            tension: 0.1
                        }]
                    },
                    options: Object.assign({}, commonOptions, {
                        plugins: {
                            title: {
                                text: 'Вес'
                            }
                        }
                    })
                });
            }

            // Использование Highcharts для свечного графика изменения веса
            if (document.getElementById("weight-change-chart")) {
                Highcharts.stockChart('weight-change-chart', {
                    chart: {
                        backgroundColor: '#f8f9fa',
                        style: {
                            fontFamily: 'Arial, sans-serif'
                        }
                    },
                    rangeSelector: {
                        selected: 1
                    },
                    title: {
                        text: 'Изменение веса по дням',
                        style: {
                            color: '#333',
                            fontWeight: 'bold'
                        }
                    },
                    series: [{
                        type: 'candlestick',
                        name: 'Вес',
                        data: weightChangeData,
                        color: 'red',
                        upColor: 'green',
                        lineColor: 'black',
                        upLineColor: 'black',
                        dataGrouping: {
                            units: [
                                ['day', [1]]
                            ]
                        }
                    }],
                    xAxis: {
                        labels: {
                            style: {
                                color: '#333'
                            }
                        },
                        gridLineColor: '#ccc'
                    },
                    yAxis: {
                        labels: {
                            style: {
                                color: '#333'
                            }
                        },
                        gridLineColor: '#ccc'
                    }
                });
            }

            if (document.getElementById("voltage-chart")) {
                new Chart(document.getElementById("voltage-chart"), {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [{
                            label: 'Напряжение 🔋',
                            data: voltageData,
                            borderColor: 'rgba(153, 102, 255, 1)',
                            backgroundColor: 'rgba(153, 102, 255, 0.2)',
                            fill: true,
                            tension: 0.1
                        }]
                    },
                    options: Object.assign({}, commonOptions, {
                        plugins: {
                            title: {
                                text: 'Напряжение'
                            }
                        }
                    })
                });
            }

            if (document.getElementById("signal-chart")) {
                new Chart(document.getElementById("signal-chart"), {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [{
                            label: 'Сигнал 📶',
                            data: signalStrengthData,
                            borderColor: 'rgba(255, 206, 86, 1)',
                            backgroundColor: 'rgba(255, 206, 86, 0.2)',
                            fill: true,
                            tension: 0.1
                        }]
                    },
                    options: Object.assign({}, commonOptions, {
                        plugins: {
                            title: {
                                text: 'Уровень сигнала'
                            }
                        }
                    })
                });
            }

        } catch (error) {
            console.error('Error parsing JSON:', error);
        }
    } else {
        console.error('Element with ID "records-data" not found.');
    }
});
