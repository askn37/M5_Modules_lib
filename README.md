# Multix Zinnia Product SDK [M5_Modules]

__Multix Zinnia Product SDK__
の共通サブモジュール。

AVR-GCC / AVR-LIBC 専用。__Arduino互換API用ではない。__

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## パス配置

このモジュールは __Arduino IDE__ の
`boards.txt/platform.txt`において
以下のパスに配置される。

```plain
{runtime.platform.path}/cores/{build.core}        <-- MacroAPI_core {includes}
{runtime.platform.path}/libraries/{build.core}    <-- MacroAPI_libraries
{runtime.platform.path}/libraries/M5_Modules      <-- このサブモジュール
{runtime.platform.path}/variants/{build.variant}  <-- ボード固有設定 {includes}
```

## 収容ライブラリ

|名称|機能|
|-|-|
|M5_Modules.h|以下のすべてを一括導入
|_M5_Modules/_|
|M5_ENV3_QMP6988.h|ENV3環境センサー
|M5_ENV3_SHT3X.h|ENV3環境センサー
|M5_HUB_PCA9548AP.h|I2C分配HUBモジュール
|M5_NCIR_MLX90614.h|NCIR非接触温度センサー
|M5_OLED_SH1107.h|OLED表示機モジュール
|M5_TOF_VL53L0X.h|TOF測距センサー

## 収容サンプルスケッチ

|名称|目的|
|-|-|
|M5_ENV3|ENV3環境センサー実演
|M5_GPS_LonLat|GPS測位モジュール実演（緯度経度表示）
|M5_GPS_NMEA|GPS測位モジュール実演（NMEA表示）
|M5_NCIR|NCIR非接触温度センサー実演
|M5_NCIR_OLED|NCIR非接触温度センサー＋OLED表示機モジュール実演
|M5_OLED_Console|OLED表示機モジュール実演（スクロール表示例）
|M5_OLED_Freq|OLED表示機モジュール実演（周波数測定表示例）
|M5_TOF|TOF測距センサー実演

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
