all: 1.histo 2.histo 3.histo

%.histo: %.probs
	cat $< | ./buckets.sh > $@

perplexity 1.probs 2.probs 3.probs &:	stats train.txt
	cat train.txt | ./stats | tee perplexity

stats: stats.c
	gcc -lm -o stats stats.c

train.txt: simplified.txt consecufy.sh
	cat $< | ./consecufy.sh > $@

simplified.txt: english.txt simplify.sh Makefile
	cat $< | ./simplify.sh > $@

clean:
	rm -f *.histo *.probs perplexity simplified.txt train.txt stats

