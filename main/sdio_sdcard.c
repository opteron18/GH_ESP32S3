/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SDMMC peripheral to communicate with SD card.

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sd_test_io.h"
#include "global_config.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include <math.h>

#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

#define EXAMPLE_MAX_CHAR_SIZE    64

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"
#define EXAMPLE_IS_UHS1    (CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_SDR50 || CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_DDR50)

#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
const char* names[] = {"CLK", "CMD", "D0", "D1", "D2", "D3"};
const int pins[] = {CONFIG_EXAMPLE_PIN_CLK,
                    CONFIG_EXAMPLE_PIN_CMD,
                    CONFIG_EXAMPLE_PIN_D0
                    #ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
                    ,CONFIG_EXAMPLE_PIN_D1,
                    CONFIG_EXAMPLE_PIN_D2,
                    CONFIG_EXAMPLE_PIN_D3
                    #endif
                    };

const int pin_count = sizeof(pins)/sizeof(pins[0]);

#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
const int adc_channels[] = {CONFIG_EXAMPLE_ADC_PIN_CLK,
                            CONFIG_EXAMPLE_ADC_PIN_CMD,
                            CONFIG_EXAMPLE_ADC_PIN_D0
                            #ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
                            ,CONFIG_EXAMPLE_ADC_PIN_D1,
                            CONFIG_EXAMPLE_ADC_PIN_D2,
                            CONFIG_EXAMPLE_ADC_PIN_D3
                            #endif
                            };
#endif //CONFIG_EXAMPLE_ENABLE_ADC_FEATURE

pin_configuration_t config = {
    .names = names,
    .pins = pins,
#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
    .adc_channels = adc_channels,
#endif
};
#endif //CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS

static esp_err_t s_example_write_file(const char *path, char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    return ESP_OK;
}

static esp_err_t s_example_read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[EXAMPLE_MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    return ESP_OK;
}

// 检查SD卡状态
bool check_sd_card_status(const char* mount_point)
{
    // 尝试创建一个简单的测试文件
    const char* test_file = "/sdcard/status_test.tmp";
    FILE* f = fopen(test_file, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "SD card status check failed: cannot create test file");
        return false;
    }
    
    const char* test_data = "test";
    size_t written = fwrite(test_data, 1, 4, f);
    fclose(f);
    
    if (written != 4) {
        ESP_LOGE(TAG, "SD card status check failed: write test failed");
        unlink(test_file);
        return false;
    }
    
    // 尝试读取文件
    f = fopen(test_file, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "SD card status check failed: cannot read test file");
        unlink(test_file);
        return false;
    }
    
    char read_data[8];
    size_t read_bytes = fread(read_data, 1, 4, f);
    fclose(f);
    unlink(test_file);
    
    if (read_bytes != 4 || strncmp(read_data, test_data, 4) != 0) {
        ESP_LOGE(TAG, "SD card status check failed: read test failed");
        return false;
    }
    
    ESP_LOGI(TAG, "SD card status check passed");
    return true;
}

// SD卡测速函数
void sdcard_speed_test(const char* mount_point)
{
    ESP_LOGI(TAG, "Starting SD card speed test...");
    
    // 显示可用内存信息
    ESP_LOGI(TAG, "Free heap: %ld bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum free heap: %ld bytes", esp_get_minimum_free_heap_size());
    
    // 检查SD卡状态
    if (!check_sd_card_status(mount_point)) {
        ESP_LOGE(TAG, "SD card status check failed, aborting speed test");
        ESP_LOGI(TAG, "This might be due to SD card being busy or filesystem issues");
        return;
    }
    
    // 添加短暂延迟，确保SD卡稳定
    vTaskDelay(pdMS_TO_TICKS(100));
    
    const char* test_filename = "/sdcard/speed_test.bin";
    const size_t test_sizes[] = {1024, 10240, 102400}; // 1KB, 10KB, 100KB, 500KB
    
    ESP_LOGI(TAG, "Starting SD card speed test...");
    ESP_LOGI(TAG, "Free heap: %lu bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum free heap: %lu bytes", esp_get_minimum_free_heap_size());
    
    // 尝试分配测试数据缓冲区
    const size_t max_test_size = 102400; // 1MB
    uint8_t* test_data = malloc(max_test_size);
    size_t buffer_size = max_test_size;
    
    if (test_data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate test data buffer, trying smaller size...");
        // 尝试分配更小的缓冲区
        buffer_size = 4096; // 100KB
        test_data = malloc(buffer_size);
        if (test_data == NULL) {
            ESP_LOGE(TAG, "Failed to allocate even 100KB buffer, aborting speed test");
            return;
        }
        ESP_LOGW(TAG, "Using reduced buffer size: 100KB");
    }
    
    // 验证test_data不为NULL
    if (test_data == NULL) {
        ESP_LOGE(TAG, "Critical error: test_data is NULL after allocation");
        return;
    }
    
    ESP_LOGI(TAG, "Successfully allocated %zu bytes for test buffer", buffer_size);
    
    // 填充测试数据
    for (size_t i = 0; i < buffer_size; i++) {
        test_data[i] = (uint8_t)(i & 0xFF);
        // 每1000次循环喂一次狗，避免在数据填充时触发看门狗
        if (i % 1000 == 0) {
            // esp_task_wdt_reset();
            vTaskDelay(pdMS_TO_TICKS(1)); // 让出CPU时间
        }
    }
    
    // // 扩展看门狗超时时间并禁用当前任务的看门狗
    // esp_task_wdt_config_t wdt_config = {
    //     .timeout_ms = 60000, // 60秒超时
    //     .idle_core_mask = 0,
    //     .trigger_panic = false
    // };
    
    // esp_err_t wdt_init_err = esp_task_wdt_init(&wdt_config);
    // if (wdt_init_err == ESP_OK) {
    //     ESP_LOGI(TAG, "Extended task watchdog timeout to 60 seconds");
    // }
    
    // esp_err_t wdt_err = esp_task_wdt_delete(NULL);
    // if (wdt_err == ESP_OK) {
    //     ESP_LOGI(TAG, "Task watchdog disabled for speed test");
    // } else {
    //     ESP_LOGW(TAG, "Could not disable task watchdog: %s", esp_err_to_name(wdt_err));
    // }
    
    for (int i = 0; i < 3; i++) {
        size_t test_size = test_sizes[i];
        
        // 检查测试大小是否超过缓冲区大小
        if (test_size > buffer_size) {
            ESP_LOGW(TAG, "Skipping test with %zu bytes (buffer size: %zu)", test_size, buffer_size);
            continue;
        }
        
        ESP_LOGI(TAG, "Testing with %zu bytes...", test_size);
        
        // 简单的空间检查 - 尝试创建测试文件
        ESP_LOGI(TAG, "Preparing test file...");
        
        // 预先删除可能存在的测试文件
        unlink(test_filename);
        
        // 写入测试
        int64_t start_time = esp_timer_get_time();
        
        // 使用POSIX接口打开文件
        int fd = open(test_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            ESP_LOGE(TAG, "Failed to open file for writing: %s", strerror(errno));
            continue;
        }
        
        ESP_LOGI(TAG, "File opened successfully, fd=%d", fd);
        
        // 分块写入，避免长时间阻塞
        size_t chunk_size = 1024; // 减小到1KB chunks，降低单次写入时间
        size_t written_total = 0;
        bool write_success = true;
        
        ESP_LOGI(TAG, "Starting write operation with %zu byte chunks...", chunk_size);
        
        for (size_t offset = 0; offset < test_size && write_success; offset += chunk_size) {
            size_t this_chunk = (offset + chunk_size > test_size) ? (test_size - offset) : chunk_size;
            
            // 确保不会访问越界
            if (offset + this_chunk > buffer_size) {
                ESP_LOGE(TAG, "Buffer overflow prevention: offset %zu + chunk %zu > buffer %zu", 
                         offset, this_chunk, buffer_size);
                write_success = false;
                break;
            }
            
            // 添加写入前的状态检查
            ESP_LOGD(TAG, "Writing chunk at offset %zu, size %zu", offset, this_chunk);
            
            // 使用POSIX write替代fwrite
            errno = 0; // 清除之前的错误
            ssize_t written = write(fd, test_data + offset, this_chunk);
            
            // 检查写入结果
            if (written != (ssize_t)this_chunk) {
                ESP_LOGE(TAG, "Write failed at offset %zu: expected %zu, got %zd, errno: %s", 
                         offset, this_chunk, written, strerror(errno));
                write_success = false;
                break;
            }
            
            written_total += written;
            
            // 每写入几个chunk就强制同步
            if ((offset / chunk_size) % 10 == 0) {
                ESP_LOGD(TAG, "Intermediate sync at offset %zu", offset);
                if (fsync(fd) != 0) {
                    ESP_LOGW(TAG, "Intermediate sync failed: %s", strerror(errno));
                }
            }
            
            // 更频繁地让出CPU时间，并重置看门狗
            // esp_task_wdt_reset();
            // vTaskDelay(pdMS_TO_TICKS(2)); // 增加延迟时间
            
            // 每写入一定数量的数据就报告进度
            if ((offset / chunk_size) % 50 == 0) {
                ESP_LOGI(TAG, "Write progress: %zu/%zu bytes (%.1f%%)", 
                         written_total, test_size, (float)written_total * 100.0f / test_size);
            }
        }
        
        if (!write_success || written_total != test_size) {
            ESP_LOGE(TAG, "Total write failed: expected %zu, got %zu", test_size, written_total);
            close(fd);
            continue;
        }
        
        // 最终同步到存储设备
        ESP_LOGI(TAG, "Final sync...");
        if (fsync(fd) != 0) {
            ESP_LOGE(TAG, "Final sync failed: %s", strerror(errno));
            close(fd);
            continue;
        }
        
        close(fd);
        int64_t write_time = esp_timer_get_time();
        
        ESP_LOGI(TAG, "Write completed, starting read test...");
        
        // 读取测试 - 使用POSIX接口
        int64_t read_start_time = esp_timer_get_time();
        
        // 使用POSIX接口打开文件进行读取
        fd = open(test_filename, O_RDONLY);
        if (fd < 0) {
            ESP_LOGE(TAG, "Failed to open file for reading: %s", strerror(errno));
            continue;
        }
        
        ESP_LOGI(TAG, "File opened for reading, fd=%d", fd);
        
        uint8_t* read_data = malloc(test_size);
        if (read_data == NULL) {
            close(fd);
            ESP_LOGE(TAG, "Failed to allocate read buffer");
            continue;
        }
        
        // 分块读取
        size_t read_total = 0;
        bool read_success = true;
        
        ESP_LOGI(TAG, "Starting read operation with %zu byte chunks...", chunk_size);
        
        for (size_t offset = 0; offset < test_size && read_success; offset += chunk_size) {
            size_t this_chunk = (offset + chunk_size > test_size) ? (test_size - offset) : chunk_size;
            
            ESP_LOGD(TAG, "Reading chunk at offset %zu, size %zu", offset, this_chunk);
            
            // 使用POSIX read替代fread
            errno = 0;
            ssize_t read_bytes = read(fd, read_data + offset, this_chunk);
            
            if (read_bytes != (ssize_t)this_chunk) {
                ESP_LOGE(TAG, "Read failed at offset %zu: expected %zu, got %zd, errno: %s", 
                         offset, this_chunk, read_bytes, strerror(errno));
                read_success = false;
                break;
            }
            read_total += read_bytes;
            
            // 更频繁地让出CPU时间，并重置看门狗
            // esp_task_wdt_reset();
            // vTaskDelay(pdMS_TO_TICKS(2));
            
            // 每读取一定数量的数据就报告进度
            if ((offset / chunk_size) % 50 == 0) {
                ESP_LOGI(TAG, "Read progress: %zu/%zu bytes (%.1f%%)", 
                         read_total, test_size, (float)read_total * 100.0f / test_size);
            }
        }
        
        close(fd);
        int64_t read_time = esp_timer_get_time();
        
        if (!read_success || read_total != test_size) {
            ESP_LOGE(TAG, "Total read failed: expected %zu, got %zu", test_size, read_total);
            free(read_data);
            continue;
        }
        
        ESP_LOGI(TAG, "Read completed, verifying data...");
        
        // 验证数据（分块进行，避免长时间阻塞）
        bool data_valid = true;
        for (size_t j = 0; j < test_size && data_valid; j += 1000) {
            size_t check_end = (j + 1000 > test_size) ? test_size : j + 1000;
            for (size_t k = j; k < check_end; k++) {
                if (read_data[k] != test_data[k]) {
                    ESP_LOGE(TAG, "Data mismatch at byte %zu: expected 0x%02X, got 0x%02X", 
                             k, test_data[k], read_data[k]);
                    data_valid = false;
                    break;
                }
            }
            // 每验证1000字节让出CPU时间
            // vTaskDelay(pdMS_TO_TICKS(1));
        }
        
        free(read_data);
        
        if (!data_valid) {
            ESP_LOGE(TAG, "Data verification failed");
            continue;
        }
        
        // 计算速度
        double write_time_sec = (write_time - start_time) / 1000000.0;
        double read_time_sec = (read_time - read_start_time) / 1000000.0;
        double write_speed = (double)test_size / (1024 * 1024) / write_time_sec; // MB/s
        double read_speed = (double)test_size / (1024 * 1024) / read_time_sec;   // MB/s
        
        ESP_LOGI(TAG, "=== Test Results ===");
        ESP_LOGI(TAG, "File size: %zu bytes (%.2f KB)", test_size, (double)test_size / 1024);
        ESP_LOGI(TAG, "Write speed: %.2f MB/s", write_speed);
        ESP_LOGI(TAG, "Read speed: %.2f MB/s", read_speed);
        ESP_LOGI(TAG, "Write time: %.2f ms", write_time_sec * 1000);
        ESP_LOGI(TAG, "Read time: %.2f ms", read_time_sec * 1000);
        ESP_LOGI(TAG, "---");
        
        // 测试之间稍微延迟
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // 测试之间稍微延迟
    vTaskDelay(pdMS_TO_TICKS(100));
    
    
    // if (test_data) {
    // free(test_data);
    // test_data = NULL;
    // }

    ESP_LOGI(TAG, "Speed test completed");

    // 连续写入测试
    ESP_LOGI(TAG, "Starting continuous write test...");
    const char* continuous_filename = "/sdcard/continuous_test.bin";
    size_t continuous_size = 102400; // 100KB
    const int continuous_count = 10;
    
    // 确保连续测试大小不超过缓冲区
    if (continuous_size > buffer_size) {
        continuous_size = buffer_size;
        ESP_LOGW(TAG, "Reducing continuous test size to %zu bytes", continuous_size);
    }
    
    int64_t total_start = esp_timer_get_time();
    int successful_writes = 0;
    for (int i = 0; i < continuous_count; i++) {
        FILE* f = fopen(continuous_filename, "wb");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for continuous write: %s", strerror(errno));
            break;
        }
        
        size_t written = fwrite(test_data, 1, continuous_size, f);
        int flush_result = fflush(f);
        fclose(f);
        
        if (written != continuous_size || flush_result != 0) {
            ESP_LOGE(TAG, "Continuous write failed at iteration %d: written=%zu, flush=%d", i, written, flush_result);
            break;
        }
        
        successful_writes++;
    }
    int64_t total_end = esp_timer_get_time();
    
    if (successful_writes == 0) {
        ESP_LOGE(TAG, "All continuous writes failed, skipping speed calculation");
        free(test_data);
        return;
    }
    
    double total_speed = (double)(continuous_size * successful_writes) / ((total_end - total_start) / 1000000.0) / 1024.0 / 1024.0;
    ESP_LOGI(TAG, "Continuous write test: %d/%d files of %zu bytes each", successful_writes, continuous_count, continuous_size);
    ESP_LOGI(TAG, "Total speed: %.2f MB/s", total_speed);
    ESP_LOGI(TAG, "Average per file: %.2f ms", (total_end - total_start) / 1000.0 / successful_writes);
    
    // 随机访问测试
    ESP_LOGI(TAG, "Starting random access test...");
    const char* random_filename = "/sdcard/random_test.bin";
    size_t random_file_size = 102400; // 1MB
    const int random_access_count = 100;
    
    // 确保随机测试文件大小不超过缓冲区
    if (random_file_size > buffer_size) {
        random_file_size = buffer_size;
        ESP_LOGW(TAG, "Reducing random test file size to %zu bytes", random_file_size);
    }
    
    // 创建大文件
    FILE* f = fopen(random_filename, "wb");
    if (f != NULL) {
        fwrite(test_data, 1, random_file_size, f);
        fclose(f);
    }
    
    // 随机读取测试
    int64_t start_time = esp_timer_get_time();
    for (int i = 0; i < random_access_count; i++) {
        size_t offset = (rand() % (random_file_size - 1024));
        size_t read_size = 1024;
        
        f = fopen(random_filename, "rb");
        if (f == NULL) continue;
        
        fseek(f, offset, SEEK_SET);
        uint8_t buffer[1024];
        fread(buffer, 1, read_size, f);
        fclose(f);
    }
    int64_t random_end = esp_timer_get_time();
    
    double random_speed = (double)(1024 * random_access_count) / ((random_end - start_time) / 1000000.0) / 1024.0 / 1024.0;
    ESP_LOGI(TAG, "Random access test: %d reads of 1KB each", random_access_count);
    ESP_LOGI(TAG, "Random access speed: %.2f MB/s", random_speed);
    
    // 清理测试文件
    unlink(test_filename);
    unlink(continuous_filename);
    unlink(random_filename);
    
    free(test_data);
    ESP_LOGI(TAG, "SD card speed test completed!");
}

// 高级性能分析函数
void sdcard_advanced_performance_test(const char* mount_point)
{
    ESP_LOGI(TAG, "Starting advanced SD card performance analysis...");
    
    const char* test_filename = "/sdcard/advanced_test.bin";
    const size_t test_size = 102400; // 100KB
    const int iterations = 50;
    
    // 准备测试数据
    uint8_t* test_data = malloc(test_size);
    if (test_data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate test data buffer for advanced test");
        return;
    }
    
    for (size_t i = 0; i < test_size; i++) {
        test_data[i] = (uint8_t)(i & 0xFF);
    }
    
    // 写入延迟分析
    ESP_LOGI(TAG, "Analyzing write latency...");
    int64_t write_times[iterations];
    int64_t min_write = INT64_MAX, max_write = 0, total_write = 0;
    
    for (int i = 0; i < iterations; i++) {
        int64_t start = esp_timer_get_time();
        FILE* f = fopen(test_filename, "wb");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for write test");
            break;
        }
        
        size_t written = fwrite(test_data, 1, test_size, f);
        fclose(f);
        int64_t end = esp_timer_get_time();
        
        if (written != test_size) {
            ESP_LOGE(TAG, "Write failed at iteration %d", i);
            break;
        }
        
        write_times[i] = end - start;
        if (write_times[i] < min_write) min_write = write_times[i];
        if (write_times[i] > max_write) max_write = write_times[i];
        total_write += write_times[i];
        
        // 删除文件以便下次测试
        unlink(test_filename);
    }
    
    double avg_write_time = (double)total_write / iterations / 1000.0; // ms
    double min_write_time = (double)min_write / 1000.0; // ms
    double max_write_time = (double)max_write / 1000.0; // ms
    double write_iops = 1000.0 / avg_write_time; // IOPS
    
    ESP_LOGI(TAG, "Write Performance Analysis:");
    ESP_LOGI(TAG, "  Average time: %.2f ms", avg_write_time);
    ESP_LOGI(TAG, "  Min time: %.2f ms", min_write_time);
    ESP_LOGI(TAG, "  Max time: %.2f ms", max_write_time);
    ESP_LOGI(TAG, "  Write IOPS: %.2f", write_iops);
    
    // 读取延迟分析
    ESP_LOGI(TAG, "Analyzing read latency...");
    
    // 先创建一个文件用于读取测试
    FILE* f = fopen(test_filename, "wb");
    if (f != NULL) {
        fwrite(test_data, 1, test_size, f);
        fclose(f);
    }
    
    int64_t read_times[iterations];
    int64_t min_read = INT64_MAX, max_read = 0, total_read = 0;
    
    for (int i = 0; i < iterations; i++) {
        int64_t start = esp_timer_get_time();
        f = fopen(test_filename, "rb");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for read test");
            break;
        }
        
        uint8_t* read_data = malloc(test_size);
        if (read_data == NULL) {
            fclose(f);
            ESP_LOGE(TAG, "Failed to allocate read buffer");
            break;
        }
        
        size_t read_bytes = fread(read_data, 1, test_size, f);
        fclose(f);
        int64_t end = esp_timer_get_time();
        
        if (read_bytes != test_size) {
            ESP_LOGE(TAG, "Read failed at iteration %d", i);
            free(read_data);
            break;
        }
        
        free(read_data);
        
        read_times[i] = end - start;
        if (read_times[i] < min_read) min_read = read_times[i];
        if (read_times[i] > max_read) max_read = read_times[i];
        total_read += read_times[i];
    }
    
    double avg_read_time = (double)total_read / iterations / 1000.0; // ms
    double min_read_time = (double)min_read / 1000.0; // ms
    double max_read_time = (double)max_read / 1000.0; // ms
    double read_iops = 1000.0 / avg_read_time; // IOPS
    
    ESP_LOGI(TAG, "Read Performance Analysis:");
    ESP_LOGI(TAG, "  Average time: %.2f ms", avg_read_time);
    ESP_LOGI(TAG, "  Min time: %.2f ms", min_read_time);
    ESP_LOGI(TAG, "  Max time: %.2f ms", max_read_time);
    ESP_LOGI(TAG, "  Read IOPS: %.2f", read_iops);
    
    // 小文件性能测试
    ESP_LOGI(TAG, "Testing small file performance...");
    const size_t small_file_size = 1024; // 1KB
    const int small_file_count = 100;
    
    int64_t small_start = esp_timer_get_time();
    for (int i = 0; i < small_file_count; i++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "/sdcard/small_%d.bin", i);
        
        f = fopen(filename, "wb");
        if (f != NULL) {
            fwrite(test_data, 1, small_file_size, f);
            fclose(f);
        }
    }
    int64_t small_end = esp_timer_get_time();
    
    double small_file_time = (double)(small_end - small_start) / 1000.0; // ms
    double small_file_iops = (double)small_file_count / (small_file_time / 1000.0);
    
    ESP_LOGI(TAG, "Small File Performance:");
    ESP_LOGI(TAG, "  Created %d files of %zu bytes each", small_file_count, small_file_size);
    ESP_LOGI(TAG, "  Total time: %.2f ms", small_file_time);
    ESP_LOGI(TAG, "  Average per file: %.2f ms", small_file_time / small_file_count);
    ESP_LOGI(TAG, "  Small file IOPS: %.2f", small_file_iops);
    
    // 清理测试文件
    for (int i = 0; i < small_file_count; i++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "/sdcard/small_%d.bin", i);
        unlink(filename);
    }
    unlink(test_filename);
    
    free(test_data);
    ESP_LOGI(TAG, "Advanced performance analysis completed!");
}


#define NUM_CHANNELS 12
#define SAMPLE_RATE_HZ 1000
#define RECORD_DURATION_SEC 10   // 采样时间 10 秒

typedef struct __attribute__((packed)) {
    uint32_t timestamp_us;     // 时间戳（微秒）
    uint16_t adc[NUM_CHANNELS]; // 12 路 ADC
} SampleFrame;

static FILE *data_file = NULL;

// 模拟 ADC 采样函数
static uint16_t read_adc_channel(int ch, uint32_t t_us)
{
    // 模拟信号：不同通道不同频率的正弦波 + 随机噪声
    float t = t_us / 1e6f;
    float value = (sinf(2 * M_PI * (1.0f + ch * 0.1f) * t) + 1.0f) * 8192 + (rand() % 100);
    if (value > 16383) value = 16383;
    if (value < 0) value = 0;
    return (uint16_t)value;
}

void record_adc_task(void *arg)
{

    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {

        .format_if_mount_failed = true,
        // .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    ESP_LOGI(TAG, "Using SDMMC peripheral");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    host.max_freq_khz = 10000;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    slot_config.width = 4;

    slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
    slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
    slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;

    slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
    slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
    slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;

    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
            check_sd_card_pins(&config, pin_count);
#endif
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);


     // 打开文件（写入模式）
    FILE *data_file = fopen("/sdcard/data.csv", "wb");
    if (!data_file) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        vTaskDelete(NULL);
        return;
    }

    // 写入 CSV 表头
    fprintf(data_file, "timestamp_us");
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        fprintf(data_file, ",adc%d", ch);
    }
    fprintf(data_file, "\n");

    // 预设缓冲区（可加快写入速度）
    static char file_buffer[8192];
    setvbuf(data_file, file_buffer, _IOFBF, sizeof(file_buffer));

    uint64_t start_time = esp_timer_get_time();
    uint32_t total_samples = SAMPLE_RATE_HZ * RECORD_DURATION_SEC * 120;
    
    int frame_counter = 0;
    for (uint32_t i = 0; i < total_samples; i++) {
        uint64_t timestamp_us = esp_timer_get_time();

        // 写入一行数据
        fprintf(data_file, "%llu", (unsigned long long)timestamp_us);
        for (int ch = 0; ch < NUM_CHANNELS; ch++) {
            int value = read_adc_channel(ch, timestamp_us);
            fprintf(data_file, ",%d", value);
        }
        frame_counter++;
        if(frame_counter == 1000){
            ESP_LOGI(TAG, "wrtie one second data");
            frame_counter = 0;
        }
        
        fprintf(data_file, "\n");

        // 控制采样速率（1kHz）
        vTaskDelay(pdMS_TO_TICKS(1000 / SAMPLE_RATE_HZ));
    }

    fflush(data_file);
    fclose(data_file);



    // ESP_LOGI(TAG, "Starting ADC recording...");

    // struct stat st;
    // if (stat("/sdcard", &st) != 0) {
    //     ESP_LOGE(TAG, "/sdcard not found!");
    //     vTaskDelete(NULL);
    //     return;
    // }

    // // 打开文件
    // data_file = fopen("/sdcard/data.bin", "wb");
    // if (!data_file) {
    //     ESP_LOGE(TAG, "Failed to open file for writing");
    //     vTaskDelete(NULL);
    //     return;
    // }

    // // 启用缓冲区写入
    // static uint8_t file_buffer[4096];
    // setvbuf(data_file, (char*)file_buffer, _IOFBF, sizeof(file_buffer));

    // SampleFrame frame;
    // uint64_t start_time = esp_timer_get_time();

    // uint32_t total_samples = SAMPLE_RATE_HZ * RECORD_DURATION_SEC;

    // for (uint32_t i = 0; i < total_samples; i++) {
    //     frame.timestamp_us = (uint32_t)esp_timer_get_time();

    //     for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    //         frame.adc[ch] = read_adc_channel(ch, frame.timestamp_us);
    //     }

    //     fwrite(&frame, sizeof(frame), 1, data_file);

    //     // 控制采样间隔 (1ms)
    //     vTaskDelay(pdMS_TO_TICKS(1000 / SAMPLE_RATE_HZ));
    // }

    // fclose(data_file);
    // ESP_LOGI(TAG, "Recording complete. Data saved to /sdcard/data.bin");

    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    vTaskDelete(NULL);
}


void sdcard_test(void)
{

    xTaskCreate(record_adc_task, "record_adc_task", 4096, NULL, 5, NULL);

    // Use POSIX and C standard library functions to work with files:

    // First create a file.
    // const char *file_hello = MOUNT_POINT"/hello.txt";
    // char data[EXAMPLE_MAX_CHAR_SIZE];
    // snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
    // ret = s_example_write_file(file_hello, data);
    // if (ret != ESP_OK) {
    //     return;
    // }

    // const char *file_foo = MOUNT_POINT"/foo.txt";
    // // Check if destination file exists before renaming
    // struct stat st;
    // if (stat(file_foo, &st) == 0) {
    //     // Delete it if it exists
    //     unlink(file_foo);
    // }

    // Rename original file
    // ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
    // if (rename(file_hello, file_foo) != 0) {
    //     ESP_LOGE(TAG, "Rename failed");
    //     return;
    // }

    // ret = s_example_read_file(file_foo);
    // if (ret != ESP_OK) {
    //     return;
    // }

    // Format FATFS
// #ifdef CONFIG_EXAMPLE_FORMAT_SD_CARD
    // ret = esp_vfs_fat_sdcard_format(mount_point, card);
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
    //     return;
    // }

    // if (stat(file_foo, &st) == 0) {
    //     ESP_LOGI(TAG, "file still exists");
    //     return;
    // } else {
    //     ESP_LOGI(TAG, "file doesn't exist, formatting done");
    // }
// #endif // CONFIG_EXAMPLE_FORMAT_SD_CARD

    // const char *file_nihao = MOUNT_POINT"/nihao.txt";
    // memset(data, 0, EXAMPLE_MAX_CHAR_SIZE);
    // snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
    // ret = s_example_write_file(file_nihao, data);
    // if (ret != ESP_OK) {
    //     return;
    // }

    // //Open file for reading
    // ret = s_example_read_file(file_nihao);
    // if (ret != ESP_OK) {
    //     return;
    // }

    // 运行SD卡测速测试
    // sdcard_speed_test(mount_point);
    
    // 运行高级性能分析
    // sdcard_advanced_performance_test(mount_point);

    // All done, unmount partition and disable SDMMC peripheral
    // esp_vfs_fat_sdcard_unmount(mount_point, card);
    // ESP_LOGI(TAG, "Card unmounted");

    // Deinitialize the power control driver if it was used
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    ret = sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete the on-chip LDO power control driver");
        return;
    }
#endif
}
