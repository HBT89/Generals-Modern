# Generals: Modern – Full Proposed Design Overview

## 🔧 Engine and Modernization Changes (Phase 1–2)
- Replace DirectX 8 with **BGFX**
- Support for **1440p/4K** and ultrawide resolutions
- UI and HUD scaling fixes (`ControlBarScheme.ini`, `ControlBarResizer.ini`)
- Enhanced camera and scroll behavior (`GameData.ini`)
- Support for **mass-building control** (rally points, upgrades)
- Shader system migrated to `wwshade2bgfx`
- `shaderc.exe` compiled and shaders reauthored
- Hotkeys, input and windowed/fullscreen stability

---

## 🧠 Gameplay Modernization

### ✔️ Tactical Shifts
- **Higher unit value**: discourages early rushing
- **Longer games**, more complex tech
- Smaller, smarter armies

### ✔️ Civilian Mechanics
- Civilian vehicles: troop transports, disguises, potential weapons
- Civilian casualties may be counted for score/balance/faction perks

---

## 🛡️ Factions & Doctrines

### **GDP – Global Defense Pact**
- Precision & tech
- **Doctrines**: Advanced Tech, Naval Warfare, Land Warfare
- Units:
  - F-47 Fighter (new)
  - VTOL Pad (aircraft dock)
  - Drone Printer (Advanced Tech)
  - Mobile Offshore Base (Naval)
  - Laser Anti-Drone Systems
  - MAD Rockets (Modular Anti-Drone)
  - Laser Relay Drone
  - Remote Weapon Kits (turret deployers)

### **ESF – Eurasian Strategic Front**
- Disruption & brute force
- **Doctrines**: Cyberwarfare, Land, Strategic Weapons
- Units:
  - TU-160 clone “Boomer Strike”
  - S-500 AA Truck
  - DF-17 Strike Power
  - Sonic Tank (anti-troop, slows vehicles)
  - Unjammable Kamikaze Drone
  - Anti-air Infantry Truck
  - SAM Turrets
  - Drone-Manhack Launcher

### **ARC / FNC – Autonomous Resistance Collective**
- Hybrid rebel tactics
- **Doctrines**: Rapid Deployment, Black Market, Cyber Theft
- Units:
  - Kamikaze Drone Swarm (Superweapon)
  - Shahed-style 4-pack Truck Launcher
  - Civilian vehicle disguises
  - Decoy Army Spawner (stealth building)
  - Drone Factory + Reload Behavior
  - Angry Mob variants
  - Drone Printer (pending doctrine placement)

---

## ⚔️ Superweapons

| Faction | Superweapon                          | Notes |
|---------|--------------------------------------|-------|
| GDP     | Precision Cruise Missile (3 charges) | High-accuracy strike |
| GDP     | Golden Dome                          | Area denial, destroys all air/drones |
| ESF     | EMP Storm                            | Temporary disable + blackout |
| FNC     | Drone Swarm                          | Like China artillery, long duration |
| FNC     | Underground Logistics Grid           | Spawns ambush units |

---

## 🎯 Upgrades and Mechanics

- MAD Rockets purchasable at Airfield, applied per helicopter
- Rocket troops become fast-launch RPG or LAW type (not slow trails)
- Railgun replaces Patriot for GDP
- Units can abandon vehicles randomly (new morale mechanic)
- Infantry can deploy **remote weapon kits** (MG/sniper depending on faction)
- Squad-like infantry logic inspired by C&C3
- Random infantry skin tones and voice variants (variant objects)
- Drone relay system supports **LOS targeting** via deployable drone
- Drone loiterer can **fire missiles until ammo depleted**, then despawn
- Spawn behavior: drones ascend above camera and disappear

---

## 🚢 Naval System

- Platforms are buildings on water:
  - Launch air units
  - Defend with AA / MAD / Patriot Laser upgrade
  - Some produce drones
- Used to extend bases on **island-only or water-heavy maps**
- **Mobile Offshore Base** (GDP): acts like a forward base
- Naval map tag (`NavalEnabled`) required
- ESF and ARC can harass naval with drones and missiles

---

## 📋 Future / Experimental Ideas

- New scoring system tracking civilian deaths
- Dynamic income theft powers (hacks that steal money)
- Strategic Points (zone-based control)
- Tech Tree replacing General Powers UI
- Multi-tier doctrine unlocks
- Smart fog-of-war + recon system (relay drones, mobile radar)
- Faction-specific announcers or UI skins
- AI that mimics players' behavior more contextually (inspired by C&C3)

