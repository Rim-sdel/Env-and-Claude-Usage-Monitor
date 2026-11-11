{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };

        fhs = pkgs.buildFHSEnv {
          name = "pio-env";
          targetPkgs =
            pkgs: with pkgs; [
              platformio
              clang-tools
              gcc
              glibc
              zlib
              python3
              git
              zsh
            ];
          profile = ''
            export PLATFORMIO_CORE_DIR="$PWD/.platformio"
          '';
          runScript = "zsh";
        };
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = [ fhs ];
          shellHook = ''
                      if [ -z "$IN_PIO_ENV" ]; then
            			export IN_PIO_ENV=1
            			echo "entering pio-env for FHS compat"
            			exec pio-env
            		  fi
          '';
        };
      }
    );
}
