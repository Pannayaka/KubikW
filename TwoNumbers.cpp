#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

const double bpm = 120.0;
const double beat = 60.0 / bpm;   // ~0.857 sec
const double bar = beat * 4;      // ~3.43 sec

// play a chord for one bar
void playChordOnBeat(const std::string& chord) {
    std::string cmd = "play -n synth " + std::to_string(bar) +
                      " " + chord + " >/dev/null 2>&1";
    system(cmd.c_str());
}

// metronome runs (strong click on beat 1, weak on 2–4)
void metronome(int totalBars) {
    int totalBeats = totalBars * 4;
    for (int i = 0; i < totalBeats; i++) {
        if (i % 4 == 0)
            system("play -n synth 0.05 sine 1500 >/dev/null 2>&1"); // strong click
        else
            system("play -n synth 0.05 sine 1000 >/dev/null 2>&1"); // weak click
        std::this_thread::sleep_for(std::chrono::duration<double>(beat));
    }
}

int main() {
    const int barsPerProgression = 5;
    const int repeats = 3;
    const int totalBars = barsPerProgression * repeats;

    

    
    std::this_thread::sleep_for(std::chrono::duration<double>(0.05));
    for (int r = 0; r < repeats; r++) {
        playChordOnBeat("sine 261.63 sine 329.63 sine 392.00"); // C
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        playChordOnBeat("sine 329.63 sine 415.30 sine 493.88"); // E
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        playChordOnBeat("sine 440.00 sine 261.63 sine 329.63"); // Am
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string cmdF  = "play -n synth " + std::to_string(bar/2) +
                            " sine 349.23 sine 440.00 sine 261.63 >/dev/null 2>&1";
        std::string cmdFm = "play -n synth " + std::to_string(bar/2) +
                            " sine 349.23 sine 415.30 sine 261.63 >/dev/null 2>&1";
        system(cmdF.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(75));
        system(cmdFm.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(67));
    }
}
