/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <string>
#include <tchar.h>

#include "Mutex.h"
#include "SharedExport.h"
#include "hidemaruexe_export.h"
#include "string_converter.h"
#include "hm_original_encode_mapfunc.h"

using namespace std;

#pragma comment (lib, "winmm.lib")


#pragma data_seg("HM_SHARED_OUTPUT_PANE_SEG")
HWND hCurHidemaruWndHandle = 0;
wchar_t szBufList[BUF_LINE][BUF_CHRS] = {};			// 外部プログラムから書き込まれるバッファ。BUF_LINE行で、1行あたりBUF_CHRS文字まで。
int InputIndex = 0;								    // 外部から共有メモリの書き込みで、今何番目まで書き込んでいるか。循環で利用される。
int OtputIndex = 0;						            // 内部から吐き出したのが、何番目まで吐き出したのか。循環で利用される。
int lstInputTimeGetTime = ::timeGetTime();          // 一番最後に外部から内部に書き込まれた時刻。
int lstOtputTimeGetTime = 0;					    // 一番最後に外部から内部に書き込まれた時刻。
DWORD curAutoUpdateTimeGetTime = ::timeGetTime();
DWORD preAutoUpdateTimeGetTime = ::timeGetTime();
#pragma data_seg()



CHidemaruExeExport HMEXE;



void OutputDebugStream(const wchar_t *format, ...) {
	wchar_t szBufDebug[4096] = L"";

	va_list arg;

	va_start(arg, format);
	_vstprintf_s(szBufDebug, format, arg);
	va_end(arg);

	OutputDebugString(szBufDebug);
}



HWND hWndOutputPane = NULL;
void OnHidemaruHandleUpdate(HWND hLocalHidemaruWndHandle) {

	if (hCurHidemaruWndHandle) {
		if (hHmOutputPaneDLL && pOutputFunc && pOutputGetWindowFunc) {
			// 対象の開いているファイル等が変化すると、対象のアウトプットウィンドウハンドルも変化する可能性
			hWndOutputPane = pOutputGetWindowFunc(hLocalHidemaruWndHandle);
		}
	}
}



// 外部から秀丸ハンドルを設定する。主に秀丸マクロからの設定が想定されている。
extern "C" intptr_t _cdecl SetHidemaruHandle(HWND handle) {
	hCurHidemaruWndHandle = handle;
	// OutputDebugStream("%d\n", handle);
	return 1;
}

intptr_t iCearMilliSecond = -1;
// 一定以上書き込みが無かった場合に、次の書き込み時、クリアするかどうか。
extern "C" int _cdecl SetWithClearTime(intptr_t iMilliSecond) {
	iCearMilliSecond = iMilliSecond;
	return 1;
}

extern "C" void _stdcall SetSharedMessage(const char *szmsg) {
	SetSharedMessageA(szmsg);
}

// 外部からメッセージをDLL内部にコピーする。
extern "C" void _stdcall SetSharedMessageA(const char *szmsg) {

	if (!szmsg) { return; }

	wstring wstr = cp932_to_utf16(szmsg);
	SetSharedMessageW(wstr.data());
}

// 外部からメッセージをDLL内部にコピーする。
extern "C" void _stdcall SetSharedMessageW(const wchar_t *szmsg) {

	if (!szmsg) { return; }

	wstring wstr = wstring(szmsg);

	// 排他Mutexオブジェクトを作成。ハンドルを得る 
	Mutex::hMutex = CreateMutex(NULL, FALSE, Mutex::GetMutexLabel());
	Mutex::BgnMutexLock();

	// バッファは順番に使っていくこととなる。
	// 共有メモリにコピーする。
	_tcsncpy_s(szBufList[InputIndex], wstr.data(), BUF_CHRS - 1);
	// 共有メモリのどのインデックスまで使ったかを覚えておく。BUF_LINEまでを順繰りで利用する。
	InputIndex++;
	if (InputIndex == BUF_LINE) { InputIndex = 0; }

	// 一番最後に外部から内部へと書き込まれた時間を控えておく
	lstInputTimeGetTime = ::timeGetTime();

	Mutex::EndMutexLock();
}


DWORD curTimeGetTime = ::timeGetTime();
DWORD preTimeGetTime = ::timeGetTime();



// 外部から秀丸ハンドルを設定する。
bool isMustBreakLoop = 0;
unsigned __stdcall OutputSharedMessage(void *) {
	intptr_t ClearTime = iCearMilliSecond; // ?_秒 最後の表示から30秒以上経過していた場合、次の文字列表示の前に「Output枠のクリア」も入れる

	while (TRUE) {
		if (isMustBreakLoop) {
			break;
		}
		// 秀丸のアウトプット枠の速度を考慮すると、この程度は休憩を挟んでいてよい。
		Sleep(30);

		if (isMustBreakLoop) {
			break;
		}
		// 秀丸のアウトプット枠の速度を考慮すると、この程度は休憩を挟んでいてよい。
		Sleep(30);

		if (isMustBreakLoop) {
			break;
		}
		// 秀丸のアウトプット枠の速度を考慮すると、この程度は休憩を挟んでいてよい。
		Sleep(30);

		if (!IsWindow(hCurHidemaruWndHandle)) {
			hCurHidemaruWndHandle = CHidemaruExeExport::GetCurWndHidemaru();
		}

		// 秀丸のウィンドウハンドルをマクロからもらっていない。何もしない
		if (!hCurHidemaruWndHandle) {
			continue;
		}

		/*
		直接実行する方法は無いですが、少々無理矢理ではありますが、
		[マクロ]→[マクロ登録]で登録したマクロの番号を指定して、WM_COMMANDを送るということは
		できてしまうと思います。
		参考までの情報として、wParamに指定するコマンド値は、
		マクロ1〜15のコマンド値は146〜160
		マクロ16〜30のコマンド値は205〜219
		マクロ31〜のコマンド値は234〜
		になっています。
		*/
		//	SendMessage( (HWND)hCurHidemaruWndHandle, 0x111, 235, 0 ); // 235=マクロ32のこと!!
		if (isMustBreakLoop) {
			break;
		}

		if (InputIndex == OtputIndex) {
			continue;
		}

		curTimeGetTime = ::timeGetTime();


		// ローカルのハンドルとして、更新する。
		HWND hLocalHidemaruWndHandle = hCurHidemaruWndHandle;
		OnHidemaruHandleUpdate(hLocalHidemaruWndHandle);

		// 最後に出力した時間から、○秒以上経過している。=アウトプット領域は一度クリアすべきだ。
		if (ClearTime > 0) {
			if ((int)curTimeGetTime - lstOtputTimeGetTime > ClearTime) {
				SendMessage((HWND)hWndOutputPane, WM_COMMAND, 1009, 0);  // 前回の結果は残さない
			}
		}

		if (isMustBreakLoop) {
			break;
		}

		// 排他Mutexオブジェクトを作成。ハンドルを得る 
		Mutex::hMutex = CreateMutex(NULL, FALSE, Mutex::GetMutexLabel());
		Mutex::BgnMutexLock();

		// 永遠にループしてしまわないように、最大でも、BUF_LINE-1個までで一度出力する。
		int maxcnt = 0;
		while (TRUE) {

			if (isMustBreakLoop) {
				break;
			}

			wstring line = wstring(szBufList[OtputIndex]);

			if (pOutputFunc) {
				line += L"\n";

				BOOL success = FALSE;
				if (pOutputWFunc) {
					success = pOutputWFunc((HWND)hLocalHidemaruWndHandle, (wchar_t*)(line.data()));
				} else {
					vector<BYTE> bite_data = EncodeWStringToOriginalEncodeVector(line);
					// --------------------- １行の文字列にパッチを当てていく ここまで----------------------------
					success = pOutputFunc((HWND)hLocalHidemaruWndHandle, bite_data.data());
				}

				// アウトプットパネルへの出力は、詰まってると失敗することがある
				if (success) {

					// 成功して初めて…
					szBufList[OtputIndex][0] = NULL;
					// 出力したら、共有メモリの出力インデックスを１つ増やす。
					// １つ増やす
					OtputIndex++;
				}
			}

			// 途中で現在のアウトプット先のOutputの関数が捕捉できないという事態
			else {
				if (isMustBreakLoop) {
					break;
				}
				Sleep(30);
				if (isMustBreakLoop) {
					break;
				}
				Sleep(30);
				if (isMustBreakLoop) {
					break;
				}
				Sleep(30);
				continue;
			}

			// 共有メモリのどのインデックスまで出力したかを覚えておく。BUF_LINEまでを順繰りで利用する。

			if (OtputIndex == BUF_LINE) { OtputIndex = 0; }
			// 入力インデックスと一致したら、全部消すそこまで。

			if (InputIndex == OtputIndex) {
				break;
			}

			// 最大でも、BUF_LINE-1個までで一度出力する。
			maxcnt++;
			if (maxcnt == BUF_LINE - 1) {
				break;
			}
		}

		lstOtputTimeGetTime = curTimeGetTime;
		Mutex::EndMutexLock();

		Sleep(10);

		if (isMustBreakLoop) {
			break;
		}

	}

	return 0;
}


