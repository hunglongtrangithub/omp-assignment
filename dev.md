So the problem is basically a maximum subarray sum problem. Since the number of candies given by a home can be zero, consider this scenario:

```
candies_thresh = 5
candy_counts = [0, 1, 0, 4, 0, 0]
```

The best sequence of homes would be `[1, 0, 4]`. We don't want to visit the zero-candies homes if possible.

Note that the parent can ban the child from receiving any candies (`candies_thresh == 0`), or a home can refuse to give the child any candies (`candy_counts[i] == 0`), so we need to be careful.

I would say visiting a house with no candies is wasteful, unless that house is between ones that do give you lots of candies. So we should aim for the shortest possible sequence that gives you the most possible number of candies. Is there a case that there are multiple of such sequences? Consider this scenario:

```
canies_thresh = 5
candy_counts = [0, 1, 4, 1, 0, 0]
```

For the sequence of homes, you can either do `[1, 2]` or `[2, 3]`. The assignment requires us to choose one with the lowest numbered start home, so we go with `[1, 2]` as the solution.

Also, home start can be equal to home end. The child is not prohibited from visiting only one home.
