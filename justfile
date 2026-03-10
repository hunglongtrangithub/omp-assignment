cc := if os() == "macos" { "gcc-15" } else { "gcc" }

run:
    make clean; make CC={{ cc }}; ./main

test:
    zig build test
