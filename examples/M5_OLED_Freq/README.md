# M5_OLED_Freq.ino

このサンプルスケッチは以下について記述している；

- OLEDディスプレイ（U119）周波数表示実演

## 対象AVR

- modernAVR世代
  - AVR Dx系統

> megaAVR-0、tinyAVR-0/1/2系統は対応しない。

## 解説

[[M5Stack : OLED Unit 1.3" 128 × 64 Display SKU: U119]](https://shop.m5stack.com/collections/m5-sensor/products/oled-unit-1-3-128-64-display)
（以下`OLED`）
は 1.3インチ の 128x64 ピクセル OLED を搭載したモジュールだ。
これを用いて CPU自身の動作周波数を計測表示する。

配線接続は次のようにする。
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

AVR_Dx系統では`CLKCTRL_OSCHFTUNE`レジスタで`CLK_PER`の速度を
`-64`から`+63`の範囲で（概ね300ppm単位で）手動調整することができる。
実際の校正は`CLKOUT`端子に`CLK_PER`を出力して、外部周波数計数器で観測するものだが、
自分自身で現在の動作周波数を測定し`OLED`に表示しようというのが本稿の趣旨である。

> 計数結果をシリアルモニターに送っても、USART速度自体が校正の影響を受けて正常出力できない。
> I2Cバス経由のディスプレイ表示なら対応速度範囲が広いので、問題なく表示できる。

周波数測定は
[[コード実行時間を計測するベンチマーク実演（TCB2組32bit計数器使用）]](https://github.com/askn37/MacroMicroAPI_lib/tree/main/examples/Timer%20applications/Benchmark)
で示した手法を使う。
計数は`CLK_PER`の分周比2に設定、
計数捕獲イベントは`RTC_OVF`をそのまま使用して2秒周期で捕獲することで
そのまま計測平均値と見做してディスプレイ表示している。

今回の場合は例えば`115200bpsの200倍＝23040000`を目標とし
`F_CPU=24000000L`設定で実験すると補正値`-11`で概ね近い周波数を得た。
これを自動化して補正値を求めるように拡張することも可能だが、
ここではそこまで実装していない。
むしろ計数入力元を外部端子にして簡易周波数表示器にするほうが有意義だろう。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
BlueSky Social: [@multix.jp](https://bsky.app/profile/multix.jp) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
