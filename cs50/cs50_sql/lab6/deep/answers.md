# From the Deep

In this problem, you'll write freeform responses to the questions provided in the specification.

## Random Partitioning

Pros:
- Even distribution
- Easy to implement

Cons:
- Requires querying all boats to retrieve data

## Partitioning by Hour

Pros:
- Only one boat needs to be queried to retrieve data
- Easy to implement

Cons:
- Uneven distribution

## Partitioning by Hash Value

Pros:
- Even distribution
- Possible to query only one boat for specific time data

Cons:
- Must query all boats to retrieve data for a time range
- More complex to implement