CC = gcc
DBFLAGS = -g
BINARIES = Leaf_Counter Aggregate_Votes Vote_Counter

Vote_Counter: Leaf_Counter Aggregate_Votes
	$(CC) -o Vote_Counter Vote_Counter.c

Aggregate_Votes:
	$(CC) -o Aggregate_Votes Aggregate_Votes.c

Leaf_Counter: 
	$(CC) -o Leaf_Counter Leaf_Counter.c

clean:
	rm -f $(BINARIES)