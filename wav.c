#include <stdio.h>
#include <stdint.h>

int main() {
    const char* filename = "output.wav";
    int sampleRate = 44100;
    int duration = 3; // 秒
    int numChannels = 2;
    int bitsPerSample = 16;
    double frequency = 440.0; // 频率440Hz（A调）

    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("无法打开文件");
        return 1;
    }

    // 写入RIFF头
    fwrite("RIFF", 1, 4, file);
    uint32_t chunkSize = 0; // 稍后填充
    fwrite(&chunkSize, 1, 4, file);
    fwrite("WAVE", 1, 4, file);

    // 写入fmt子块
    fwrite("fmt ", 1, 4, file);
    uint32_t subchunk1Size = 16;
    fwrite(&subchunk1Size, 1, 4, file);
    uint16_t audioFormat = 1; // PCM格式
    fwrite(&audioFormat, 1, 2, file);
    fwrite(&numChannels, 1, 2, file);
    uint32_t sampleRateValue = sampleRate;
    fwrite(&sampleRateValue, 1, 4, file);
    uint32_t byteRate = sampleRate * numChannels * bitsPerSample / 8;
    fwrite(&byteRate, 1, 4, file);
    uint16_t blockAlign = numChannels * bitsPerSample / 8;
    fwrite(&blockAlign, 1, 2, file);
    fwrite(&bitsPerSample, 1, 2, file);

    // 写入data子块头
    fwrite("data", 1, 4, file);
    uint32_t subchunk2Size = 0; // 稍后填充
    fwrite(&subchunk2Size, 1, 4, file);

    long dataStart = ftell(file); // 记录数据起始位置

    // 生成正弦波音频数据
    int numSamples = sampleRate * duration;
    for (int i = 0; i < numSamples; ++i) {
        double t = (double)i / sampleRate;
        double value = sin(2.0 * 3.14159265358979323846 * frequency * t);
        int16_t sample = (int16_t)(value * 32767.0);
        fwrite(&sample, sizeof(int16_t), 1, file);
    }

    // 更新data子块大小
    long dataEnd = ftell(file);
    uint32_t dataSize = dataEnd - dataStart;
    fseek(file, dataStart - 4, SEEK_SET);
    fwrite(&dataSize, 1, 4, file);

    // 更新RIFF块大小
    uint32_t riffSize = dataEnd - 8;
    fseek(file, 4, SEEK_SET);
    fwrite(&riffSize, 1, 4, file);

    fclose(file);
    printf("WAV文件生成成功：%s\n", filename);
    return 0;
}