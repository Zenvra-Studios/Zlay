#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE_DIR = ROOT / "zlay"
SHADER_DIR = ROOT / "shader"
VALID_PRESETS = ("debug", "release")
VALID_BACKENDS = ("auto", "x11", "gtk", "win32", "winrt", "cocoa")


def relative_path(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT))
    except ValueError:
        return str(path)


def cmake_bool(value: bool) -> str:
    return "ON" if value else "OFF"


def build_type_from_preset(preset: str) -> str:
    if preset == "release":
        return "Release"
    return "Debug"


def default_build_dir(preset: str, backend: str) -> Path:
    if backend == "auto":
        return ROOT / "build" / preset
    return ROOT / "build" / f"{preset}-{backend}"


def run_command(command: list[str], *, cwd: Path = ROOT) -> None:
    print("+ " + " ".join(command), flush=True)
    completed = subprocess.run(command, cwd=str(cwd), check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def expected_layout_files(args: argparse.Namespace) -> list[Path]:
    files = [
        ROOT / "CMakeLists.txt",
        ROOT / "zlay.h",
        SOURCE_DIR / "zlay_arena.c",
        SOURCE_DIR / "zlay_core.c",
        SOURCE_DIR / "zlay_hash.c",
        SOURCE_DIR / "zlay_layout.c",
        SOURCE_DIR / "zlay_render.c",
        SOURCE_DIR / "zlay_style.c",
        SOURCE_DIR / "zlay_internal.h",
        SOURCE_DIR / "zlay_simd.h",
        SOURCE_DIR / "components" / "button.h",
        SOURCE_DIR / "components" / "input.h",
        SOURCE_DIR / "components" / "label.h",
    ]

    if args.renderer:
        files += [
            SOURCE_DIR / "renderer" / "zlay_renderer.c",
            SOURCE_DIR / "renderer" / "zlay_renderer.h",
        ]

    if args.drivers:
        files += [
            SOURCE_DIR / "driver" / "opengl" / "zlay_opengl.c",
            SOURCE_DIR / "driver" / "opengl" / "zlay_opengl.h",
            SOURCE_DIR / "driver" / "vulkan" / "zlay_vulkan.c",
            SOURCE_DIR / "driver" / "vulkan" / "zlay_vulkan.h",
        ]

    if args.os:
        files += [
            SOURCE_DIR / "os" / "zlay_os.h",
            SOURCE_DIR / "os" / "libx11" / "zlay_impl_x11.c",
            SOURCE_DIR / "os" / "libx11" / "zlay_impl_x11.h",
            SOURCE_DIR / "os" / "gtk" / "zlay_impl_gtk.c",
            SOURCE_DIR / "os" / "gtk" / "zlay_impl_gtk.h",
            SOURCE_DIR / "os" / "win32" / "zlay_impl_win32.c",
            SOURCE_DIR / "os" / "win32" / "zlay_impl_win32.h",
            SOURCE_DIR / "os" / "winrt" / "zlay_impl_winrt.c",
            SOURCE_DIR / "os" / "winrt" / "zlay_impl_winrt.h",
            SOURCE_DIR / "os" / "cocoa" / "zlay_impl_cocoa.c",
            SOURCE_DIR / "os" / "cocoa" / "zlay_impl_cocoa.h",
        ]

    if args.shaders:
        files += [
            SOURCE_DIR / "zlay_antialiasing.c",
            SOURCE_DIR / "zlay_antialiasing.h",
            ROOT / "scripts" / "embed_shaders.py",
            SHADER_DIR / "common" / "antialiasing.glsl",
            SHADER_DIR / "common" / "antialiasing.hlsl",
            SHADER_DIR / "common" / "antialiasing.slang",
            SHADER_DIR / "common" / "antialiasing.wgsl",
            SHADER_DIR / "glsl" / "rect.vert.glsl",
            SHADER_DIR / "glsl" / "rect.frag.glsl",
            SHADER_DIR / "vulkan" / "rect.vert.glsl",
            SHADER_DIR / "vulkan" / "rect.frag.glsl",
            SHADER_DIR / "webgl" / "rect.vert.glsl",
            SHADER_DIR / "webgl" / "rect.frag.glsl",
            SHADER_DIR / "wgsl" / "rect.vert.wgsl",
            SHADER_DIR / "wgsl" / "rect.frag.wgsl",
            SHADER_DIR / "hlsl" / "rect.vert.hlsl",
            SHADER_DIR / "hlsl" / "rect.frag.hlsl",
            SHADER_DIR / "slang" / "rect.vert.slang",
            SHADER_DIR / "slang" / "rect.frag.slang",
        ]

    return files


def check_layout(args: argparse.Namespace, *, verbose: bool = False) -> None:
    missing = [path for path in expected_layout_files(args) if not path.exists()]
    if missing:
        lines = "\n".join(f"  - {relative_path(path)}" for path in missing)
        raise SystemExit(f"ZLay source layout is incomplete. Missing files:\n{lines}")

    if verbose:
        print(f"Layout OK: {relative_path(SOURCE_DIR)}/")


def configure(args: argparse.Namespace, build_dir: Path) -> None:
    command = [
        "cmake",
        "-S",
        str(ROOT),
        "-B",
        str(build_dir),
        "-G",
        args.generator,
        f"-DCMAKE_BUILD_TYPE={build_type_from_preset(args.preset)}",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        f"-DZLAY_BUILD_EXAMPLES={cmake_bool(args.examples)}",
        f"-DZLAY_BUILD_DRIVERS={cmake_bool(args.drivers)}",
        f"-DZLAY_BUILD_RENDERER={cmake_bool(args.renderer)}",
        f"-DZLAY_BUILD_SHADERS={cmake_bool(args.shaders)}",
        f"-DZLAY_BUILD_OS={cmake_bool(args.os)}",
        f"-DZLAY_OS_BACKEND={args.backend}",
    ]
    run_command(command)


def build(args: argparse.Namespace, build_dir: Path) -> None:
    command = ["cmake", "--build", str(build_dir)]
    if args.parallel:
        command.append(f"-j{args.parallel}")
    else:
        command.append("-j")
    run_command(command)


def run_example(build_dir: Path) -> None:
    exe_name = "zlay_example_basic.exe" if os.name == "nt" else "zlay_example_basic"
    exe = build_dir / exe_name
    if not exe.exists():
        raise SystemExit(f"Example executable not found: {exe}")
    run_command([str(exe)])


def clean(build_dir: Path) -> None:
    if build_dir.exists():
        print(f"Removing {build_dir}")
        shutil.rmtree(build_dir)
    else:
        print(f"Nothing to clean: {build_dir}")


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="ZLay build helper for CMake, examples, and platform backend checks."
    )
    parser.add_argument(
        "action",
        nargs="?",
        default="all",
        choices=("doctor", "configure", "build", "run", "all", "clean"),
        help="Action to perform. Default: all.",
    )
    parser.add_argument("--preset", choices=VALID_PRESETS, default="debug")
    parser.add_argument("--backend", choices=VALID_BACKENDS, default="auto")
    parser.add_argument("--build-dir", type=Path, default=None)
    parser.add_argument("--generator", default="Ninja")
    parser.add_argument("-j", "--parallel", type=int, default=0)

    parser.add_argument("--examples", dest="examples", action="store_true", default=True)
    parser.add_argument("--no-examples", dest="examples", action="store_false")
    parser.add_argument("--drivers", dest="drivers", action="store_true", default=True)
    parser.add_argument("--no-drivers", dest="drivers", action="store_false")
    parser.add_argument("--renderer", dest="renderer", action="store_true", default=True)
    parser.add_argument("--no-renderer", dest="renderer", action="store_false")
    parser.add_argument("--shaders", dest="shaders", action="store_true", default=True)
    parser.add_argument("--no-shaders", dest="shaders", action="store_false")
    parser.add_argument("--os", dest="os", action="store_true", default=True)
    parser.add_argument("--no-os", dest="os", action="store_false")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    build_dir = (args.build_dir or default_build_dir(args.preset, args.backend)).resolve()

    if args.drivers and not args.renderer:
        raise SystemExit("--drivers requires --renderer")

    if args.action == "clean":
        clean(build_dir)
        return 0

    if args.action == "doctor":
        check_layout(args, verbose=True)
        return 0

    check_layout(args)

    if args.action in ("configure", "all"):
        configure(args, build_dir)

    if args.action in ("build", "all"):
        if args.action == "build" and not (build_dir / "CMakeCache.txt").exists():
            configure(args, build_dir)
        build(args, build_dir)

    if args.action == "run":
        run_example(build_dir)
    elif args.action == "all" and args.examples:
        run_example(build_dir)

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
