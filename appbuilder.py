import os
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
# Front-end source files
# --------------------------------------------------

frontend_dir = os.path.join(project_root, "front-end")
output_exe = os.path.join(project_root, "output.exe")

frontend_source_files = []

if os.path.exists(frontend_dir):
    for filename in os.listdir(frontend_dir):
        if filename.endswith(".cpp"):
            frontend_source_files.append(
                os.path.join(frontend_dir, filename)
            )

cmd = [
    compiler,
    "-std=c++17"
] + frontend_source_files + [
    f"-I{frontend_dir}"
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

imgui_backends_inc = os.path.join(
    project_root,
    "libraries",
    "imgui",
    "backends"
)

cmd.append(f"-I{imgui_inc}")
cmd.append(f"-I{imgui_backends_inc}")

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

if "opengl32" not in sdl3_cfg.get("libraries", []):
    cmd.append("-lopengl32")

if "ws2_32" not in sdl3_cfg.get("libraries", []):
    cmd.append("-lws2_32")

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
    for src_file in frontend_source_files:
        print("  ->", src_file)

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

# --------------------------------------------------
# Main
# --------------------------------------------------

if __name__ == "__main__":
    build()