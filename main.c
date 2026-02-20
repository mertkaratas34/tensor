#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// Veri Tipleri
typedef enum {
    DT_FLOAT32,
    DT_FLOAT16,
    DT_INT8
} DataType;

// Union Yapısı
typedef union {//bellekte yer ayılır tensorun ıcındekı veriye göre ram den tasarruf edilmiş olur
    float* f32;
    uint16_t* f16;
    int8_t* i8;
    void* raw; // Bellek tahsisi ve temizliği
} TensorBuffer;

// Ana Tensor Yapısı
typedef struct {
    DataType type;// Etiketimiz (Örn: DT_INT8)
    uint32_t count;// Kaç tane sayı tutuyoruz?
    TensorBuffer data;// Verilerin kendisi (Union)
    float scale;// Kuantizasyon ölçeği (Küçültme oranımız)
    int8_t zero_point;// Sıfırın nereye denk geldiği
} Tensor;

// Değeri -128 ile 127 arasına sabitleyen yardımcı fonksiyon
int8_t clamp_int8(float value) {
    if (value > 127.0f) return 127;
    if (value < -128.0f) return -128;
    return (int8_t)value;
}

Tensor* tensor_olustur(uint32_t n, DataType type) {
    Tensor* t = (Tensor*)malloc(sizeof(Tensor));
    t->count = n;//eleman sayısı
    t->type = type;
    t->scale = 1.0f;
    t->zero_point = 0;

    size_t element_size; //veri tipine göre bellekte tutulacak yer
    if (type == DT_FLOAT32) element_size = sizeof(float);
    else if (type == DT_FLOAT16) element_size = sizeof(uint16_t);
    else element_size = sizeof(int8_t);

    // veriyi asıl tutacak yer n ile eleman sayısı çarpılarak bulunur
    t->data.raw = malloc(n * element_size);

    return t;
}

// Güvenli Quantization Uygulaması
void quantize_islem(float* src, Tensor* dst) {
    if (dst->type != DT_INT8) return;

    for (uint32_t i = 0; i < dst->count; i++) {
        // 1. Ölçekle ve sıfır noktasını ekle
        float raw_quantized = roundf(src[i] / dst->scale) + dst->zero_point;

        // 2. Taşkınlık olmaması için clamp yap ve ata
        dst->data.i8[i] = clamp_int8(raw_quantized);//clamp int8 int sınır kontrolu
    }
}

int main() {
    uint32_t n = 4;
    // Kasten büyük bir sayı ekledim (100.0f)
    float ham_veriler[] = {10.1f, 25.2f, 39.9f, 100.0f};

    // 8-bit bir Tensör
    Tensor* q_tensor = tensor_olustur(n, DT_INT8);//olcek 0.5 yapılıp tensor fonksiyonuna yollanır
    q_tensor->scale = 0.5f;
    quantize_islem(ham_veriler, q_tensor);

    printf("--- Union Tabanli Tensor Raporu ---\n");
    for (uint32_t i = 0; i < n; i++) {
        // De-quantization: f = S * (q - Z)
        float recovered = q_tensor->scale * (q_tensor->data.i8[i] - q_tensor->zero_point);
        printf("[%d] Bellekteki (int8): %4d | Gercek (float): %.1f\n",
                i, q_tensor->data.i8[i], recovered);
    }

    // Temizlik
    free(q_tensor->data.raw); // Artık f32 yerine raw'ı free ediyoruz
    free(q_tensor);

    return 0;
}