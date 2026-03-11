const std = @import("std");
const c = @cImport({
    @cInclude("candy.h");
});

fn testBuildPrefixSum(counts: []const u16, expected: []const u64) !void {
    const n = counts.len;
    const P = try std.testing.allocator.alloc(u64, n + 1);
    defer std.testing.allocator.free(P);

    c.buildPrefixSum(counts.ptr, n, P.ptr);

    try std.testing.expectEqual(0, P[0]);
    try std.testing.expectEqualSlices(u64, expected, P[1..]);
}

fn testLowerBound(P: []const u64, lo: usize, hi: usize, target: u64, expected: usize) !void {
    const result = c.lowerBound(P.ptr, lo, hi, target);
    try std.testing.expectEqual(expected, result);
}

fn testFindBestHomeRange(counts: []const u16, thresh: u16, expected_start: usize, expected_end: usize) !void {
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
    try testFindBestHomeRange(&[_]u16{ 2, 4, 3, 2, 1 }, 10, 1, 4);
    try testFindBestHomeRange(&[_]u16{ 0, 1, 4, 2, 4, 1, 0 }, 10, 2, 4);
}

test "threshold is zero" {
    try testFindBestHomeRange(&[_]u16{ 2, 4, 3, 2, 1 }, 0, 5, 5);
}

test "threshold is larger than sum" {
    try testFindBestHomeRange(&[_]u16{ 2, 4, 3, 2, 1 }, 20, 0, 4);
}

test "threshold is too small for any range" {
    try testFindBestHomeRange(&[_]u16{ 10, 11, 12, 13, 14 }, 1, 5, 5);
}

test "prefer ranges with lower index if tie" {
    try testFindBestHomeRange(&[_]u16{ 1, 0, 4, 0, 0 }, 5, 0, 2);
    try testFindBestHomeRange(&[_]u16{ 0, 1, 0, 4, 0 }, 5, 0, 3);
    try testFindBestHomeRange(&[_]u16{ 0, 0, 1, 0, 4 }, 5, 0, 4);
    try testFindBestHomeRange(&[_]u16{ 2, 3, 2, 0, 3 }, 5, 0, 1);
    try testFindBestHomeRange(&[_]u16{ 0, 1, 4, 2, 4, 1, 0 }, 5, 0, 2);
    try testFindBestHomeRange(&[_]u16{ 0, 1, 4, 2, 4, 1, 0 }, 6, 2, 3);
}

test "buildPrefixSum basic" {
    try testBuildPrefixSum(&[_]u16{ 2, 4, 3, 2, 1 }, &[_]u64{ 2, 6, 9, 11, 12 });
    try testBuildPrefixSum(&[_]u16{ 1, 2, 3 }, &[_]u64{ 1, 3, 6 });
    try testBuildPrefixSum(&[_]u16{5}, &[_]u64{5});
    try testBuildPrefixSum(&[_]u16{ 0, 0, 0 }, &[_]u64{ 0, 0, 0 });
}

test "buildPrefixSum empty" {
    try testBuildPrefixSum(&[_]u16{}, &[_]u64{});
}

test "lowerBound basic" {
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 0, 0);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 1, 0);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 2, 1);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 3, 1);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 6, 2);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 7, 3);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 15, 4);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 0, 5, 16, 5);
}

test "lowerBound subrange" {
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 2, 5, 0, 2);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 2, 5, 5, 2);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 2, 5, 6, 2);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 2, 5, 10, 3);
    try testLowerBound(&[_]u64{ 1, 3, 6, 10, 15 }, 2, 5, 16, 5);
}

test "lowerBound with duplicates" {
    try testLowerBound(&[_]u64{ 1, 1, 1, 3, 3, 5 }, 0, 6, 1, 0);
    try testLowerBound(&[_]u64{ 1, 1, 1, 3, 3, 5 }, 0, 6, 2, 3);
    try testLowerBound(&[_]u64{ 1, 1, 1, 3, 3, 5 }, 0, 6, 3, 3);
    try testLowerBound(&[_]u64{ 1, 1, 1, 3, 3, 5 }, 0, 6, 4, 5);
    try testLowerBound(&[_]u64{ 1, 1, 1, 3, 3, 5 }, 2, 5, 1, 2);
}
