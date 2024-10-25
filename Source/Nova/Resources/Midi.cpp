#include "Midi.h"

Midi::Midi(const std::string& midiFilename, const float& midiFileDurationSeconds)
{
    //  midi読み込み
    _ASSERT_EXPR(midiFile_.read(midiFilename), L"midiFile loading is failed.");

    //  midiファイルの長さ設定
    midiFileDurationSeconds_ = midiFileDurationSeconds;

}

//	初期化処理
void Midi::Initialize()
{

}

//	終了処理
void Midi::Finalize()
{

}

//	更新処理
void Midi::Update(const float& elpasedTime)
{
    UpdateCurrentTimer(elpasedTime);    //  タイマー更新

}

//  タイマー更新
void Midi::UpdateCurrentTimer(const float& elapsedTime)
{
    //  midiのタイマー更新
    currentTimer_ += elapsedTime;
    if (currentTimer_ > midiFileDurationSeconds_)    //  ファイルの全体の時間[s]を超えたら巻き戻す
    {
        currentTimer_ = 0.0f;
    }
}

//	入力されたタイミングから、最も近いノートの開始時間を返す	
float Midi::GetNearMidiTime(const float& inputTime)
{
    float noteTime = 0.0f;
    float lastTimeSubtract = FLT_MAX;   //  過去に一番短かった時間の差
    float currentTimeSubtract = 0.0f;   //  Inputとnoteの時間の差(絶対値)

    // 全トラックのMIDIイベントからノートオンだけ取り出す
    for (int track = 0; track < GetMidiFile().getTrackCount(); ++track)
    {
        for (int event = 0; event < GetMidiFile()[track].size(); ++event)
        {
            if (GetMidiFile()[track][event].isNoteOn())     //  ノートオンだったら 
            {
                //  時間の差を求める
                currentTimeSubtract = std::fabs(GetMidiFile()[track][event].seconds - inputTime);

                //  一番短かった時間の差と比較
                if (currentTimeSubtract < lastTimeSubtract)
                {
                    lastTimeSubtract = currentTimeSubtract;
                    noteTime = GetMidiFile()[track][event].seconds;
                }
            }
        }
    }

    return noteTime;
}

//  引数で受け取った時間がノートオンかどうか
bool Midi::IsInputNoteOn(const float& time)
{
    bool isNoteOn = false;
    float lastTimeSubtract = FLT_MAX;   //  過去に一番短かった時間の差
    float currentTimeSubtract = 0.0f;   //  Inputとnoteの時間の差(絶対値)

    // 全トラックのMIDIイベントからノートオンだけ取り出す
    for (int track = 0; track < GetMidiFile().getTrackCount(); ++track)
    {
        for (int event = 0; event < GetMidiFile()[track].size(); ++event)
        {
            if (GetMidiFile()[track][event].isNoteOn())     //  ノートオンだったら 
            {
                //  時間の差を求める
                currentTimeSubtract = std::fabs(GetMidiFile()[track][event].seconds - time);

                //  一番短かった時間の差がなければ
                if (currentTimeSubtract < 0.17f)
                {
                    isNoteOn = true;
                }
                else
                {
                    isNoteOn = false;
                }

            }
        }
    }

    return isNoteOn;
}

//  現在の時間がノートオンかどうか
bool Midi::IsCurrentTimeNoteOn()
{
    return IsInputNoteOn(currentTimer_);
}

//  ノートを追加するメソッド
void Midi::AddNote(const EventType& eventType, const int& noteNumber, const float& time)
{
    MidiNote note;
    note.eventType_     = eventType;
    note.noteNumber_    = noteNumber;
    note.time_          = time;
    notes_.emplace_back(note);

}

//	デバッグ描画
void Midi::DrawDebug()
{

}
