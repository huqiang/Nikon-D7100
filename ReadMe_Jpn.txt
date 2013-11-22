Nikon Type0010 Module SDK Revision.1.0 概要


■用途
 カメラのコントロールを行う。


■サポートするカメラ
 D7100


■動作環境・制限事項
 [Windows]
    Windows XP (SP3)
    (※Professional, Home Edition)
    Windows Vista (SP2) --- 32bit版 / 64bit版(32bit互換モード)
    (※Ultimate, Enterprise, Business, Home Premium, Home Basic)
    Windows 7 (SP1) --- 32bit版 / 64bit版(32bit互換モード)
    (※Ultimate, Enterprise, Professional, Home Premium, Home Basic)
    Windows 8 -- 32bit / 64bit(32bit互換モード)

 [Macintosh]
    MacOS X 10.6.8
    OS X 10.7.5
    OS X 10.8.2
    ※PowerPC/Rosettaでのご使用は、サポートしておりません。


■内容物
 [Windows]
    Documents
      MAID3(J).pdf : 基本インターフェース仕様
      MAID3Type0010(J).pdf : Type0010 Moduleで使用される拡張インターフェース仕様
      Usage of Type0010 Module(J).pdf : Type0010 Module を使用する上での注意事項
      Type0010 Sample Guide(J).pdf : サンプルプログラムの使用方法

    Binary Files
      Type0010.md3 : Windows用 Type0010 Module本体
      NkdPTP.dll : Windows用　PTPドライバ
 
    Header Files
      Maid3.h : MAIDインターフェース基本ヘッダ
      Maid3d1.h : Type0010用MAIDインターフェース拡張ヘッダ
      NkTypes.h : 本プログラムで使用する型の定義
      NkEndian.h : 本プログラムで使用する型の定義
      Nkstdint.h : 本プログラムで使用する型の定義

    Sample Program
      Type0010CtrlSample(Win) : Microsoft Visual Studio 2010 用プロジェクト


 [Macintosh]
    Documents
      MAID3(J).pdf : 基本インターフェース仕様
      MAID3Type0010(J).pdf : Type0010 Moduleで使用される拡張インターフェース仕様
      Usage of Type0010 Module(J).pdf : Type0010 Module を使用する上での注意事項
      Type0010 Sample Guide(J).pdf : サンプルプログラムの使用方法

    Binary Files
        Type0010 Module.bundle : Macintosh用 Type0010 Module本体 
        libNkPTPDriver.dylib : Macintosh用 PTP ドライバ
 
    Header Files
      Maid3.h : MAIDインターフェース基本ヘッダ
      Maid3d1.h : Type0010用MAIDインターフェース拡張ヘッダ
      NkTypes.h : 本プログラムで使用する型の定義
      NkEndian.h : 本プログラムで使用する型の定義
      Nkstdint.h : 本プログラムで使用する型の定義

    Sample Program
      Type0010CtrlSample(Mac) : Xcode 3.2.4用のサンプルプログラムプロジェクト


■制限事項
 本Module SDKを利用してコントロールできるカメラは1台のみです。
 複数台のコントロールには対応していません。
