# M5_TOF.ino

このサンプルスケッチは以下について記述している；

- TOF測距センサー（U010）実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 解説

[[M5Stack : Time-of-Flight Distance Ranging Sensor Unit (VL53L0X) SKU: U010]](https://shop.m5stack.com/collections/m5-sensor/products/tof-sensor-unit)
（以下`TOF`）
は`VL53L0X`レーザー測距センサーを搭載したモジュールだ。

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

`VL53L0X`からは測距距離をミリ単位で取得することができる。
通電後は使用に先立ち`initialize`メソッドを呼ばなければならない。
`update`メソッドが真を返せば最新情報取得に成功し、
`getDistance`メソッド（単位はミリメートル）で測定結果を取得できる。
測距可能範囲は概ね`50`〜`2000`ミリメートルである。
測定範囲外は取得できず`update`メソッドが偽を返す。

```c
#include <M5_Modules.h>
TOF_VL53L0X_Class TOF = {Wire};

/* setup */
TOF.initialize(); /* true is success */

/* loop */
if (TOF.update()) { /* true is success */
  int distance = TOF.getDistance();
}
```

取得した生の値は外光の影響も受けてかなり暴れているので、平準化したほうが良い。
しかし積算回数を増やすとそのぶん即応性に欠けてくるので、
過度に追求しないほうが良いかも知れない。

> 校正や制御の詳細はオープンソースではないため、ここでの制御には組み込まれていない。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
