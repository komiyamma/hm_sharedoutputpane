# HmSharedOutputPane

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/komiyamma/hm_sharedoutputpane?label=HmSharedOutputPane&color=6479ff)](https://github.com/komiyamma/hm_sharedoutputpane/releases)
[![MIT](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)
![Hidemaru 8.01](https://img.shields.io/badge/Hidemaru-v8.01-6479ff.svg)

「外部のアプリケーション(=秀丸から起動していないアプリケーション)」から、「今開いている秀丸」の「アウトプット枠」へと直接出力するためのコンポーネントです。

より詳細な情報は、作者様のウェブサイトを参照してください。
[https://秀丸マクロ.net/?page=nobu_tool_hm_sharedoutputpane](https://秀丸マクロ.net/?page=nobu_tool_hm_sharedoutputpane)

---

## 概要

**HmSharedOutputPane** は、秀丸エディタの強力なプラグインで、コマンドラインツールや他のGUIアプリケーションなど、秀丸エディタの外部で実行されているプロセスから、秀丸のアウトプット枠へ直接テキストを送信することができます。

バックグラウンドで動作するコンパイル処理の結果をリアルタイムで表示したり、他のツールからのログを受け取ったりするのに非常に便利です。

## 主な機能

- **即時出力**: 外部アプリケーションからメッセージを即座にアウトプット枠に表示します。
- **自動表示**: アウトプット枠が閉じられている場合でも、メッセージ受信時に自動的に開きます。
- **複数アプリ対応**: 複数のアプリケーションから同時にメッセージを送信しても、問題なく処理されます。
- **堅牢な動作**: 秀丸が後から起動されたり、再起動されたりした場合でも、その間のメッセージを（バッファの範囲内で）保持し、起動後に表示します。
- **自動クリア**: 最後の出力から指定時間が経過した場合に、次回の出力時にアウトプット枠を自動でクリアする機能があります。

## 動作環境

- 秀丸エディタ v8.01 以降

## インストール

1. リリースページから、お使いの秀丸エディタのビット数（32bit/64bit）に合った `HmSharedOutputPane.zip` をダウンロードします。
2. `HmSharedOutputPane.zip` を解凍します。
3. `HmSharedOutputPane.dll` を、秀丸エディタ本体(`hidemaru.exe`)と同じディレクトリにコピーします。
4. 秀丸エディタのメニューから `マクロ` > `マクロ登録` を開きます。
5. 「自動起動」の設定項目で、「アクティブ切り替え」イベントに対して、解凍した `HmSharedOutputPane.mac` を登録します。

## 使い方

このリポジトリには、DLLの機能を利用するための実行ファイルが含まれています。

### コマンドラインからの利用

`HmSharedOutputPaneCon.exe` (C++) または `HmSharedOutputPane.exe` (C#) を使うと、コマンドラインから簡単にメッセージを送ることができます。

**例:**
```shell
> HmSharedOutputPaneCon.exe "これはテストメッセージです。"
```
```shell
> HmSharedOutputPane.exe "C#版からのテストメッセージです。"
```
引数として渡された文字列が、現在アクティブな秀丸エディタのアウトプット枠に出力されます。

### 各言語からの呼び出し

DLLを直接利用することで、様々なプログラミング言語からアウトプット枠への出力機能を利用できます。
詳細は作者様のウェブサイトで解説されています。
- [各言語での簡易な呼び出し例](https://xn--pckzexbx21r8q9b.net/?page=nobu_tool_hm_sharedoutputpane_call)
- [C#での応用的な呼び出し例](https://xn--pckzexbx21r8q9b.net/?page=nobu_tool_hm_sharedoutputpane_call2)

## ビルド方法

このプロジェクトは Visual Studio 2017 以降でビルドできます。
リポジトリのルートにある `HmSharedOutputPane.src\HmSharedOutputPane.sln` ファイルを Visual Studio で開いてビルドしてください。

## ライセンス

このプロジェクトは [MIT License](LICENSE) の下で公開されています。
