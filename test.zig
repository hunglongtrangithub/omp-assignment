const std = @import("std");
const c = @cImport({
    @cInclude("candy.h");
});

test "basic case" {
    var home_start: usize = undefined;
    var home_end: usize = undefined;
    const candy_counts = &[_]u16{ 2, 4, 3, 2, 1 };
    c.findBestHomeRange(
        candy_counts,
        5,
        10,
        &home_start,
        &home_end,
    );
    try std.testing.expectEqual(1, home_start);
    try std.testing.expectEqual(4, home_end);
}
