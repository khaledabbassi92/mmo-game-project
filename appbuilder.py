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

# Separate lists to collect compiler flags vs linker flags
include_flags = []
source_files = []
linker_flags = []

# --------------------------------------------------
# Dynamic Libraries Configuration
# --------------------------------------------------

libs_config = config.get("libraries", {})

for lib_name, lib_data in libs_config.items():
    lib_dir = os.path.join(project_root, "libraries", lib_name)
    
    # Add Include Directory (-I)
    inc_rel_path = lib_data.get("include", ".")
    inc_full_path = os.path.normpath(os.path.join(lib_dir, inc_rel_path))
    include_flags.append(f"-I{inc_full_path}")

    # Add ImGui Backends Include if imgui
    if lib_name.lower() == "imgui":
        backends_inc = os.path.join(lib_dir, "backends")
        if os.path.exists(backends_inc):
            include_flags.append(f"-I{backends_inc}")

    # Add Source Files (.cpp)
    for src in lib_data.get("source", []):
        full_src = os.path.normpath(os.path.join(lib_dir, src))
        source_files.append(full_src)

    # Add Library Link Path (-L)
    if "lib" in lib_data:
        lib_path = os.path.normpath(os.path.join(lib_dir, lib_data["lib"]))
        linker_flags.append(f"-L{lib_path}")

    # Add Linker Flags (-l)
    for lib_flag in lib_data.get("libraries", []):
        linker_flags.append(f"-l{lib_flag}")

# Default System Linkers
default_system_libs = ["-lopengl32", "-lws2_32"]

# Assemble command in correct order: Compiler -> Sources -> Includes -> Linker flags -> Output
cmd.extend(source_files)
cmd.extend(include_flags)
cmd.extend(linker_flags)
cmd.extend(default_system_libs)

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
    for src_file in frontend_source_files + source_files:
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