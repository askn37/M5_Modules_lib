# M5_GPS_NMEA.ino

このサンプルスケッチは以下について記述している；

- Mini GPS/BDS（U032）NMEA表示実演

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
この実演はその NMEAテキスト出力をシリアルモニターにプリントする。

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

## GPS設定

`GPS`モジュールの初期状態では`9600bps 8N1`で通信を開始する。
搭載されている`AT6558`は以下の独自コマンドを認識する。

### $PCAS01

USART通信速度を変更する。指定値は以下のインデックス。既定値は「9600bps」。
これは電源を切っても暫くは（バッテリーバックアップされて）既定値に戻らないことに注意。

```plain
$PCAS01,1*1D<CR><LF> /* 9600bps */
$PCAS01,3*1F<CR><LF> /* 38400bps */
$PCAS01,5*19<CR><LF> /* 115200bps */
```

### $PCAS02

NMEA出力間隔を変更する。指定はミリ秒単位で行う。既定値は「1Hz」（1000ms）。
最大10Hz（100ms）まで短くできる。

```plain
$PCAS02,1000*2E<CR><LF> /* 1000ms=1Hz */
$PCAS02,200*1D<CR><LF>  /* 200ms=5Hz */
```

### $PCAS03

8種類の NMEA情報の各々を、出力するかしないかを`1`以上か`0`で指定する。
`0`は出力を止める。`1`は`$PCAS02`の時間間隔に対する倍率。

8桁の指定は左から`GGA,GLL,GSA,GSV,RMC,VTG,ZDA,TXT`である。

```plain
$PCAS03,1,1,1,1,1,1,1,1*02<CR><LF> /* 初期値 */
$PCAS03,0,0,0,0,1,0,0,0*03<CR><LF> /* RMC以外停止 */
$PCAS03,0,0,0,0,0,0,0,0*04<CR><LF> /* 全部停止 */
```

### $PCAS04

追跡する測位衛星の種類を決める。

```plain
$PCAS04,1*18<CR><LF> /* GPS */
$PCAS04,5*1C<CR><LF> /* GPS+GLONASS */
$PCAS04,7*1E<CR><LF> /* GPS+BDS+GLONASS */
```

### $PCAS12

指定の秒数のあいだ、測位モジュールを省電力休止状態にする。
ただしこれは対応するファームウェアが書き込まれている場合に限る。
通常販売品は対応していない。

```plain
$PCAS12,60*28<CR><LF> /* Going Standby 60sec (Firmware V4.2.0.3) */
```

## I2Cデバイスとの併用

一見、
[[M5Stack : I2C Hub 1 to 6 Expansion Unit (PCA9548APW) SKU: U040-B]](https://shop.m5stack.com/collections/m5-sensor/products/i2c-hub-1-to-6-expansion-unit-pca9548apw)
（以下`I2CHUB`）
で配線を分配し、`I2CHUB`のポート制御で`GPS`とその他の棲み分けができそうに思えるが、実際にやってみるとできない。
無通信状態でも`GPS`側の`TXD`線が`HIGH`を強制するため
`SCL`信号が正しく機能できないからだ。

この結果`I2CHUB`が初期状態で閉ざしているポートを`I2C`通信で開くことはできるが、
いったん`GPS`ポートを開いてしまうと、閉じるためのコマンドが
`I2CHUB`に届かなくなって、電源を切るまで閉じられなくなる。

ただ失敗する理由は明白なので、`GPS`モジュールの`TXD`線を
ダイオードやトランジスタの追加でオープンドレイン制御状態にすれば、
混在可能にすることは不可能ではない。
もっとも素直に、両者は異なる外部ポートで制御するほうが確実かつ簡単ではある。

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
