with import <nixpkgs> {};

let
    stdenv10 = overrideCC stdenv gcc10;
in
    stdenv10.mkDerivation rec {
        name = "go";
        env = buildEnv {
            name = name;
            paths = buildInputs;
        };
        buildInputs = [
            pkgconfig
            SDL2
            SDL2_image
            SDL2_ttf
            SDL2_mixer
            clang-tools
            valgrind
        ];
    }
