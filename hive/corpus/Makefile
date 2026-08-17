all: 1.histo 2.histo 3.histo

3.histo: 3.prob buckets.sh
	cat $< | ./buckets.sh > $@

3.prob: train.txt tripProb.sh
	cat $< | ./tripProb.sh  > $@

2.histo: 2.prob buckets.sh
	cat $< | ./buckets.sh > $@

2.prob: train.txt pairProb.sh
	cat $< | ./pairProb.sh  > $@

1.histo: train.txt letterProb.sh
	cat $< | ./letterProb.sh > $@

train.txt: simplified.txt consecufy.sh
	cat $< | ./consecufy.sh > $@

simplified.txt: english.txt simplify.sh
	cat $< | ./simplify.sh > $@

