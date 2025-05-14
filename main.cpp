#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Параметры сети Wi-Fi
const char* ssid = "esp";
const char* password = "08092004";

// Настройки статического IP-адреса
WebServer server(80);

// Пины для потенциометров
const int pot1Pin = 7;   // Первый потенциометр
const int pot2Pin = 9;  // Второй потенциометр

// Переменные для данных
float voltage = 0.0;  // Значение с первого потенциометра
float amperage = 0.0;  // Значение со второго потенциометра
float power = 0.0;
// Главная страница
void handleRoot() {
    String html = R"rawliteral(
    <!DOCTYPE html>
<html lang="ru">
<head>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Показания электроэнергии</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        headerlogotxt {
            font-family: Arial, sans-serif;
            font-size: 250%;
            font-weight: 900;
            background: linear-gradient( to right, rgb(26, 73, 204),rgb(255, 153, 0));
            background-size: 200% auto;
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            -webkit-text-stroke:3px #402d57;
            background-position: center center;
            animation: textflow 2s linear infinite;
        }
        @keyframes textflow {
            from {background-position: 200% center;}
            to {background-position: 0% center;}
        }
        .containerlogo {
            width: 100%;
            max-width: 100%;
            margin: 0 auto;
            text-align: center;
            padding: 20px;
            background-color: #fbcca8;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            justify-content: center;
        }
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            color: #333;
        }

        .container {
            width: 80%;
            max-width: 900px;
            margin: 0 auto;
            margin-bottom: 5%;
            text-align: center;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        header {
            justify-content: center;
            text-align: center;
            margin-bottom: 30px;
            display: block;
        }

        h1 {
            font-size: 200%;
            color: #333;
        }

        .data {
            display: flex;
            justify-content: space-between;
            flex-wrap: wrap;
        }

        .data-item {
            flex: 1;
            margin-right: 20px;
            margin-bottom: 20px;
        }

        .data-item:last-child {
            margin-right: 0;
        }

        h2 {
            font-size: 1.5em;
            color: #555;
            margin-bottom: 10px;
        }

        .reading, .last-updated {
            font-size: 1.2em;
            color: #2a669d;
        }

        footer {
            text-align: center;
            margin-top: 30px;
            font-size: 0.9em;
            color: #777;
        }
        button {
            padding: 1%;
            font-size: large;
            font-weight: 700;
            color: #555;
        }
    </style>
</head>

<header>
    <img src="https://upload.wikimedia.org/wikipedia/ru/thumb/a/ac/%D0%A0%D1%83%D1%81%D0%93%D0%B8%D0%B4%D1%80%D0%BE.png/300px-%D0%A0%D1%83%D1%81%D0%93%D0%B8%D0%B4%D1%80%D0%BE.png">
    <div class="containerlogo">
        <headerlogotxt>НЕ РУСГИДРО</headerlogotxt>
    </div>
</header>
<body>
    <div class="container">
        <header>
            <h1>Показания электроэнергии</h1>
        </header>
        
        <section class="data">
            <div class="data-item">
                <h2>Предыдущее показание</h2>
                <p class="reading">1250 кВт·ч</p>
            </div>
            <div class="data-item">
                <h2>Текущее показание</h2>
                <p class="reading">Мощность: )rawliteral" + String(power) + R"rawliteral(кВт·ч</p>
            </div>
            <div class="data-item">
                <h2>Дата последнего обновления</h2>
                <p class="last-updated"><time id="currentTime"></time></p>
                <script>
                  var timeElement = document.getElementById('currentTime');
                  setInterval(function () {
                    var currentTime = new Date();
                    timeElement.textContent = currentTime.toLocaleDateString('ru');
                  }, 1000);
                </script>            </div>
        </section>
        <div class="data-item">
            <button>Передать показания</button>
        </div>
    </div>
    <div class="container">
        <header>
            <h1>Графики</h1>
        </header>
        <!---->
    <!---->
    <canvas id="power"></canvas>
    <script>
        const ctx2 = document.getElementById('power').getContext('2d');
        const power = new Chart(ctx2, {
            type: 'line',
            data: {
                labels: [],
                datasets: [
                    { label: 'Мощность', data: [], borderColor: 'green', fill: false },
                ]
            },
            options: {
                responsive: true,
                scales: {
                    x: { title: { display: true, text: 'Время' } },
                    y: { title: { display: true, text: 'кВт' } }
                }
            }
        });

        async function fetchData() {
            const response = await fetch('/data');
            const data = await response.json();
            const time = new Date().toLocaleTimeString();

            power.data.labels.push(time);
            power.data.datasets[0].data.push(data.power);

            if (power.data.labels.length > 100) {
                power.data.labels.shift();
                power.data.datasets[0].data.shift();
                }

                power.update();
        }

        setInterval(fetchData, 1000); // Обновление данных каждые 1 секунду
    </script>
        <!---->
        <!---->
    <canvas id="voltage"></canvas>
    <script>
        const ctx3 = document.getElementById('voltage').getContext('2d');
        const voltage = new Chart(ctx3, {
            type: 'line',
            data: {
                labels: [],
                datasets: [
                    { label: 'Напряжение', data: [], borderColor: 'blue', fill: false },
                ]
            },
            options: {
                responsive: true,
                scales: {
                    x: { title: { display: true, text: 'Время' } },
                    y: { title: { display: true, text: 'Значение (210-250)' } }
                }
            }
        });

        async function fetchData() {
            const response = await fetch('/data');
            const data = await response.json();
            const time = new Date().toLocaleTimeString();

            voltage.data.labels.push(time);
            voltage.data.datasets[0].data.push(data.voltage);

            if (voltage.data.labels.length > 100) {
                voltage.data.labels.shift();
                voltage.data.datasets[0].data.shift();
                }

                voltage.update();
        }
setInterval(fetchData, 1000); // Обновление данных каждые 1 секунду
    </script>
        <!---->
        <!---->
    <canvas id="amperage"></canvas>
    <script>
        const ctx4 = document.getElementById('amperage').getContext('2d');
        const amperage = new Chart(ctx4, {
            type: 'line',
            data: {
                labels: [],
                datasets: [
                    { label: 'Ток', data: [], borderColor: 'red', fill: false },
                ]
            },
            options: {
                responsive: true,
                scales: {
                    x: { title: { display: true, text: 'Время' } },
                    y: { title: { display: true, text: 'Значение (0-100)' } }
                }
            }
        });

        async function fetchData() {
            const response = await fetch('/data');
            const data = await response.json();
            const time = new Date().toLocaleTimeString();

            amperage.data.labels.push(time);
            amperage.data.datasets[0].data.push(data.amperage);

            if (amperage.data.labels.length > 100) {
                amperage.data.labels.shift();
                amperage.data.datasets[0].data.shift();
                }

                amperage.update();
        }

        setInterval(fetchData, 1000); // Обновление данных каждые 1 секунду
    </script>
        <!---->
    </div>
    </div>
    <footer>
        <p>&copy; 2024 БИК-22-ИВ1. Все права защищены.</p>
    </footer>
</body>
</html>

    )rawliteral";

    server.send(200, "text/html", html);
}

// Отправка данных
void handleData() {
    voltage = map(analogRead(pot1Pin),0,4095,210,250);
    amperage = map(analogRead(pot2Pin),0,4095,0,100);
    power = voltage * amperage * 0.001;
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["voltage"] = voltage;
    jsonDoc["amperage"] = amperage;
    jsonDoc["power"] = power;
    String json;
    serializeJson(jsonDoc, json);
    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(115200);

    // Настройка пинов для потенциометров
    pinMode(pot1Pin, INPUT);
    pinMode(pot2Pin, INPUT);

    // Настройка Wi-Fi с фиксированным IP
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Подключение к Wi-Fi...");
    }

    Serial.println("Wi-Fi подключен.");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());

    // Настройка маршрутов
    server.on("/", handleRoot);
    server.on("/data", handleData);

    server.begin();
    Serial.println("Сервер запущен.");
}

void loop() {
    server.handleClient();
}
