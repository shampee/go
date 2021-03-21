with import <nixpkgs> {};

let
    stdenv8 = overrideCC stdenv gcc8;
in
    stdenv8.mkDerivation rec {
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
        ];
    }
