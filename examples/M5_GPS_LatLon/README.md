# M5_GPS_LatLon.ino

このサンプルスケッチは以下について記述している；

- Mini GPS/BDS（U032）緯度経度表示実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 解説

[[M5Stack: Mini GPS/BDS Unit (AT6558) SKU:U032]](https://shop.m5stack.com/collections/m5-sensor/products/mini-gps-bds-unit)
（以下`GPS`）
は`UART`接続のGPS位置測位モジュールだ。
この実演はその時刻情報と位置情報をシリアルモニターにプリントする。

配線接続は次のようにする。
__"Zinnia Duino"__ の場合はオンボード上の
Grove/M5互換コネクタに結線するだけで良い。
IOREF選択電圧は3V/5Vどちらでもよい。

```plain
[Sensor]   [modernAVR]
    TXD --> PIN_PA3
    RXD <-- PIN_PA2
     5V <-- 5V
    GND --> GND
```

この製品の Grove端子は`UART`通信用（青）である。
__"Duino"__ のこのコネクタは本来`I2C`通信用（赤相当）で
ハードウェア支援USARTは接続されていない。
そこで`<SoftwareUART.h>`を利用してソフトウェアUARTを構築し、通信する。

> 他の`I2C`デバイスと配線は共用できない。
`I2CHUB`等を用いて分配することもできない。

## 解説

`GPS`の NMEA出力は`<SoftwareUART.h>`で読み取り、
`<GPS_RMC.h>`で`$**RMC`行を解析し、UTC時刻と現在位置を取得する。

観測視野にひとつも衛星が入ってきていない場合、
時刻情報も位置情報も取得できない。

観測視野にひとつ以上の衛星が入ってくると、
とりあえず時刻情報が獲得され
`GPS`モジュール内の`RTC`が同期され、
1秒後から時刻情報が取得可能になる。
ここからは通電している限り時刻情報が獲得できなくなることはない。

観測視野に3つ以上の衛星が一定時間以上入ってくるか、
4つ以上の衛星が入ってきて信号が同期されると、位置情報が獲得される。
位置精度は追跡中の衛星数に依存し、
最大で緯度に対し`1/60万秒`（約30cm）まで校正される。

> AVR-LIBCの場合、通常は浮動小数点精度が`float`に制限されるため、
最高精度は（現在位置が日本の場合は）経度方向で`1/10万秒`（約2m）に低下する。
これは緯度よりも経度の方が10進数で1桁大きいため最小精度も1桁大きくなることによる。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
