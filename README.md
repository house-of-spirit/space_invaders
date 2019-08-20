# Space Invaders

Space Invaders is a 1978 arcade game created by Tomohiro Nishikado. It was manufactured and sold by Taito in Japan, and licensed in the United States by the Midway division of Bally. Within the shooter genre, Space Invaders was the first fixed shooter and set the template for the shoot 'em up genre. The goal is to defeat wave after wave of descending aliens with a horizontally moving laser to earn as many points as possible.

This project is an attempt at achieving full emulation of the original space invaders arcade.

# The Arcade

The Space Invaders arcade runs an 8080 clocked at 2.0MHz. The graphics are rendered onto a 256x224 display, which is looked at with an angled mirror. 

![The Space Invaders arcade](https://www.tintoyarcade.com/image/cache/data/product/Images_5600_5699/TTA5651-Space-Invaders-TIny-Arcade-01-1000x1000.jpg)

# Emulation

This emulator utilizes classical interpreted emulation. There is no apparent need for fancy techniques like caching and JIT, because the original processor runs an 8080 clocked at 2.0MHz, which is tremendously much slower than consumer CPUs today. 

As of now, this emulator contains these features:

- [x] Full code emulation
- [x] Powerful debugger
- [x] Disassembler
- [x] Debug info support
- [x] Mid-screen and VBLANK interrupts.
- [ ] Graphics rendering
- [ ] Sound
- [ ] Input Handling
