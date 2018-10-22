# Homework 2 #

## Implementing a look-aside cache ## 

### Tanmay Dubey ###

Separate chaining allows for hash tables with a high load factor, and saves space when the load factor is low. Open addressing saves memory when the hash table entries are small, and saves the time of allocating memory for each new entry record. I used separate chaining, since my hash table entries contain data as well as references to external storage, and thus are somewhat large. In such a situation, maintaining an array with a low load factor, as required of open addressing, is wasteful, since there will be many large empty slots. 

My FIFO implementation doesn't work if an element is removed and then immediately inserted (the element's order in the queue is not updated in that case). The problem here is finding a queue data structure with constant insertion to back, deletion from front, and random lookup. The data structure that comes the closest to supporting this is a dynamic array, which has deletion from back.

I used information in the following link to guide my development of the FIFO evictor: https://stackoverflow.com/questions/1403890/how-do-you-implement-a-class-in-c.

