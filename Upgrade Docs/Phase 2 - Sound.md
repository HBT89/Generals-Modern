# Phase 2 – Sound System Modernization (Planned)

## 🎯 Goal
Modernize and future-proof the audio subsystem for Command & Conquer: Generals Zero Hour by:
- Replacing all legacy and proprietary audio libraries (Miles Sound System, Asimp3) with open, cross-platform solutions
- Ensuring robust MP3 and PCM playback, streaming, and mixing
- Maintaining compatibility with existing audio abstractions (AudioDevice, AudioChannel, AudioSample)
- Preparing the engine for advanced audio features and modding

---

## ✅ Completed So Far
- Identified all usages of Asimp3 (MP3 decoding) and Miles6 (audio backend) in WPAudio and WWVegas
- Confirmed that all Asimp3 code is missing or commented out, but MP3 support is still referenced in the engine
- Documented all code locations where AUDIO_COMPRESS_MP3 and Miles Sound System APIs are used
- Assessed the structure of the audio pipeline to allow backend replacement with minimal changes

---

## 🧱 Core Feature Set

### 1. MP3 Decoding (Asimp3 Replacement)
- Use minimp3 as a drop-in decoder for all MP3 playback and streaming
- Implement a thin wrapper (e.g., `mp3decode_minimp3.cpp/h`) that mimics the old Asimp3 API (MP3_transfer, etc.)
- Ensure seamless PCM output for the engine's existing audio pipeline

### 2. Audio Backend (Miles6 Replacement)
- Use OpenAL Soft as the new cross-platform audio backend
- Implement a new backend (e.g., `WPAudio_OpenAL.cpp/h`) that provides the same device/channel/sample API as the old code
- Route all playback, mixing, and streaming through OpenAL Soft

### 3. Minimal Disruption
- Retain all existing AudioDevice/AudioChannel/AudioSample abstractions
- Only swap out the backend and decoder logic, minimizing changes to higher-level engine code

---

## 🛠 Implementation Tasks

| Task | File | Type |
|------|------|------|
| Remove all Asimp3 code and references | WPAudio/* | Engine source |
| Add minimp3 and wrapper for MP3 decoding | WPAudio/mp3decode_minimp3.cpp/h | Engine source |
| Replace all MP3_transfer and related logic with minimp3-based code | WPAudio/* | Engine source |
| Remove all Miles6 code and references | WWVegas/Miles6/*, WWAudio/* | Engine source |
| Add OpenAL Soft backend and wrapper | WPAudio_OpenAL.cpp/h | Engine source |
| Replace device/channel/sample logic to use OpenAL Soft | WPAudio/* | Engine source |
| Test and debug all audio playback and streaming | All | Engine source |
| Update documentation and developer notes | Upgrade Docs/* | Docs |

---

## 🚧 General TODO Plan (Sound System Migration)

1. **MP3 Decoder Replacement**
   - Remove all Asimp3 includes, build steps, and code
   - Add minimp3 to the project and implement a wrapper that provides the same interface as the old MP3_transfer and related functions
   - Update all code paths that reference AUDIO_COMPRESS_MP3 to use the new minimp3-based decoder

2. **Audio Backend Replacement**
   - Remove all Miles6 and proprietary audio backend code
   - Add OpenAL Soft to the project and implement a backend that provides the same device/channel/sample API
   - Update all device/channel/sample logic to use OpenAL Soft for playback, mixing, and streaming

3. **Testing and Integration**
   - Test MP3 and PCM playback, streaming, and mixing in isolation
   - Integrate new decoder and backend, and test the full audio pipeline
   - Remove all obsolete code and update documentation

---

## ✅ Completion Criteria

- Game launches and plays all audio (MP3, PCM, streaming) correctly
- No missing symbols or build errors related to Asimp3 or Miles6
- All audio resource management is handled by OpenAL Soft and minimp3 (no proprietary code remains)
- No crash on startup, shutdown, or audio device change
- Audio system is cross-platform and ready for future enhancements

---

## 🔒 Future Enhancements

- Add config UI for advanced audio settings
- Implement 3D positional audio and environmental effects
- Add support for additional codecs (OGG, FLAC, etc.)
- Expand OpenAL backend for advanced features (reverb, filters, etc.)


