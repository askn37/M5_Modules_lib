# M5_TOF.ino

このサンプルスケッチは以下について記述している；

- OLEDディスプレイ（U119）キャラクタコンソール実演

## 対象AVR

- megaAVR世代
  - megaAVR-0系統
  - tinyAVR-0/1/2系統
- modernAVR世代
  - AVR Dx系統

## 解説

[[M5Stack : OLED Unit 1.3" 128 × 64 Display SKU: U119]](https://shop.m5stack.com/collections/m5-sensor/products/oled-unit-1-3-128-64-display)
（以下`OLED`）
は 1.3インチ の 128x64 ピクセル OLED を搭載したモジュールだ。
`<M5_OLED_SH1107.h>`はこれを横向きの 16キャラクタx4行 英数コンソールとして使用できる機能を提供する。

<blockquote class="twitter-tweet"><p lang="ja" dir="ltr"><a href="https://twitter.com/hashtag/%E9%9B%BB%E5%AD%90%E5%B7%A5%E4%BD%9C?src=hash&amp;ref_src=twsrc%5Etfw">#電子工作</a> M5 OLED(SH1107)のキャラクコンソール風AVR実装のテスト。ぬめぬめスクロール改行付。かなり速度落として撮影してるが全速60fps行ける模様。 <a href="https://t.co/5VNZZzjeEY">pic.twitter.com/5VNZZzjeEY</a></p>&mdash; 朝日薫舞鶴甲27 (@askn37) <a href="https://twitter.com/askn37/status/1584807313395036160?ref_src=twsrc%5Etfw">October 25, 2022</a></blockquote> <script async src="https://platform.twitter.com/widgets.js" charset="utf-8"></script>

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

## 初期化

`TWI/I2C`バスの初期化では、速度指定は普通`TWI_SM`とするが
他にこのバスを使うセンサーがないのであれば`TWI_FM`でもよく
キャラクタ表示速度を高速化できる。
バスのAVR側プルアップ抵抗指定はモジュール側のを使うので`false`とする。

`OLED`クラスインスタンスには`Wire`インスタンスと、使用するフォントセットを指定する。
__MicroAPI__ が標準で用意している対応フォントセットは`moderndos_8x16`なので、
これを指定する。表示可能文字コードは`0x20`から`0x7F`の範囲だ。

`OLED`の初期化はまず`clear`メソッドを呼ぶことで始まる。
このメソッドは自身のインスタンスを返すので、
メソッドチェーンが可能だ。

```c
#include <M5_Modules.h>
OLED_SH1107_Class OLED = {Wire, moderndos_8x16};

/* setup */
Wire.initiate(TWI_SM, false);
OLED
  .clear()
// .setFlip(true)
// .setRevesible(true)
// .setExtTableInROM(moderndos_8x16) /* 0x80-0xFF External character bank */
// .setExtTableInRAM(extra_table)    /* 0x10-0x1F Volatility External character area */
;
```

`setFlip(bool)`はディスプレイの向きを決める。
`false`は右上原点、`true`は左下原点になる。

`setRevesible(bool)`はピクセルの白黒を決める。
`false`は黒地に白、`true`は白地に黒で表示する。

`setExtTableInROM`は`0x80`から`0xFF`の文字コードに割り付ける外字フォントセットを指定する。使わなければ表示内容は不定だ。

`setExtTableInRAM`は`0x10`から`0x1F`の文字コードに割り付ける
16キャラクタぶん 256バイトの 外字表示用RAMバッファアドレスを指定する。
これはコンソールのちょうど一行分にあたり、
漢字フォント表示やカーソル合成表示に使ったりできる。

標準フォントセットのキャラクタビットマップをこのRAMバッファに転写したり
論理合成したりするには`setExtFont`メソッドが使える。

```c
for (int i = 0; i < 16; i++) {
  OLED
    .setExtFont(&extra_table[i << 4], '@' + i, OLED_SET) /* Load ROM Font */
    .setExtFont(&extra_table[i << 4], '\x0', OLED_XOR)   /* Exor Mask */
  ;
}
```

> `moderndos_8x16`の場合
`"_^~-="`等がグリフ合成に使える。
`0x7F`には `"ﾞ"`（degree）記号のグリフが入っているので
`"Å"`を合成したり、`度ﾞ分'秒"`桁区切りが表示できる。

## スクロール表示

単に`OLED`へプリントしていけば、それはスクロールコンソール表示になる。
`\r`で仮想カーソル位置が行頭に復帰、
`\n`で新規改行、
16文字毎に行頭折返しで自動的にスクロールが進む。

```c
OLED.println(F("Hello World!"));
```

## 絶対位置表示

`clear(false)`メソッドで画面消去すると、以後は絶対位置表示モードになる。
仮想カーソルが画面表示外に移動しても自動スクロールはされない。
仮想カーソル位置は左上原点として`setPosition(X,Y)`で自由に設定できる。

```c
OLED.clear(false);
OLED.setPosition(0, 0).print(F("1st line"));
OLED.setPosition(0, 1).print(F("2nd line"));
```

内部的には8行分の画面があるので、仮想カーソルの`Y`は`7`まで指定可能だ。
5行目から8行目は画面外なのでそのままでは表示されないが、
`setScroll(Z)`メソッドで表示開始位置を変更することができる。
`Z`には`0`から`127`が指定でき、ピクセル単位で調整できる。
これを使うと1ピクセル単位のスムーズスクロールや、
2画面フレームバッファ切り替えができる。（画面上下はリング状に繋がっている）

```c
OLED.setPosition(0, 0).print(F("1st view"));
OLED.setPosition(0, 4).print(F("2nd view"));

OLED.setScroll(0);  /* "1st view" is top */
OLED.setScroll(64); /* "2nd view" is top */
```

特定の行だけをクリアするには`newLine(Y)`メソッドを使用する。

```c
OLDE.newLine(1);
```

絶対位置表示を止めてスクロール表示に戻すには`clear(true)`メソッドを実行する。

## その他

`drawTestPattern`メソッドは画面全体（2画面ぶん）をテストキャラクタで埋め尽くす。

```c
OLED.clear(false);
OLED.drawTestPattern();
for (int i = 0; i < 128; i++) {
  OLED.setScroll(i);
  delay_millis(100);
}
```

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)
