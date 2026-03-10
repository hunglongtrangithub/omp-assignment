cc := if os() == "macos" { "gcc-15" } else { "gcc" }

run:
    make main_sequential CC=gcc; ./main_sequential

run-parallel:
    make main_parallel CC={{ cc }}; ./main_parallel

test:
    zig build test
