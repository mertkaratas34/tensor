# C-Tensor

Bu proje, C dilinde düşük seviyeli bellek yönetimi kullanılarak geliştirilmiş, hafif ve verimli bir **Tensor** yapısıdır. Proje, özellikle belleğin kısıtlı olduğu senaryolarda veriyi en optimize şekilde tutmak ve işlemek amacıyla tasarlanmıştır.

## 🚀 Öne Çıkan Özellikler

* **Union-Based Memory:** `union` yapısı sayesinde `float32`, `float16` ve `int8` pointerları aynı bellek adresini paylaşır. Bu, veri tipine göre dinamik ve efektif RAM kullanımı sağlar.
* **INT8 Quantization:** Floating-point (ondalıklı) verileri 8-bit tam sayılara dönüştürerek veri boyutunu %75'e varan oranda küçültür.
* **Safety Clamping:** Kuantizasyon işlemi sırasında `int8` sınırlarını (-128, 127) aşan değerleri otomatik olarak sabitler (overflow protection).
* **Dequantization:** Sıkıştırılmış veriyi tekrar orijinal ölçeğine döndürme desteği.

## 🛠️ Teknik Detaylar

### Veri Yapısı
Tensörler, sadece veri dizisi değil, aynı zamanda verinin nasıl yorumlanacağını söyleyen meta-verileri de içerir:

| Bileşen | Açıklama |
| :--- | :--- |
| `DataType` | Verinin tipi (FLOAT32, INT8 vb.) |
| `TensorBuffer` | Bellekteki ham verinin paylaşımlı adresi |
| `Scale` | Veri sıkıştırma katsayısı (f = q * scale) |

### Kuantizasyon Formülü
Sıkıştırma işlemi şu formül ile gerçekleştirilir:
$$q = \text{round}\left(\frac{f}{\text{scale}}\right) + \text{zero\_point}$$

## 💻 Kullanım Örneği

```c
// 1. Tensör oluştur (4 elemanlı, INT8 tipinde)
Tensor* q_tensor = tensor_olustur(4, DT_INT8);

// 2. Ölçek belirle ve kuantize et
q_tensor->scale = 0.5f;
quantize_islem(ham_veriler, q_tensor);

// 3. Veriyi kullan ve temizle
free(q_tensor->data.raw);
free(q_tensor);
