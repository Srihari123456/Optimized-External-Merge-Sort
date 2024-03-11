# CS - 764

## Group 5:
Srihari Sridharan (sridharan9@wisc.edu) - 908 575 4456 \
Surendra Parla (smparla@wisc.edu) - 908 590 0984 \
Neha Chadaga (nchadaga@wisc.edu) - 908 587 6465

## Techniques Implemented in this Submission:

1. Quicksort - Quick sort is vital in internal sorting due to its efficient average-case time complexity, making it exceptionally fast for large datasets. Its divide-and-conquer strategy with low memory usage ensures swift sorting, often outperforming other algorithms like merge sort or insertion sort. It suits best in this particular case where we have a uniform random distribution.

2. Tournament Trees - In external sorting, tournament trees serve as efficient structures to handle the merging of sorted chunks from the disk, reducing comparisons and improving the merge step's efficiency. They optimize the selection of the next smallest or largest element when merging sorted chunks from external memory, minimizing I/O operations and enhancing overall sorting performance.

3. Offset-Value Coding - Offset-value codes decide many comparisons in a tree-of-losers priority queue. Column value comparisons are only required if two rows have equal offset-value codes. This reduces the number of necessary column value comparisons, making the process more efficient.

4. Minimum count of row and column comparisons - Tree-of-losers priority queues are more efficient than the traditional and better-known tree-of-winners priority queues because the former use only leaf-to-root passes with 𝑙𝑜𝑔<sub>2</sub>𝑁 comparisons, whereas the latter also need root-to-leaf passes with up to 2×𝑙𝑜𝑔<sub>2</sub>𝑁 comparisons.

5. Cache-size mini runs - Implementing quicksort on 1MB cache-sized chunks instead of 100MB in DRAM enhances spatial locality. By processing smaller, contiguous portions in cache, the algorithm capitalizes on cache efficiency, reducing memory access latency and cache misses. This spatially aware sorting strategy not only improves memory access performance  by aligning with the cache's characteristics but also mitigates the impact of larger data sets, ensuring more efficient sorting and minimizing delays associated
with accessing larger, slower DRAM.

6. Device-optimized page sizes - Optimizing pages across memory hierarchies minimizes bandwidth wastage by strategically managing data movement. This involves placing frequently accessed data in faster, closer-to-the-processor memory tiers and less frequently accessed data in larger, slower tiers. 
By aligning data with the memory hierarchy's speed and capacity characteristics, the system reduces unnecessary data transfers, enhancing the overall efficiency of the time taken to sort large data. This optimization ensures that the most crucial information is readily available in high-speed memory, minimizing the impact of memory latency and bandwidth constraints on system performance.

7. Spilling memory-to-SSD - Data is too large to be stored and processed in DRAM, which is why it is more efficient to spill the memory to SSD as they offer persistent storage.
By spilling excess data to SSD, the system avoids crashes due to insufficient memory, ensuring data integrity. This process helps maintain overall system stability.  
It helps in enabling systems to handle larger workloads that exceed the available physical RAM.

8. Spilling from SSD to disk - This process is part of a memory management strategy, optimizing data storage to prevent system slowdowns or crashes due to storage limitations while balancing the trade-offs between speed and capacity.

9. Verifying sets of rows & values - It is always possible that in the process of data transfer between memory levels and while sorting, bits might be corrupted or some duplicate data might be added to the final file. So it is necessary to have an efficient mechanism to check whether all the records in the input are present in the output after all operations.

10. Verifying sort order - Verifying the sort order in external sorting ensures the accuracy and correctness of the sorting process, confirming that the sorted output meets the expected order and criteria, which is crucial for data integrity and error detection, especially when dealing with vast amounts of data distributed across multiple storage devices. It acts as a validation step, enhancing reliability and confidence in the sorting results, and reducing the risk of errors or inconsistencies in the sorted data.

## Commands to Run
 ```
cd Code
make all
./sort -c "record_count" -s "record_size" -o trace0.txt > trace0.txt
 ```
for verification
```
cd TestScripts
g++ TestScript.cpp -o TestScript.o
./TestScript.o
 ```
in is the input directory \
out is the output directory
 
HDD_OUT.txt holds the final sort result
HDD.txt holds the data generated

## Project's State:
Completed.

## Contributions:
Srihari Sridharan - Tournament Tree, Offset Value Coding, Minimum count of row and column comparisons \
Surendra Parla - Spilling from SSD to disk, Spilling memory-to-SSD, Cache-size mini runs, Device-optimized page sizes \
Neha Chadaga - Internal sorting-Quicksort, Verifying sets of rows & values, Verifying sort order \
