# VC版本录音工具

这是一个利用VC开发的版本录音并转文字并翻译为英语工具。

## 特点

- **声音录取：** 使用`portaudio`从话筒录取声音。
- **环形缓冲区：** 将录取的声音存放到一个环形缓冲区中。
- **语音数据处理：** 开启一个新的线程，从环形缓冲区读取30毫秒的语音数据。
- **静音检测：** 通过`webrtcvad`判断该30毫秒的语音是否是静音。如果是静音，则丢弃；不是静音则保存到新的缓冲区中。
- **数据发送：** 直到缓冲区中放满1秒的语音数据（或有一段语音后跟着一段静音，认为这前面的语音在这里中断），就发送到百度，使用`easywsclient`作为websocket工具，将语音转为文字，并翻译为英文语音。

## 百度接口使用说明

为了使用百度的接口，你首先需要一个百度的账号，请按照以下步骤替换你的账号信息：

```cpp
doc.AddMember("app_id", "xxxxxxxx", doc.GetAllocator());
doc.AddMember("app_key", "*************************", doc.GetAllocator());
```

## 编译与运行

请使用**Visual Studio 2022**打开工程，编译并运行。
