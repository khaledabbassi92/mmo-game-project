import os
import sys
import json
import subprocess

# --------------------------------------------------
# Paths & Configuration
# --------------------------------------------------

project_root = os.path.abspath(os.path.dirname(__file__))
config_path = os.path.join(project_root, "libconfig.json")

with open(config_path, "r") as f:
    config = json.load(f)

compiler = "g++"

# --------------------------------------------------
# Front-end source files (ALL 3 FILES)
# --------------------------------------------------

frontend_dir = os.path.join(project_root, "front-end")

main_file  = os.path.join(frontend_dir, "main.cpp")
gui_file   = os.path.join(frontend_dir, "gui.cpp")
world_file = os.path.join(frontend_dir, "world.cpp")  # <--- WAS MISSING!

output_exe = os.path.join(project_root, "output.exe")

# Base command with C++17 and all source files
cmd = [
    compiler,
    "-std=c++17",
    main_file,
    gui_file,
    world_file,                                       # <--- Added
    f"-I{frontend_dir}"                               # <--- Include core.h
]

# --------------------------------------------------
# Libraries configuration
# --------------------------------------------------

libs_config = config.get("libraries", {})

# --------------------------------------------------
# SDL3
# --------------------------------------------------

sdl3_cfg = libs_config.get("SDL3", {})

sdl3_inc = os.path.join(
    project_root,
    "libraries",
    "SDL3",
    sdl3_cfg.get("include", "include")
)

sdl3_lib = os.path.join(
    project_root,
    "libraries",
    "SDL3",
    sdl3_cfg.get("lib", "lib")
)

cmd.append(f"-I{sdl3_inc}")

# --------------------------------------------------
# Dear ImGui
# --------------------------------------------------

imgui_cfg = libs_config.get("imgui", {})

imgui_inc = os.path.join(
    project_root,
    "libraries",
    "imgui",
    imgui_cfg.get("include", ".")
)
imgui_backends_inc = os.path.join(project_root, "libraries", "imgui", "backends")

cmd.append(f"-I{imgui_inc}")
cmd.append(f"-I{imgui_backends_inc}")                 # <--- Fixes imgui_impl_sdl3.h not found

# Add ImGui source files
for src in imgui_cfg.get("source", []):
    full_src = os.path.join(
        project_root,
        "libraries",
        "imgui",
        src
    )
    cmd.append(full_src)

# --------------------------------------------------
# SDL3 library directory & linker flags
# --------------------------------------------------

cmd.append(f"-L{sdl3_lib}")

for lib in sdl3_cfg.get("libraries", []):
    cmd.append(f"-l{lib}")

# Ensure opengl32 is linked on Windows
if "opengl32" not in sdl3_cfg.get("libraries", []):
    cmd.append("-lopengl32")

# --------------------------------------------------
# Output executable
# --------------------------------------------------

cmd.extend([
    "-o",
    output_exe
])

# --------------------------------------------------
# Build Execution
# --------------------------------------------------

def build():
    print("=" * 60)
    print("MMO CLIENT BUILD SYSTEM")
    print("=" * 60)
    print("Project root:", project_root)
    print("\nSource files:")
    print("  ->", main_file)
    print("  ->", gui_file)
    print("  ->", world_file)
    print("\nCompiler command:\n", " ".join(cmd))
    print("-" * 60)

    result = subprocess.run(cmd)
    print("-" * 60)

    if result.returncode == 0:
        print("BUILD SUCCESSFUL.")
        print(f"Executable: {output_exe}")
        print("=" * 60)
        subprocess.Popen([output_exe])
    else:
        print(f"BUILD FAILED (Exit code: {result.returncode})")
        print("=" * 60)

if __name__ == "__main__":
    build()