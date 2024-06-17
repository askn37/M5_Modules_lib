# M5_NCIR_OLED.ino

このサンプルスケッチは以下について記述している；

- NCIR非接触温度センサー＋OLEDディスプレイ表示（U028+U119）実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR-Dx系統
  - AVR-Ex系統

## 解説

ここでは\
[[M5Stack : NCIR Non-Contact Infrared Thermometer Sensor Unit (MLX90614) SKU: U028]](https://shop.m5stack.com/collections/m5-sensor/products/ncir-sensor-unit)
（以下`NCIR`）
で取得したデータを\
[[M5Stack : OLED Unit 1.3" 128 × 64 Display SKU: U119]](https://shop.m5stack.com/collections/m5-sensor/products/oled-unit-1-3-128-64-display)
（以下`OLED`）
に表示する。
`I2C`バスの配線は信号分配が必要なので、\
[[M5Stack : I2C Hub 1 to 6 Expansion Unit (PCA9548APW) SKU: U040-B]](https://shop.m5stack.com/collections/m5-sensor/products/i2c-hub-1-to-6-expansion-unit-pca9548apw)
（以下`I2CHUB`）
または\
[[M5Stack : 1 to 3 HUB Expansion Unit (GROVE Expander) SKU: U006]](https://shop.m5stack.com/collections/m5-sensor/products/mini-hub-module)
と併用する。

## I2CHUB の制御

`I2CHUB`（`PCA9548AP`）のI2Cバスチャネル選択は次のようにする。

```c
#include <M5_Modules.h>
HUB_PCA9548AP_Class I2CHUB = {Wire};

/* get current channels bitflags */
uint8_t chs = I2CHUB.get();

/* Ch[0-7] all bitflags enable */
I2CHUB.set(0xFF);
```

電源投入直後は全チャネルが閉じているので必ず`set`メソッドの実行が必要だ。
与えるビットフラグは`LSB`下位ビットから順に`Ch[0-7]`となっている。
複数のチャネルを同時に開くことも、すべて閉じることもできる。

> 上記製品の場合`Ch[67]`は配線されておらず使用不可となる。
また基板上のポート番号表記は`[1-6]`である。適宜`Ch[0-5]`に読み替えること。

ジャンパーパッドを操作して制御アドレスをデフォルト`0x70`以外に
変更している場合は、クラスインスタンスにその指定`HUB_PCA9548AP_ADDR_[0-7]`を追加する。
複数の`I2CBUS`を同時に配線した場合は、
クラスインスタンスをその数だけ用意して使い分ける。

```c
HUB_PCA9548AP_Class I2CHUB = {Wire, HUB_PCA9548AP_ADDR_0};
```

要注意なのは
[`ENV3(QMP6988)`](https://shop.m5stack.com/collections/m5-sensor/products/env-iii-unit-with-temperature-humidity-air-pressure-sensor-sht30-qmp6988)
環境センサーと同時使用する場合で、そちらの制御アドレスも`0x70`であり、
かつ固定されていて変更できない。
したがってそれがひとつでもバス配線上にあると`I2CHUB`の制御に支障をきたす。
この場合は`I2CHUB`側のジャンパーパッドを必ず初期状態から変更しなければならない。

> 制御アドレスを変更しないで同時使用すると`QMP6988`にアクセスする度に
勝手に`PCA9548AP`の設定が変わったりバスエラーが多発してまともな制御ができない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
