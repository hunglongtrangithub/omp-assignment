const std = @import("std");
const c = @cImport({
    @cInclude("candy.h");
});

fn testFn(counts: []const u16, thresh: u16, expected_start: usize, expected_end: usize) !void {
    var home_start: usize = undefined;
    var home_end: usize = undefined;
    c.findBestHomeRange(
        counts.ptr,
        counts.len,
        thresh,
        &home_start,
        &home_end,
    );
    try std.testing.expectEqual(expected_start, home_start);
    try std.testing.expectEqual(expected_end, home_end);
    var home_start_parallel: usize = undefined;
    var home_end_parallel: usize = undefined;
    c.findBestHomeRangeParallel(
        counts.ptr,
        counts.len,
        thresh,
        &home_start_parallel,
        &home_end_parallel,
    );
    try std.testing.expectEqual(expected_start, home_start_parallel);
    try std.testing.expectEqual(expected_end, home_end_parallel);
}

test "basic case" {
    try testFn(&[_]u16{ 2, 4, 3, 2, 1 }, 10, 1, 4);
    try testFn(&[_]u16{ 0, 1, 4, 2, 4, 1, 0 }, 10, 2, 4);
}

test "threshold is zero" {
    try testFn(&[_]u16{ 2, 4, 3, 2, 1 }, 0, 5, 5);
}

test "threshold is larger than sum" {
    try testFn(&[_]u16{ 2, 4, 3, 2, 1 }, 20, 0, 4);
}

test "threshold is too small for any range" {
    try testFn(&[_]u16{ 10, 11, 12, 13, 14 }, 1, 5, 5);
}

test "prefer ranges with lower index if tie" {
    try testFn(&[_]u16{ 1, 0, 4, 0, 0 }, 5, 0, 2);
    try testFn(&[_]u16{ 0, 1, 0, 4, 0 }, 5, 0, 3);
    try testFn(&[_]u16{ 0, 0, 1, 0, 4 }, 5, 0, 4);
    try testFn(&[_]u16{ 2, 3, 2, 0, 3 }, 5, 0, 1);
    try testFn(&[_]u16{ 0, 1, 4, 2, 4, 1, 0 }, 5, 0, 2);
    try testFn(&[_]u16{ 0, 1, 4, 2, 4, 1, 0 }, 6, 2, 3);
}
