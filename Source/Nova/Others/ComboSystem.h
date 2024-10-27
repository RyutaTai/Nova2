#pragma once

#include <string>
#include <vector>

struct ComboMove
{
    ComboMove(const std::string& name, InputType input, float frameWindow, float damage)
        : name(name), input(input), frameWindow(frameWindow), damage(damage) {}

    void AddNextMove(ComboMove* move)
    {
        nextMoves.push_back(move);
    }

    // 入力に基づいて次の技を取得
    ComboMove* GetNextMove(InputType playerInput)
    {
        for (auto& move : nextMoves) {
            if (move->input == playerInput) {
                return move;
            }
        }
        return nullptr;  // 適切な次の技がない場合、nullを返す
    }

public:
    std::string name;                    // コンボの技名
    InputType input;                     // 必要な入力（例：パンチ、キックなど）
    float frameWindow;                  // 技がキャンセルされ次に繋がる猶予フレーム（時間）
    float damage;                        // 技のダメージ
    std::vector<ComboMove*> nextMoves;   // 次の技へのポインタのリスト

};

class ComboSystem 
{
public:
    ComboSystem() : currentMove_(nullptr), elapsedTime_(0.0f) {}

    void SetComboSequence(ComboMove* firstMove)
    {
        currentMove_ = firstMove;
    }

    void Update(float deltaTime) 
    {
        if (currentMove_ == nullptr) return;  // 現在のコンボが存在しない場合、終了

        elapsedTime_ += deltaTime;  // 経過時間を増やす

        // 次の入力が猶予時間内にあるかどうかを確認
        if (elapsedTime_ <= currentMove_->frameWindow)
        {
            InputType playerInput = GetPlayerInput();  // プレイヤー入力を取得
            ComboMove* nextMove = currentMove_->GetNextMove(playerInput);

            if (nextMove != nullptr) 
            {
                // 正しい入力が検出された場合、次の技に遷移
                ExecuteMove(currentMove_);
                currentMove_ = nextMove;
                elapsedTime_ = 0.0f;
            }
        }
        else 
        {
            // 猶予時間を超えた場合、コンボ終了
            EndCombo();
        }
    }

    void ExecuteMove(ComboMove* move)
    {
        // 技を実行する処理（アニメーション、ダメージ処理など）
        std::cout << "Executing: " << move->name << ", Damage: " << move->damage << std::endl;
    }

    void EndCombo()
    {
        std::cout << "Combo ended." << std::endl;
        currentMove_ = nullptr;
    }

private:
    InputType GetPlayerInput()
    {
        // 実際のプレイヤー入力を取得する処理（仮）
        return InputType::Punch;  // 例: パンチ入力を検出
    }

private:
    ComboMove* currentMove_;  // 現在のコンボの技
    float elapsedTime_;       // 技が発動してからの経過時間

};

