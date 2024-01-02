# M5_NCIR.ino

このサンプルスケッチは以下について記述している；

- NCIR非接触温度センサー（U028）実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 解説

[[M5Stack : NCIR Non-Contact Infrared Thermometer Sensor Unit (MLX90614) SKU: U028]](https://shop.m5stack.com/collections/m5-sensor/products/ncir-sensor-unit)
（以下`NCIR`）
は`MLX90614`非接触温度センサー
を搭載してI2C制御できるデバイスだ。
ここではその計測値をシリアルプロッタに出力する。

センサー接続は次のようにする。
__"Zinnia Duino"__ の場合はオンボード上の
Grove/M5互換コネクタに結線するだけで良い。
IOREF選択電圧は3V/5Vどちらでもよい。

```plain
[Sensor]   [modernAVR]
    SCL <-- PIN_PA3
    SDA <-> PIN_PA2
     5V <-- 5V
    GND --> GND
```

## SMBus

`MLX90614`はSMBus規格デバイスだ。
これは電気的にも通信規約的にも`I2C`と互換性があるが完全に同じではない。
応用コードレベルでは次の違いに注意を要する。

- 通信速度が（SMBusバージョンにも依るが） 100kbps±5% でなければならない。これは他の`I2C`デバイスとの混用に制約を課す。
- 送受信とも1回のトランザクションペイロード末尾に「8bit CRC」が付く。
CRCは`PEC`と呼ばれる方式で、ペイロード以前の主装置が従装置に送った
アドレス指定やレジスタ番地指定のビットフィールドもすべて計算に含み、検証される。
つまり1トランザクションにつき、1PECを付加する。

`PEC`についてはデータ読み出しにしか使わない通信では無視しても構わないが、
SMBusが本来想定している高ノイズ高温環境（ほぼ車載向け）用途では検証すべきだろう。
レジスタ変更のためにデータ送信をするならば省略できず必須である。

## データ取得

`MLX90614`デバイス通電後は使用に先立ち`initialize`メソッドを呼ばなければならない。
そのあとは`update`メソッドが真を返せば最新情報取得に成功し、
`getAmbientTemperature`メソッド（単位は摂氏）で環境温度が、
`getObjectTemperature`メソッド（単位は摂氏）で非接触センサー温度が、それぞれが取得できる。

```c
#include <M5_Modules.h>
NCIR_MLX90614_Class NCIR = {Wire};

/* setup */
Wire.initiate(TWI_SM, false);

if ( ! NCIR.initialize() ) {
  /* Failed ! */
}

/* loop */
if ( NCIR.update() ) { /* true is success */
    float temp0 = NCIR.getAmbientTemperature();
    float temp1 = NCIR.getObjectTemperature();
 // float temp2 = NCIR.getObjectTemperature2();
}
```

`MLX90614`はバリエーションによっては第2の非接触センサーを持っているものがあるので、
それは`getObjectTemperature2`メソッド（単位は摂氏）で取得できる。

## 通信障害

SMBus通信中に電源断があると`MLX90614`内部ステートマシンが異常状態に陥って
以後の通信に応答しなくなることが時折ある。
ほとんどの場合は`initialize`メソッドでこの状態を回復できるが、
稀に内蔵EEPROM内容が化けて往生する現象が見られる。
この場合は初期設定値を強制的に送り込んでから電源を入れ直せば回復できる __かもしれない。__

> 毎秒数回データ取得する高頻度運用では特にこの障害を招きやすい。

```c
/* 初期設定値は個体によって異なるため、以下に上げるのは一例でしかない */

/* シングルセンサータイプの例 */
NCIR.SMBus_write(NCIR_MLX90614_enum::NCIR_ROM_CONFIG_gc, 0x9FB4);

/* デュアルセンサーまたは望遠センサータイプの例 */
NCIR.SMBus_write(NCIR_MLX90614_enum::NCIR_ROM_CONFIG_gc, 0xAFF4);
```

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
