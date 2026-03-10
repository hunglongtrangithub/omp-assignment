const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    const mod = b.addModule("omp_assignment", .{
        .root_source_file = b.path("test.zig"),
        .target = target,
    });

    mod.addCSourceFile(.{
        .file = b.path("candy.c"),
        .flags = &.{
            "-fopenmp",
            "-Wall",
            "-Wextra",
            "-Wpedantic",
            "-Wshadow",
            "-Wconversion",
            "-Werror",
            "-Wcast-qual",
            "-Wnull-dereference",
            "-std=c11",
        },
    });

    mod.addIncludePath(b.path("."));

    const mod_tests = b.addTest(.{
        .root_module = mod,
    });

    const run_mod_tests = b.addRunArtifact(mod_tests);

    const test_step = b.step("test", "Run tests");
    test_step.dependOn(&run_mod_tests.step);
}
