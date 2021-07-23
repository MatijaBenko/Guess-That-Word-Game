// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    // TEXT() Macro Used to encode string variables to use in UE4
    Super::BeginPlay();

    // Concatenate The two Strings together ensuring a "/" inbetween them
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
    FFileHelper::LoadFileToStringArrayWithPredicate(Isograms, *WordListPath, [](const FString &Word)
                                                    { return Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word); });

    SetupGame();
}

void UBullCowCartridge::OnInput(const FString &PlayerInput) // When the player hits enter
{
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else
    {
        ProcessGuess(PlayerInput);
    }
}

void UBullCowCartridge::SetupGame()
{
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)];
    PlayerLife = HiddenWord.Len();
    bGameOver = false;

    // Welcoming Player
    PrintLine(TEXT("Welcome to the BullCow!"));
    PrintLine(TEXT("Guess the %i letter word!"), HiddenWord.Len());
    PrintLine(TEXT("You have %i of lives"), PlayerLife);
    PrintLine(TEXT("Type in your guess. \npress ENTER to continue. . ."));
    
    // PrintLine(TEXT("The HiddenWord is: %s"), *HiddenWord); //Debug Line
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nPress enter to play again."));
}

void UBullCowCartridge::ProcessGuess(const FString &PlayerGuess)
{

    if (PlayerGuess == HiddenWord)
    {
        PrintLine(TEXT("You have Won!"));
        EndGame();
        return;
    }

    if (PlayerGuess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("Sorry, try guessing again! \nYou have %i lives remaing."), PlayerLife);
        PrintLine(TEXT("The hidden word is %i letters long."), HiddenWord.Len());
        return;
    }

    if (!IsIsogram(PlayerGuess))
    {
        PrintLine(TEXT("No repeating letters, guess again!"));
        return;
    }

    // Life Remove
    PrintLine(TEXT("You lost a life!"));
    --PlayerLife;

    if (PlayerLife <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("You have no lives left!"));
        PrintLine(TEXT("The hidden word was: %s"), *HiddenWord);

        EndGame();
        return;
    }
    FBullCowCount Score = GetBullCows(PlayerGuess);

    PrintLine(TEXT("You have %i Bulls and %i Cows"), Score.Bulls, Score.Cows);

    PrintLine(TEXT("Guess again, you have %i lives left."), PlayerLife);
}

bool UBullCowCartridge::IsIsogram(const FString &PlayerGuess)
{
    for (int32 Index = 0; Index < PlayerGuess.Len(); Index++)
    {
        for (int32 Comparison = Index + 1; Comparison < PlayerGuess.Len(); Comparison++)
        {
            if (PlayerGuess[Index] == PlayerGuess[Comparison])
            {
                return false;
            }
        }
    }
    return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString> &WordList) const
{
    TArray<FString> ValidWords;

    for (FString Word : WordList)
    {
        if (Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word))
        {
            ValidWords.Emplace(Word);
        }
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString &PlayerGuess) const
{
    FBullCowCount Count;

    for (int32 GuessIndex = 0; GuessIndex < PlayerGuess.Len(); GuessIndex++)
    {
        if (PlayerGuess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls++;
            continue;
        }

        for (int32 HiddenIndex = 0; HiddenIndex < HiddenWord.Len(); HiddenIndex++)
        {
            if (PlayerGuess[GuessIndex] == HiddenWord[HiddenIndex])
            {
                Count.Cows++;
                break;
            }
        }
    }
    return Count;
}