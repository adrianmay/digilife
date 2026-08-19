For the letter guesser demo, some training data is prepared in here.

Ideally the input file "english.txt" should be split into a training and test set, but carefully because it contains essays from very different authors. Most authors have multiple essays in there so it should be possible to split the styles evenly.

I simplify the alphabet a bit and remove weird symbols, then replace the remaining characters with consecutive ones in ascii. This yields a language called Inglesh.

Then a bunch of calculations are done to show that:

* Some Inglesh characters characters are much commoner than others 
* Inglesh is rich enough that you could be twice as good at guessing the next character if you knew the previous two, assuming you're very familiar with the language.

The aim here is to grow a culture that predicts the next letter better than would be possible without knowing the preceding letters, and must therefore be using knowledge of the language.

