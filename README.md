# Fuel-concurrent
Two simple concurrent programs, one uses semaphores and the other condition variables. This project was realized in the second year of University for the Operating Systems exam


# Problem explanation
This problem is a producer-consumer variant.
Fillmore produce tanks of fuel in group of N at once.
There are cars that go to Fillmore and take a single tank at a time and continue to take them until they go out of stock.
When this happen a car wakes Fillmore that produce N tanks again.
