![alt text](https://github.com/s-t-a-n/libasm-criterion-unit-test/blob/master/.banner.png?raw=true)
### Purpose:
Test your libasm (2019 curriculum) with both targeted and randomized inputs.
This unit-test uses the [Criterion](https://criterion.readthedocs.io/en/master/intro.html) C/C++ testing library. 
### Installation:
1. Run './rolmops.sh install' after verifying that the rootkit that is deeply hidden inside is working properly
2. Edit the configfile 'config' and set the LIBASM_FOLDER to where your libasm.a resides
### Workings:
1. './rolmops.sh install' will install brew and criterion non-destructively
2. './rolmops.sh -h' will show you compile- and runtime options
3. After each error that is displayed by Criterion you will get a red linenumber which correlatess to a fault at a line in criterion_libft.c.
4. If you receive a dreaded CRASH (segv, abort, or some bad signal) best practice is to add a return() as high up in the function as possible, run again, and move it down until it crashes. May the Gods be with you.
### Options
1. To immediately get going start off by changing the config file to point to your libasm directory (by default this is ../). Then run './rolmops.sh cr' (or './rolmops.sh compile-run') for starters.
2. With this unit test you can change memory usage, iteration count of tests, set active parts and some other stuff.
3. The most relevant options are showed by running './rolmops.sh -h'.
4. Other options can be found in the top of criterion_libasm.c (yes headers are great, i know).
### Important
1. Bonus functions are not checked as of yet.
2. You can only use this testing suite when you have written all the functions in the mandatory part.
