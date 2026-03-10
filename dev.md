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

Okay I completed the implementation and added some good unit tests. I also documented what the function thinks is best. Now I wanna think about how to parallelize this with OpenMP. So we can divide the work by splitting the candy_counts array into chunks, and each thread takes a chunk. We can let threads do their own thing, and in the end we have the best ranges with their corresponding candies total from each thread. We pick the one with highest total candies count, and if there are multiple with the same total, we pick the one with the lowest start home. This way we can get the best sequence of homes in parallel.

Wait but how do we handle the case where the best sequence of homes spans across two chunks? For example, if we have:

```
candies_thresh = 5
candy_counts = [0, 4, 0, 1, 0, 0]
```

and we split it into two chunks: `[0, 4, 0]` and `[1, 0, 0]`, the best range of homes is `[1, 3]`, which spans across the two chunks. We need to make sure that we consider sequences that span across the boundaries of the chunks. One way to do this is to have each thread not only compute the best sequence within its chunk but also keep track of the best sequence that starts in its chunk and extends into the next chunk. Then, when we combine the results from all threads, we can also consider the best sequences that span across the boundaries.

I need to think of a way to do that.
