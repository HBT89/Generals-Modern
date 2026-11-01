# Sound Mod: Missile Launch and Fly-Away

Reference: [Patriot Missile Launch (Aviation Wall, YouTube)](https://www.youtube.com/watch?v=dcroMWXuLac)

**Goal:** A launch should sound like the reference. A hard ignition crack comes from the launcher. Then the rocket-motor roar follows the missile as it climbs and flies away. The roar gets quieter, darker in tone and lower in pitch with distance. Stock Generals plays one static "whoosh" at the launcher, and the missile itself makes no sound.

> **Note on the reference:** we couldn't pull the audio for spectral analysis because YouTube's bot check blocked the download. The breakdown below describes what a real Patriot (PAC-2/PAC-3, solid-fuel motor) launch sounds like on recordings of this kind. Confirm the timings against the clip by ear.
>
> **Licensing:** the video is copyrighted. **Do not rip its audio into the mod.** Use it only as a reference. Source the samples from CC0 or licensed libraries (rocket/RPG/firework motor recordings work well), or build them from layered synthesis.

---

## 1. What the reference sound is made of

| # | Phase | Time (approx.) | Character | Where it plays from |
|---|-------|----------------|-----------|---------------------|
| 1 | **Ignition / canister break** | 0 – 0.3 s | Sharp broadband crack/boom. Very fast attack and strong low end (thump around 40–120 Hz). | Launcher (fixed) |
| 2 | **Boost roar** | 0.2 – ~2 s | Loud, harsh, tearing noise with crackle (solid-propellant "popcorn" crackle in the 2–8 kHz range). Full-spectrum and close. | Missile (moving) |
| 3 | **Fly-away** | ~2 – 8 s | The same roar recedes. Pitch drops (Doppler as it accelerates away), highs roll off (air absorption) and volume falls. It turns into a hissing, rumbling "shhhhh". | Missile (moving) |
| 4 | **Tail / echo** | overlaps 2–3 | Low rumble and slap-back echoes off terrain. These come from the launch site, not from the missile. | Launcher (fixed) |

The key point: phases 1 and 4 are **anchored to the launcher**, and phases 2 and 3 **move with the missile**. Two separate sound sources produce the "launch, then it leaves" feeling. One sample can't do it.

---

## 2. What the engine already supports (no code changes)

I checked against the source in this repo:

- **`Weapon` → `FireSound`** (`GameEngine/Source/GameLogic/Object/Weapon.cpp:195`) plays once at the launcher. Use it for **phases 1 and 4**.
- **Projectile object → `SoundAmbient`** (`ThingTemplate.cpp:206`) is a looping sound attached to the missile's drawable. Every frame, `MilesAudioManager.cpp:2348` calls `AIL_set_3D_position` with the object's current position, so **the sound already travels with the missile**. It stops when the missile is destroyed. Use it for **phases 2 and 3**.
- **`MissileAIUpdate` → `IgnitionFX`** (`MissileAIUpdate.cpp:99, 486`) fires an FXList when the motor lights. An FXList can contain a `Sound` nugget, so this is an option for a separate ignition "pop" if `IgnitionDelay` is used.
- **Distance falloff:** `MinRange`/`MaxRange` per AudioEvent feed `AIL_set_3D_sample_distances` (`MilesAudioManager.cpp:2836`). The listener is a "microphone" placed between the ground at screen centre and the camera (`GameAudio.cpp:340-362`). A missile climbing off-screen therefore fades out naturally.

### Gaps (need engine work, section 4)
- **No Doppler.** No velocity is ever passed to Miles for game sounds. Only the unused WWAudio `Sound3D` path sets one.
- **No distance-based muffling.** `LowPassCutoff` is applied **once, at start, and only if the sound starts off-screen** (`MilesAudioManager.cpp:1331`). It is never updated as the missile flies away.
- **Hard cut-off.** Once a 3D sound drops below `MinSampleVolume`, it is killed outright (`MilesAudioManager.cpp:2334`), which can chop the fade tail.

---

## 3. Phase 1: INI-only implementation (can ship now)

### Audio assets to produce
| File | Length | Notes |
|------|--------|-------|
| `msl_launch_crack01..03.wav` | 1.5–3 s | Ignition crack plus low thump plus short echo tail. 3 variants. Mono, 22/44 kHz. |
| `msl_motor_loop01.wav` | 1–2 s, seamless loop | Boost roar with crackle, full spectrum. Mono. Loop point must be click-free. |
| `msl_motor_far_loop01.wav` *(optional)* | 1–2 s loop | Pre-filtered (low-passed around 2 kHz, darker) version for long range / off-screen. |

### `Data/INI/SoundEffects.ini` (or `WeaponSounds.ini`)
```ini
AudioEvent MissileLaunchCrack
  Sounds = msl_launch_crack01 msl_launch_crack02 msl_launch_crack03
  Control = random
  Volume = 95
  VolumeShift = -10
  PitchShift = -6 6
  Limit = 3
  Priority = high
  Type = world shrouded everyone
  MinRange = 250
  MaxRange = 1100
End

AudioEvent MissileMotorLoop
  Sounds = msl_motor_loop01
  Control = loop
  Volume = 85
  PitchShift = -4 4
  Limit = 6
  Priority = normal
  Type = world shrouded everyone
  MinRange = 150            ; full volume close to the missile
  MaxRange = 1400           ; long range so the fly-away tail is audible
  LowPassCutoff = 35        ; muffled when the missile is off-screen
End
```

### Weapon (e.g. `Data/INI/Weapon.ini`, Patriot / SAM / S-500 / MAD rockets)
```ini
Weapon PatriotMissileWeapon
  ...
  FireSound = MissileLaunchCrack       ; launcher-anchored phases 1 and 4
  ProjectileObject = PatriotMissile
  ...
End
```

### Projectile object (e.g. `Data/INI/Object/...`)
```ini
Object PatriotMissile
  ...
  SoundAmbient = MissileMotorLoop      ; travels with the missile (phases 2 and 3)
  ...
End
```

**Expected result:** a crack at the launcher, then a roar that pans and fades with the missile and stops on impact (the detonation sound takes over). That gets about 70% of the reference with zero code.

---

## 4. Phase 2: engine work for the full effect

All changes go in `GameEngineDevice/Source/MilesAudioDevice/MilesAudioManager.cpp` plus new optional AudioEvent fields in `INIAudioEventInfo.cpp` / `AudioEventInfo.h`. Existing INI keeps working.

### 4.1 Doppler (pitch drop as it flies away)
- Store the previous position on each `PlayingAudio` 3D entry.
- In the per-frame position update (around line 2348), compute `velocity = (pos - prevPos) / frameTime` and call `AIL_set_3D_velocity_vector(playing->m_3DSample, vx, vy, vz)`. The API is already in use in `WWAudio/Sound3D.cpp:399`.
- Set the listener velocity to 0 (or derive it from camera movement, damped).
- Add an AudioEvent field **`DopplerScale`** (default `0` = off, so stock sounds are unchanged). Game units and projectile speeds aren't real-world. Without scaling, a fast missile would warble wildly. Clamp the resulting pitch to about ±1 octave.

### 4.2 Distance-based air absorption (gets darker as it recedes)
- Add an AudioEvent field **`DistanceLowPass`** (percent; 0 = off).
- In the same per-frame loop, compute `t = clamp((dist - MinRange)/(MaxRange - MinRange))` and call `AIL_set_3D_sample_occlusion(sample, t * DistanceLowPass)` every frame, not just once in `initFilters3D`.
- Optional: crossfade `msl_motor_loop01` into `msl_motor_far_loop01` using `t`. Pre-filtered audio sounds better than runtime occlusion alone.

### 4.3 Graceful tail instead of hard cut
- When a moving 3D loop drops below `MinSampleVolume` (line 2334), fade it over about 250 ms rather than calling `releasePlayingAudio` immediately. Alternatively, let the event opt out of culling with a flag.

### 4.4 Optional: motor burnout
- Missiles with `FuelLifetime` stop thrusting before impact. Expose an optional `SoundMotorBurnout` / stop-ambient hook in `MissileAIUpdate` so the roar can end, or switch to a quiet whistle, when fuel runs out instead of lasting until detonation.

---

## 5. Where to use it in Generals: Modern

The launch/fly-away pair is generic. Make it the house style for every rocket-motor weapon, with per-weapon samples:

- GDP: Precision Cruise Missile (heavier, longer booster roar), MAD Rockets, laser-relay drone missiles
- ESF: S-500 AA Truck, DF-17 Strike Power, SAM Turrets
- ARC/FNC: Shahed-style truck launcher (launch crack plus a moped-like engine loop instead of a rocket roar)

(Note: `Mod Additions.md` says the Railgun replaces the Patriot for GDP. This treatment still applies to every other missile in the roster.)

## 6. Test checklist
- [ ] Launch heard at the launcher even when the missile leaves the screen immediately
- [ ] Roar pans left/right with the missile across the screen
- [ ] Roar fades over about 5–8 s for long-range shots and never clicks off abruptly
- [ ] Salvo of 4+ missiles doesn't exceed voice limits (`Limit`) or drown out unit voices
- [ ] Off-screen launches are muffled (`LowPassCutoff`)
- [ ] (Phase 2) Pitch drops on fly-away; no warble at very high projectile speeds
