{
  description = "PPISS — PlatformIO Development Environment (ESP8266)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      targetPkgs = pkgs: with pkgs; [
        platformio-core
        python3
        git
        gcc
        gnumake
        udev
        zlib
        ncurses
        stdenv.cc.cc.lib
        glibc
        libusb1
        openssl
        cacert
      ];

      profile = ''
        export PLATFORMIO_CORE_DIR="''${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"
        export SSL_CERT_FILE="${pkgs.cacert}/etc/ssl/certs/ca-bundle.crt"
      '';

      mkPioEnv = name: cmd: pkgs.buildFHSEnv {
        inherit name targetPkgs profile;
        runScript = pkgs.writeScript name ''
          #!/bin/bash
          set -e
          exec ${cmd}
        '';
      };

    in
    {
      devShells.${system}.default = (pkgs.buildFHSEnv {
        name = "ppiss-dev";
        inherit targetPkgs profile;
        runScript = "bash";
      }).env;

      packages.${system} = {
        default = mkPioEnv "ppiss-build"   "platformio run -e d1_mini";
        upload  = mkPioEnv "ppiss-upload"  "platformio run -e d1_mini --target upload";
        monitor = mkPioEnv "ppiss-monitor" "platformio device monitor";
      };
    };
}
