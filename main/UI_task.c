#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "cJSON.h"

#include "esp_wifi.h"
#include <sys/socket.h>

static esp_err_t status_handler(httpd_req_t *req);

#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif

static const char *TAG = "LINGUO_DEVICE";

#ifndef HTTPD_WS_TYPE_TEXT
typedef enum {
    HTTPD_WS_TYPE_CONTINUE = 0x0,
    HTTPD_WS_TYPE_TEXT = 0x1,
    HTTPD_WS_TYPE_BINARY = 0x2,
    HTTPD_WS_TYPE_CLOSE = 0x8,
    HTTPD_WS_TYPE_PING = 0x9,
    HTTPD_WS_TYPE_PONG = 0xa
} httpd_ws_type_t;

typedef struct {
    bool final;
    bool fragmented;
    httpd_ws_type_t type;
    uint8_t *payload;
    size_t len;
} httpd_ws_frame_t;
#endif

void control_hardware(void);

// 设备状态结构体
typedef struct {
    uint8_t intensity;          // 强度 1-30
    bool switch1;              // 开关1
    bool switch2;              // 开关2  
    bool switch3;              // 开关3
    bool switch4;              // 开关4
    uint8_t mode;              // 模式 1-10
    uint8_t slider_value;      // 滑杆值 0-100
    uint16_t timer_seconds;    // 计时器秒数
    bool timer_running;        // 计时器运行状态
} device_status_t;

static device_status_t device_status = {
    .intensity = 1,
    .switch1 = false,
    .switch2 = false,
    .switch3 = false,
    .switch4 = false,
    .mode = 1,
    .slider_value = 0,
    .timer_seconds = 0,
    .timer_running = false
};

// WebSocket客户端管理
static httpd_handle_t server = NULL;
static int websocket_fd = -1;

// WiFi配置
#define WIFI_SSID      "LINGUO_DEVICE"
#define WIFI_PASS      "12345678"
#define WIFI_CHANNEL   1
#define MAX_STA_CONN   4

// HTML页面内容（嵌入式）
static const char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>灵络 - 痛经治疗仪</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
            color: #333;
        }
        
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: rgba(255, 255, 255, 0.95);
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
        }
        
        .header {
            text-align: center;
            margin-bottom: 30px;
        }
        
        .brand {
            font-size: 2.5em;
            font-weight: bold;
            background: linear-gradient(45deg, #667eea, #764ba2);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 10px;
        }
        
        .subtitle {
            color: #666;
            font-size: 1.1em;
        }
        
        .control-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 25px;
            margin-bottom: 30px;
        }
        
        .control-panel {
            background: white;
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 8px 25px rgba(0,0,0,0.08);
            border: 2px solid #f0f0f0;
        }
        
        .panel-title {
            font-size: 1.3em;
            font-weight: 600;
            margin-bottom: 20px;
            color: #333;
            border-bottom: 3px solid #667eea;
            padding-bottom: 10px;
        }
        
        .intensity-grid {
            display: grid;
            grid-template-columns: repeat(6, 1fr);
            gap: 8px;
            margin-bottom: 15px;
        }
        
        .intensity-btn {
            aspect-ratio: 1;
            border: 2px solid #ddd;
            background: white;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.2s;
            font-weight: 600;
        }
        
        .intensity-btn:hover {
            border-color: #667eea;
            transform: scale(1.05);
        }
        
        .intensity-btn.active {
            background: #667eea;
            color: white;
            border-color: #667eea;
        }
        
        .switch-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
        }
        
        .switch-item {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 10px;
            border: 2px solid transparent;
            transition: all 0.2s;
        }
        
        .switch-item.active {
            background: #e3f2fd;
            border-color: #667eea;
        }
        
        .toggle-switch {
            position: relative;
            width: 50px;
            height: 24px;
        }
        
        .toggle-switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 24px;
        }
        
        .slider:before {
            position: absolute;
            content: "";
            height: 18px;
            width: 18px;
            left: 3px;
            bottom: 3px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }
        
        input:checked + .slider {
            background-color: #667eea;
        }
        
        input:checked + .slider:before {
            transform: translateX(26px);
        }
        
        .mode-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 10px;
        }
        
        .mode-btn {
            padding: 15px;
            border: 2px solid #ddd;
            background: white;
            border-radius: 10px;
            cursor: pointer;
            transition: all 0.2s;
            text-align: center;
            font-weight: 500;
        }
        
        .mode-btn:hover {
            border-color: #667eea;
            transform: translateY(-2px);
        }
        
        .mode-btn.active {
            background: #667eea;
            color: white;
            border-color: #667eea;
        }
        
        .slider-container {
            margin: 20px 0;
        }
        
        .slider-input {
            width: 100%;
            height: 8px;
            border-radius: 4px;
            background: #ddd;
            outline: none;
            -webkit-appearance: none;
        }
        
        .slider-input::-webkit-slider-thumb {
            appearance: none;
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            box-shadow: 0 2px 6px rgba(0,0,0,0.2);
        }
        
        .timer-display {
            text-align: center;
            font-size: 3em;
            font-weight: bold;
            color: #667eea;
            margin: 20px 0;
            font-family: 'Courier New', monospace;
        }
        
        .timer-controls {
            display: flex;
            gap: 15px;
            justify-content: center;
            flex-wrap: wrap;
        }
        
        .btn {
            padding: 12px 25px;
            border: none;
            border-radius: 25px;
            cursor: pointer;
            font-weight: 600;
            transition: all 0.2s;
            font-size: 1em;
        }
        
        .btn-primary {
            background: #667eea;
            color: white;
        }
        
        .btn-primary:hover {
            background: #5a6fd8;
            transform: translateY(-2px);
        }
        
        .btn-success {
            background: #28a745;
            color: white;
        }
        
        .btn-danger {
            background: #dc3545;
            color: white;
        }
        
        .status-bar {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 15px;
            margin-top: 20px;
            text-align: center;
            border: 2px solid #e9ecef;
        }
        
        .connection-status {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            font-weight: 500;
        }
        
        .status-dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background: #dc3545;
        }
        
        .status-dot.connected {
            background: #28a745;
            animation: pulse 2s infinite;
        }
        
        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }
        
        @media (max-width: 768px) {
            .container {
                padding: 20px;
            }
            
            .control-grid {
                grid-template-columns: 1fr;
            }
            
            .intensity-grid {
                grid-template-columns: repeat(5, 1fr);
            }
            
            .brand {
                font-size: 2em;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="brand">灵络</div>
            <div class="subtitle">智能痛经治疗仪</div>
        </div>
        
        <div class="control-grid">
            <!-- 强度调节 -->
            <div class="control-panel">
                <div class="panel-title">治疗强度</div>
                <div class="intensity-grid" id="intensityGrid"></div>
                <div style="text-align: center; margin-top: 10px;">
                    <span>当前强度: </span><span id="currentIntensity" style="font-weight: bold; color: #667eea;">1</span>
                </div>
            </div>
            
            <!-- 功能开关 -->
            <div class="control-panel">
                <div class="panel-title">功能开关</div>
                <div class="switch-grid">
                    <div class="switch-item" id="switch1Item">
                        <span>加热功能</span>
                        <label class="toggle-switch">
                            <input type="checkbox" id="switch1">
                            <span class="slider"></span>
                        </label>
                    </div>
                    <div class="switch-item" id="switch2Item">
                        <span>按摩模式</span>
                        <label class="toggle-switch">
                            <input type="checkbox" id="switch2">
                            <span class="slider"></span>
                        </label>
                    </div>
                    <div class="switch-item" id="switch3Item">
                        <span>舒缓模式</span>
                        <label class="toggle-switch">
                            <input type="checkbox" id="switch3">
                            <span class="slider"></span>
                        </label>
                    </div>
                    <div class="switch-item" id="switch4Item">
                        <span>智能模式</span>
                        <label class="toggle-switch">
                            <input type="checkbox" id="switch4">
                            <span class="slider"></span>
                        </label>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="control-grid">
            <!-- 治疗模式 -->
            <div class="control-panel">
                <div class="panel-title">治疗模式</div>
                <div class="mode-grid" id="modeGrid"></div>
            </div>
            
            <!-- 强度滑杆 -->
            <div class="control-panel">
                <div class="panel-title">精细调节</div>
                <div class="slider-container">
                    <input type="range" min="0" max="100" value="0" class="slider-input" id="intensitySlider">
                    <div style="display: flex; justify-content: space-between; margin-top: 10px; font-size: 0.9em; color: #666;">
                        <span>最低</span>
                        <span id="sliderValue">0%</span>
                        <span>最高</span>
                    </div>
                </div>
            </div>
        </div>
        
        <!-- 计时器 -->
        <div class="control-panel">
            <div class="panel-title">治疗计时器</div>
            <div class="timer-display" id="timerDisplay">00:00</div>
            <div class="timer-controls">
                <button class="btn btn-primary" onclick="setTimer(5)">5分钟</button>
                <button class="btn btn-primary" onclick="setTimer(10)">10分钟</button>
                <button class="btn btn-primary" onclick="setTimer(15)">15分钟</button>
                <button class="btn btn-primary" onclick="setTimer(30)">30分钟</button>
                <button class="btn btn-success" id="startStopBtn" onclick="toggleTimer()">开始</button>
                <button class="btn btn-danger" onclick="resetTimer()">重置</button>
            </div>
        </div>
        
        <div class="status-bar">
            <div class="connection-status">
                <span class="status-dot" id="statusDot"></span>
                <span id="connectionStatus">连接中...</span>
            </div>
        </div>
    </div>
    
    <script>
        let ws;
        let timerInterval;
        let currentTimer = 0;
        let timerRunning = false;
        
        // WebSocket连接
        function connectWebSocket() {
            ws = new WebSocket('ws://' + window.location.hostname + '/ws');
            
            ws.onopen = function() {
                document.getElementById('statusDot').classList.add('connected');
                document.getElementById('connectionStatus').textContent = '已连接';
                console.log('WebSocket connected');
            };
            
            ws.onmessage = function(event) {
                try {
                    const data = JSON.parse(event.data);
                    updateUI(data);
                } catch (e) {
                    console.error('解析消息失败:', e);
                }
            };
            
            ws.onclose = function() {
                document.getElementById('statusDot').classList.remove('connected');
                document.getElementById('connectionStatus').textContent = '连接断开';
                console.log('WebSocket disconnected');
                // 3秒后重连
                setTimeout(connectWebSocket, 3000);
            };
            
            ws.onerror = function(error) {
                console.error('WebSocket error:', error);
            };
        }
        
        // 发送数据到ESP32
        function sendData(data) {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify(data));
            }
        }
        
        // 更新UI
        function updateUI(data) {
            if (data.intensity !== undefined) {
                document.getElementById('currentIntensity').textContent = data.intensity;
                updateIntensityButtons(data.intensity);
            }
            
            if (data.slider_value !== undefined) {
                document.getElementById('intensitySlider').value = data.slider_value;
                document.getElementById('sliderValue').textContent = data.slider_value + '%';
            }
            
            if (data.mode !== undefined) {
                updateModeButtons(data.mode);
            }
            
            ['switch1', 'switch2', 'switch3', 'switch4'].forEach(switchName => {
                if (data[switchName] !== undefined) {
                    document.getElementById(switchName).checked = data[switchName];
                    const item = document.getElementById(switchName + 'Item');
                    if (data[switchName]) {
                        item.classList.add('active');
                    } else {
                        item.classList.remove('active');
                    }
                }
            });
            
            if (data.timer_seconds !== undefined) {
                currentTimer = data.timer_seconds;
                updateTimerDisplay();
            }
            
            if (data.timer_running !== undefined) {
                timerRunning = data.timer_running;
                updateTimerButton();
            }
        }
        
        // 初始化强度按钮
        function initIntensityButtons() {
            const grid = document.getElementById('intensityGrid');
            for (let i = 1; i <= 30; i++) {
                const btn = document.createElement('button');
                btn.className = 'intensity-btn';
                btn.textContent = i;
                btn.onclick = () => {
                    sendData({intensity: i});
                };
                if (i === 1) btn.classList.add('active');
                grid.appendChild(btn);
            }
        }
        
        // 更新强度按钮状态
        function updateIntensityButtons(intensity) {
            document.querySelectorAll('.intensity-btn').forEach((btn, index) => {
                if (parseInt(btn.textContent) === intensity) {
                    btn.classList.add('active');
                } else {
                    btn.classList.remove('active');
                }
            });
        }
        
        // 初始化模式按钮
        function initModeButtons() {
            const modes = [
                '舒适模式', '强化模式', '温和模式', '脉冲模式', '波浪模式',
                '按摩模式', '深度模式', '循环模式', '智能模式', '自定义'
            ];
            const grid = document.getElementById('modeGrid');
            
            modes.forEach((mode, index) => {
                const btn = document.createElement('button');
                btn.className = 'mode-btn';
                btn.textContent = mode;
                btn.onclick = () => {
                    sendData({mode: index + 1});
                };
                if (index === 0) btn.classList.add('active');
                grid.appendChild(btn);
            });
        }
        
        // 更新模式按钮状态
        function updateModeButtons(mode) {
            document.querySelectorAll('.mode-btn').forEach((btn, index) => {
                if (index + 1 === mode) {
                    btn.classList.add('active');
                } else {
                    btn.classList.remove('active');
                }
            });
        }
        
        // 滑杆事件
        document.getElementById('intensitySlider').oninput = function() {
            document.getElementById('sliderValue').textContent = this.value + '%';
            sendData({slider_value: parseInt(this.value)});
        };
        
        // 开关事件
        ['switch1', 'switch2', 'switch3', 'switch4'].forEach(switchName => {
            document.getElementById(switchName).onchange = function() {
                const item = document.getElementById(switchName + 'Item');
                if (this.checked) {
                    item.classList.add('active');
                } else {
                    item.classList.remove('active');
                }
                sendData({[switchName]: this.checked});
            };
        });
        
        // 计时器功能
        function setTimer(minutes) {
            currentTimer = minutes * 60;
            sendData({timer_seconds: currentTimer});
            updateTimerDisplay();
        }
        
        function toggleTimer() {
            timerRunning = !timerRunning;
            sendData({timer_running: timerRunning});
            
            if (timerRunning) {
                startTimerCountdown();
            } else {
                stopTimerCountdown();
            }
            updateTimerButton();
        }
        
        function resetTimer() {
            timerRunning = false;
            currentTimer = 0;
            stopTimerCountdown();
            sendData({timer_running: false, timer_seconds: 0});
            updateTimerDisplay();
            updateTimerButton();
        }
        
        function startTimerCountdown() {
            timerInterval = setInterval(() => {
                if (currentTimer > 0) {
                    currentTimer--;
                    updateTimerDisplay();
                    sendData({timer_seconds: currentTimer});
                } else {
                    // 计时结束
                    timerRunning = false;
                    stopTimerCountdown();
                    updateTimerButton();
                    sendData({timer_running: false});
                    alert('治疗完成！');
                }
            }, 1000);
        }
        
        function stopTimerCountdown() {
            if (timerInterval) {
                clearInterval(timerInterval);
                timerInterval = null;
            }
        }
        
        function updateTimerDisplay() {
            const minutes = Math.floor(currentTimer / 60);
            const seconds = currentTimer % 60;
            document.getElementById('timerDisplay').textContent = 
                String(minutes).padStart(2, '0') + ':' + String(seconds).padStart(2, '0');
        }
        
        function updateTimerButton() {
            const btn = document.getElementById('startStopBtn');
            btn.textContent = timerRunning ? '暂停' : '开始';
            btn.className = timerRunning ? 'btn btn-danger' : 'btn btn-success';
        }
        
        // 页面加载完成后初始化
        document.addEventListener('DOMContentLoaded', function() {
            initIntensityButtons();
            initModeButtons();
            connectWebSocket();
        });
    </script>
</body>
</html>
)rawliteral";

// WiFi事件处理
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" joined, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" left, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

// WiFi初始化
void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = WIFI_CHANNEL,
            .password = WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                    .required = true,
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP started. SSID:%s password:%s channel:%d",
             WIFI_SSID, WIFI_PASS, WIFI_CHANNEL);
}

// 发送状态到WebSocket客户端
void send_status_to_websocket(void)
{
    if (websocket_fd == -1) return;

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "intensity", device_status.intensity);
    cJSON_AddBoolToObject(json, "switch1", device_status.switch1);
    cJSON_AddBoolToObject(json, "switch2", device_status.switch2);
    cJSON_AddBoolToObject(json, "switch3", device_status.switch3);
    cJSON_AddBoolToObject(json, "switch4", device_status.switch4);
    cJSON_AddNumberToObject(json, "mode", device_status.mode);
    cJSON_AddNumberToObject(json, "slider_value", device_status.slider_value);
    cJSON_AddNumberToObject(json, "timer_seconds", device_status.timer_seconds);
    cJSON_AddBoolToObject(json, "timer_running", device_status.timer_running);

    char *json_string = cJSON_Print(json);
    if (json_string) {
        // 使用简单的发送方式
        int sent = send(websocket_fd, json_string, strlen(json_string), 0);
        if (sent < 0) {
            ESP_LOGE(TAG, "Failed to send data to websocket");
        }
        free(json_string);
    }
    cJSON_Delete(json);
}

// 处理WebSocket消息
void handle_websocket_message(char* data)
{
    cJSON *json = cJSON_Parse(data);
    if (json == NULL) {
        ESP_LOGE(TAG, "Invalid JSON received");
        return;
    }

    bool status_changed = false;

    cJSON *intensity = cJSON_GetObjectItem(json, "intensity");
    if (intensity && cJSON_IsNumber(intensity)) {
        uint8_t new_intensity = (uint8_t)intensity->valueint;
        if (new_intensity >= 1 && new_intensity <= 30) {
            device_status.intensity = new_intensity;
            status_changed = true;
            ESP_LOGI(TAG, "Intensity set to: %d", new_intensity);
        }
    }

    cJSON *switch1 = cJSON_GetObjectItem(json, "switch1");
    if (switch1 && cJSON_IsBool(switch1)) {
        device_status.switch1 = cJSON_IsTrue(switch1);
        status_changed = true;
        ESP_LOGI(TAG, "Switch1: %s", device_status.switch1 ? "ON" : "OFF");
    }

    cJSON *switch2 = cJSON_GetObjectItem(json, "switch2");
    if (switch2 && cJSON_IsBool(switch2)) {
        device_status.switch2 = cJSON_IsTrue(switch2);
        status_changed = true;
        ESP_LOGI(TAG, "Switch2: %s", device_status.switch2 ? "ON" : "OFF");
    }

    cJSON *switch3 = cJSON_GetObjectItem(json, "switch3");
    if (switch3 && cJSON_IsBool(switch3)) {
        device_status.switch3 = cJSON_IsTrue(switch3);
        status_changed = true;
        ESP_LOGI(TAG, "Switch3: %s", device_status.switch3 ? "ON" : "OFF");
    }

    cJSON *switch4 = cJSON_GetObjectItem(json, "switch4");
    if (switch4 && cJSON_IsBool(switch4)) {
        device_status.switch4 = cJSON_IsTrue(switch4);
        status_changed = true;
        ESP_LOGI(TAG, "Switch4: %s", device_status.switch4 ? "ON" : "OFF");
    }

    cJSON *mode = cJSON_GetObjectItem(json, "mode");
    if (mode && cJSON_IsNumber(mode)) {
        uint8_t new_mode = (uint8_t)mode->valueint;
        if (new_mode >= 1 && new_mode <= 10) {
            device_status.mode = new_mode;
            status_changed = true;
            ESP_LOGI(TAG, "Mode set to: %d", new_mode);
        }
    }

    cJSON *slider = cJSON_GetObjectItem(json, "slider_value");
    if (slider && cJSON_IsNumber(slider)) {
        uint8_t new_slider = (uint8_t)slider->valueint;
        if (new_slider <= 100) {
            device_status.slider_value = new_slider;
            status_changed = true;
            ESP_LOGI(TAG, "Slider value set to: %d", new_slider);
        }
    }

    cJSON *timer_seconds = cJSON_GetObjectItem(json, "timer_seconds");
    if (timer_seconds && cJSON_IsNumber(timer_seconds)) {
        device_status.timer_seconds = (uint16_t)timer_seconds->valueint;
        status_changed = true;
        ESP_LOGI(TAG, "Timer set to: %d seconds", device_status.timer_seconds);
    }

    cJSON *timer_running = cJSON_GetObjectItem(json, "timer_running");
    if (timer_running && cJSON_IsBool(timer_running)) {
        device_status.timer_running = cJSON_IsTrue(timer_running);
        status_changed = true;
        ESP_LOGI(TAG, "Timer: %s", device_status.timer_running ? "RUNNING" : "STOPPED");
    }

    cJSON_Delete(json);

    // 如果状态有变化，执行相应的硬件控制
    if (status_changed) {
        // TODO: 在这里添加实际的硬件控制代码
        // 例如：控制PWM输出、GPIO开关等
        // control_hardware();
        
        // 发送更新后的状态给所有客户端
        send_status_to_websocket();
    }
}

// 硬件控制函数（根据实际硬件进行实现）
static esp_err_t control_handler(httpd_req_t *req)
{
    if (req->method == HTTP_POST) {
        char buf[1024];
        int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
        if (ret > 0) {
            buf[ret] = '\0';
            handle_websocket_message(buf);
        }
    }
    
    // 返回当前状态
    return status_handler(req);
}

// WebSocket处理函数
static esp_err_t websocket_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "WebSocket connection established");
        websocket_fd = httpd_req_to_sockfd(req);
        
        // 发送当前状态给新连接的客户端
        send_status_to_websocket();
        return ESP_OK;
    }

    // 接收数据
    char buf[1024];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret > 0) {
        buf[ret] = '\0';
        ESP_LOGI(TAG, "Received message: %s", buf);
        handle_websocket_message(buf);
        
        // 发送响应
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, "{\"status\":\"ok\"}", -1);
    }

    return ESP_OK;
}

// 主页处理函数
static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store, must-revalidate");
    httpd_resp_set_hdr(req, "Pragma", "no-cache");
    httpd_resp_set_hdr(req, "Expires", "0");
    return httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
}

// API状态处理函数
static esp_err_t status_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "intensity", device_status.intensity);
    cJSON_AddBoolToObject(json, "switch1", device_status.switch1);
    cJSON_AddBoolToObject(json, "switch2", device_status.switch2);
    cJSON_AddBoolToObject(json, "switch3", device_status.switch3);
    cJSON_AddBoolToObject(json, "switch4", device_status.switch4);
    cJSON_AddNumberToObject(json, "mode", device_status.mode);
    cJSON_AddNumberToObject(json, "slider_value", device_status.slider_value);
    cJSON_AddNumberToObject(json, "timer_seconds", device_status.timer_seconds);
    cJSON_AddBoolToObject(json, "timer_running", device_status.timer_running);

    char *json_string = cJSON_Print(json);
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);
    
    free(json_string);
    cJSON_Delete(json);
    return ESP_OK;
}

static esp_err_t poll_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    
    // 返回当前状态
    return status_handler(req);
}

// HTTP服务器启动
static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        
        // 主页
        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &index_uri);

        // WebSocket端点
        httpd_uri_t websocket_uri = {
            .uri        = "/ws",
            .method     = HTTP_GET,
            .handler    = websocket_handler,
            .user_ctx   = NULL
        };
        httpd_register_uri_handler(server, &websocket_uri);

        // API状态
        httpd_uri_t status_uri = {
            .uri       = "/api/status",
            .method    = HTTP_GET,
            .handler   = status_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &status_uri);

        // 轮询端点
        httpd_uri_t poll_uri = {
            .uri       = "/api/poll",
            .method    = HTTP_GET,
            .handler   = poll_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &poll_uri);

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

// 定时器任务 - 处理设备计时器
void timer_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1秒

    while (1) {
        if (device_status.timer_running && device_status.timer_seconds > 0) {
            device_status.timer_seconds--;
            
            // 发送更新到WebSocket客户端
            send_status_to_websocket();
            
            if (device_status.timer_seconds == 0) {
                device_status.timer_running = false;
                ESP_LOGI(TAG, "Timer finished!");
                
                // TODO: 添加治疗完成的处理逻辑
                // 例如：停止所有输出，发送提醒等
            }
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void FRT_UI_TaskCreate(void)
{
    ESP_LOGI(TAG, "灵络痛经治疗仪启动中...");

    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化WiFi AP模式
    wifi_init_softap();

    // 启动Web服务器
    server = start_webserver();
    if (server == NULL) {
        ESP_LOGE(TAG, "Failed to start webserver");
        return;
    }

    // 创建定时器任务
    xTaskCreate(timer_task, "timer_task", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "灵络治疗仪已启动完成!");
    ESP_LOGI(TAG, "请连接WiFi: %s", WIFI_SSID);
    ESP_LOGI(TAG, "密码: %s", WIFI_PASS);
    ESP_LOGI(TAG, "访问地址: http://192.168.4.1");

    // TODO: 在这里添加硬件初始化代码
    // - GPIO配置
    // - PWM初始化
    // - ADC/DAC初始化
    // - SPI/I2C初始化等
}