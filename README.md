🛰️ TMUY2025 Model Uydu Yer İstasyonu Yazılımı
Bu proje, TEKNOFEST 2025 Model Uydu Yarışması (TMUY2025) kapsamında  geliştirilmiş olan Yer İstasyonu Arayüzü (GUI) yazılımıdır.

Yazılım, model uydudan gelen telemetri verilerini seri port üzerinden okur, ayrıştırır, görselleştirir ve kayıt altına alır. Aynı zamanda yer istasyonu kamerasından görüntü alıp kayıt yapma yeteneğine sahiptir.

🚀 Özellikler
1. Seri Port Haberleşmesi ve Telemetri
Otomatik Port Algılama: Bilgisayara bağlı aktif COM portlarını listeler.

Veri Okuma: Belirlenen Baud Rate (Varsayılan 9600) üzerinden gelen CSV formatındaki verileri okur.

Veri Ayrıştırma (Parsing): Gelen veri paketini; Basınç, Yükseklik, Sıcaklık, Pil Gerilimi, GPS, İniş Hızı ve Oryantasyon (Pitch, Roll, Yaw) verilerine ayırır.

CSV Loglama: Gelen tüm verileri TMUY2025_K1_687324_TLM.csv dosyasına anlık olarak kaydeder.

2. Gerçek Zamanlı Grafikler (QCustomPlot)
Aşağıdaki veriler için gerçek zamanlı akan grafikler oluşturulur:

Yükseklik Grafiği: Taşıyıcı ve Görev Yükü yükseklikleri ile aralarındaki fark.

Sıcaklık Grafiği: Sensör sıcaklık verisi.

Pil Gerilimi Grafiği: Uydunun pil durumu.

Basınç Grafiği: İki farklı basınç sensörü verisi.

İniş Hızı Grafiği: Uydunun dikey iniş hızı.

3. Harita ve Konum Takibi
QML Harita Entegrasyonu: OpenStreetMap altyapısı kullanılarak uydunun GPS (Enlem/Boylam) verisi harita üzerinde canlı olarak işaretlenir.

4. Multimedya ve Video Kaydı
Canlı Kamera Yayını: Bilgisayara bağlı kameraları algılar ve arayüzde gösterir.

Video Kaydı: Kamera görüntüsünü kayit.mp4 olarak yerel diske kaydeder.

5. ARAS (Hata Durum Göstergesi)
Uydudan gelen hata koduna (Error Code) göre arayüzdeki durum ledlerini (Yeşil/Kırmızı) otomatik olarak günceller.

6. Manuel Komut Gönderme
Ayrılma Komutu: Belirlenen port üzerinden uyduya manuel "AYRIL" komutu göndererek görev yükü ayrılma mekanizmasını tetikler.

🛠️ Kullanılan Teknolojiler ve Kütüphaneler
Bu proje C++ dili ve Qt Framework kullanılarak geliştirilmiştir.

Qt 6.x (Core, Gui, Widgets, SerialPort, Multimedia, MultimediaWidgets, QuickWidgets)

QCustomPlot: Grafik çizimleri için kullanılmıştır.

OpenStreetMap (QML): Konum takibi için.


📊 Telemetri Formatı
Yazılım, verileri şu sırayla virgülle ayrılmış (CSV) olarak bekler: <PKT_NO>, <STATU>, <HATA_KODU>, <SAAT>, <BASINC1>, <BASINC2>, <YUKSEKLIK1>, <YUKSEKLIK2>, <FARK>, <INIS_HIZI>, <SICAKLIK>, <PIL>, <GPS_LAT>, <GPS_LONG>, <GPS_ALT>, <PITCH>, <ROLL>, <YAW>, <RHRH>, <IOT1>, <IOT2>, <TAKIM_NO>
