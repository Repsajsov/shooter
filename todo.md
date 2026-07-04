# Shooter project — roadmap

## Fase 0 — Techniek-fundering
- [x] Window + Camera3D
- [x] Mouse-look (yaw/pitch)
- [x] WASD movement (relatief aan kijkrichting, cross-product voor right-vector)
- [ ] Schieten (raycasting) — bezig

## Fase 1 — Kern-loop compleet
- [ ] Fire-rate/cooldown op schieten
- [ ] Meerdere targets (array i.p.v. losse struct)
- [ ] Health op targets i.p.v. instant-kill
- [ ] Player health + damage nemen

## Fase 2 — Eerste echte enemy
- [ ] Enemy-struct met state (idle/aggro/dead)
- [ ] Grunt: simpele AI, loopt naar speler toe
- [ ] Damage aan speler bij dichtbij komen
- [ ] Checkpoint: eerste "playable combat loop"

## Fase 3 — Movement-beslissing (NOG TE BESLISSEN)
- [ ] Kiezen: movement-depth (beperkt/tactisch vs. sustained-escape)
  - Overweging: sustained mobility (air-strafe/bunny hop) ondermijnt aim-focus
  - Richting: korte-burst movement, geen momentum-stacking
- [ ] Jump + gravity
- [ ] Dash (cooldown-based)
- [ ] FOV-punch bij dash/sprint (lerp naar hogere fovy)

## Fase 4 — Aim-diversiteit roster (Voltaic-geïnspireerd, organisch verweven in lore)
- [ ] Strafer — tracking-type, beweegt in boog om speler heen
- [ ] Swarmling — flick-type, ring-spawn rond speler (progressive ring bij intro)
- [ ] Sentinel — precisie/weak-point-type, groot met klein kwetsbaar punt
- [ ] Elke enemy: silhouette + kleur + fictie-reden (geen "trainingswidget"-gevoel)

## Fase 5 — Wapens & ammo
- [ ] Tweede wapen
- [ ] Ammo-schaarste per wapen (géén reload — bewuste keuze)
- [ ] Wapen-switch input

## Fase 6 — Eerste level
- [ ] Arena groter dan test-box, met verticaliteit
- [ ] Spawn-systeem (triggers/volgorde)
- [ ] Level 1: 4x Grunt (fodder) + 1x Strafer (introductie)

## Fase 7 — Progressie
- [ ] Level 2: Grunt+Strafer + nieuwe Swarmling
- [ ] Level 3: alles + Sentinel (climax-arena)
- [ ] Patroon herhalen: nieuw → combineer → climax

## Fase 8 — Polish & performance
- [ ] Instanced rendering (DrawMeshInstanced voor veel enemies)
- [ ] Fog (sfeer + occlusion-tool, moeras-thema)
- [ ] PS1 vertex-wobble shader
- [ ] Low-res render target

## Fase 9 — Lore/wereld
- [ ] Setting: moeras + verdronken cultus/godheid
- [ ] Enemy-visuals aanpassen aan lore (amfibisch/verrot i.p.v. generieke cubes)

## Design-principes (niet vergeten)
- Geen reloads — non-stop actie
- Aim-diversiteit moet organisch aanvoelen, nooit als "Kovaak's-les"
- Movement = spektakel/traversal, niet als primaire ontsnapping aan aim-druk
- Elke nieuwe enemy krijgt eigen introductie-moment voor hij gemixt wordt
